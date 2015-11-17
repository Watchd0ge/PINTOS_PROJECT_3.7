#include "vm/frame.h"
#include "vm/page.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include <stdbool.h>

/* Implements supplemental page table
   Code for hash table functionality taken from A.8.5 in Pintos
   reference
*/

/* Takes a frame and maps it to a page.
   Returns the newly created page */
struct page * create_page(void *addr, int flags)
{
  struct page * upage = malloc (sizeof(struct page));
  upage -> addr = (void *)(((uint32_t)addr) & (~PGMASK));
  upage -> dirty = 0;
  upage -> accessed = 0;
  upage -> flags = flags;
//  hash_insert (&t->sup_page_table, &kpage->hash_elem);
  return upage;
}

void insert_page (struct page * upage)
{
  struct thread *t = thread_current();
  hash_insert (&t->sup_page_table, &upage->hash_elem);
}

void map_frame_to_page(void *addr, void *frame)
{
  struct page * upage = page_lookup ((void *)((uint32_t)addr & (~PGMASK)));
  upage -> addr = frame;
}

/* Used only when file read bytes == PGSIZE */
struct page * create_unmapped_page(void *addr, uint8_t flags)
{
   struct thread *t = thread_current();
   struct page * upage = malloc(sizeof(struct page));
   upage -> addr = (void *)((uint32_t)addr & (~PGMASK));
   upage -> flags = flags;
   hash_insert (&t->sup_page_table, &upage->hash_elem);
   return upage;
}

struct page * map_page_to_frame (void *addr, int flags)
{
  struct thread *t = thread_current();
  /*Create a new page*/
  if (flags == NEW_PAGE)
    {
      struct page * upage = malloc (sizeof(struct page));
      upage -> addr = (void *)((uint32_t)addr & (~PGMASK));
      upage -> dirty = 0;
      upage -> accessed = 0;
      upage -> flags = NEW_PAGE;
      /*Insert page into supplemental page table*/
      hash_insert (&t->sup_page_table, &upage->hash_elem);
      /*Create a frame */
      void *kpage = get_frame(flags);
      if (kpage)
	{
	  pagedir_set_page (t->pagedir, upage->addr, kpage, true);
      	}
      else
	{
	  /*Shouldn't happen as frame eviction is taken care of in get_frame*/
	}
      return upage;
    }
  else
    {
      return 0;
    }
}

void set_page_accessed(struct page *page)
{
  page->accessed = 1;
}

void set_page_dirty(struct page * page)
{
  page->dirty = 1;
}

unsigned page_hash (const struct hash_elem *p_, void *aux UNUSED)
{
  const struct page *p = hash_entry (p_, struct page, hash_elem);
  return hash_bytes (&p->addr, sizeof p->addr);
}

bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
                void *aux UNUSED)
{
  const struct page *a = hash_entry (a_, struct page, hash_elem);
  const struct page *b = hash_entry (b_, struct page, hash_elem);

  return a->addr < b->addr;
}

struct page * page_lookup (void *address)
{
  struct thread *t = thread_current();
  struct page p;
  struct hash_elem *e;
  p.addr = (void *)((uint32_t)address & (~PGMASK));
  e = hash_find (&t->sup_page_table, &p.hash_elem);
  return e != NULL ? hash_entry (e, struct page, hash_elem) : NULL;
}

/* Install_page without actually reading data from disk */
bool
mark_page (void *upage, uint8_t *addr,
           size_t length, uint32_t flag,
           block_sector_t sector_no)
{
  struct thread *t = thread_current ();

  if (pagedir_get_page (t->pagedir, upage) != NULL)
    return false;

  return sup_pt_add (t->pagedir, upage, addr, length, flag, sector_no)
         != NULL;
}

/* Create an entry to sup_pt, according to the given info */
struct page_struct *
sup_pt_add (uint32_t *pd, void *upage, uint8_t *vaddr, size_t length,
            uint32_t flag, block_sector_t sector_no)
{
  /* Find pte */
  uint32_t *pte = sup_pt_pte_lookup (pd, upage, true);

  /* Allocate page_struct, i.e., a new entry in sup_pt */
  struct page_struct *ps =
    (struct page_struct*) malloc (sizeof (struct page_struct));
  if (ps == NULL)
    return NULL;

  /* Fill in sup_pt entry info */
  ps->key = (uint32_t) pte;
  ps->fs = malloc (sizeof (struct frame_struct));

  if (ps->fs == NULL)
  {
    free (ps);
    return NULL;
  }

  lock_init (&ps->fs->frame_lock);
  lock_acquire (&ps->fs->frame_lock);
  ps->fs->vaddr = vaddr;
  ps->fs->length = length;
  ps->fs->flag = flag;
  ps->fs->sector_no = sector_no;
  list_init (&ps->fs->pte_list);

  /* Register the page itself to pte_list of frame_struct */
  struct pte_shared *pshr =
    (struct pte_shared *)malloc (sizeof (struct pte_shared));
  if (pshr == NULL)
  {
    free (ps->fs);
    free (ps);
    return NULL;
  }
  pshr->pte = pte;
  list_push_back (&ps->fs->pte_list, &pshr->elem);
  lock_release (&ps->fs->frame_lock);

  /* Register at supplemental page table */
  lock_acquire (&sup_pt_lock);
  hash_insert (&sup_pt, &ps->elem);
  lock_release (&sup_pt_lock);

  /* Register at frame table */
  lock_acquire (&frame_list_lock);
  list_push_back (&frame_list, &ps->fs->elem);
  lock_release (&frame_list_lock);

  return ps;
}
