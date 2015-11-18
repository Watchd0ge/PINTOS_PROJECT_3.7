#include "vm/frame.h"
#include "vm/page.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "devices/block.h"
#include "filesys/inode.h"
#include <stdbool.h>

static unsigned page_hash_func (const struct hash_elem *h_elem, void *aux UNUSED)
{
  struct page *pg = hash_entry(h_elem, struct page, elem);
  return hash_int((int) pg->user_addr);
}

static bool page_less_func (const struct hash_elem *h_elem_a, const struct hash_elem *h_elem_b, void *aux UNUSED)
{
  struct page *pga = hash_entry(h_elem_a, struct page, elem);
  struct page *pgb = hash_entry(h_elem_b, struct page, elem);
  if (pga->user_addr < pgb->user_addr)
    {
      return true;
    }
  return false;
}

void sup_page_table_init (struct hash * h) {
  hash_init (h, page_hash_func, page_less_func, NULL);
}

/* Takes a frame and maps it to a page.
   Returns the newly created page */
bool create_file_page(struct file *file, int32_t ofs, uint8_t *upage, uint32_t read_bytes, uint32_t zero_bytes, bool writable)
{
  struct page * sup_pt_entry = malloc (sizeof(struct page));
  if (sup_pt_entry == NULL) {
    PANIC ("NOT ENOUGH KERNEL MEMORY\n");
  } else {
    sup_pt_entry->loc         = FILE;
    sup_pt_entry->user_addr   = upage;
    sup_pt_entry->phys_addr   = NULL;
    sup_pt_entry->file        = file;
    sup_pt_entry->offset      = ofs;
    sup_pt_entry->read_bytes  = read_bytes;
    sup_pt_entry->zero_bytes  = zero_bytes;
    sup_pt_entry->writable    = writable;
    return (hash_insert(&thread_current()->spt, &sup_pt_entry->elem) == NULL); // NULL means successful entry
  }
}

// bool add_file_to_page_table (struct file *file, int32_t ofs, uint8_t *upage,
// 			     uint32_t read_bytes, uint32_t zero_bytes,
// 			     bool writable)
// {
//   struct sup_page_entry *spte = malloc(sizeof(struct sup_page_entry));
//   if (!spte)
//     {
//       return false;
//     }
//   spte->file = file;
//   spte->offset = ofs;
//   spte->uva = upage;
//   spte->read_bytes = read_bytes;
//   spte->zero_bytes = zero_bytes;
//   spte->writable = writable;
//   spte->is_loaded = false;
//   spte->type = FILE;
//   spte->pinned = false;
//
//   return (hash_insert(&thread_current()->spt, &spte->elem) == NULL);
// }

// void insert_page (struct page * upage)
// {
//   struct thread *t = thread_current();
//   printf ("IT FAILED HERE 0\n");
//   hash_insert (&t->spt.pages, &upage->hash_elem);
// }

// void map_frame_to_page(void *addr, void *frame)
// {
//   struct page * upage = page_lookup ((void *)((uint32_t)addr & (~PGMASK)));
//   upage -> kaddr = frame;
// }

/* Used only when file read bytes == PGSIZE */
// struct page * create_unmapped_page(void *addr, uint8_t flags)
// {
//    struct thread *t = thread_current();
//    struct page * upage = malloc(sizeof(struct page));
//    upage -> addr = (void *)((uint32_t)addr & (~PGMASK));
//    upage -> flags = flags;
//    hash_insert (&t->spt, &upage->hash_elem);
//    return upage;
// }

// struct page * map_page_to_frame (void *addr, int flags)
// {
//   struct thread *t = thread_current();
//   /*Create a new page*/
//   if (flags == NEW_PAGE)
//     {
//       struct page * upage = malloc (sizeof(struct page));
//       upage -> addr = (void *)((uint32_t)addr & (~PGMASK));
//       upage -> dirty = 0;
//       upage -> accessed = 0;
//       upage -> flags = NEW_PAGE;
//       /*Insert page into supplemental page table*/
//       hash_insert (&t->spt, &upage->hash_elem);
//       /*Create a frame */
//       void *kpage = get_frame(flags);
//       if (kpage)
// 	{
// 	  pagedir_set_page (t->pagedir, upage->addr, kpage, true);
//       	}
//       else
// 	{
// 	  /*Shouldn't happen as frame eviction is taken care of in get_frame*/
// 	}
//       return upage;
//     }
//   else
//     {
//       return 0;
//     }
// }

// void set_page_accessed(struct page *page)
// {
//   page->accessed = 1;
// }
//
// void set_page_dirty(struct page * page)
// {
//   page->dirty = 1;
// }
//
// unsigned page_hash (const struct hash_elem *p_, void *aux UNUSED)
// {
//   const struct page *p = hash_entry (p_, struct page, hash_elem);
//   return hash_bytes (&p->addr, sizeof p->addr);
// }
//
// bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
//                 void *aux UNUSED)
// {
//   const struct page *a = hash_entry (a_, struct page, hash_elem);
//   const struct page *b = hash_entry (b_, struct page, hash_elem);
//
//   return a->addr < b->addr;
// }
//
// struct page * page_lookup (void *address)
// {
//   struct thread *t = thread_current();
//   struct page p;
//   struct hash_elem *e;
//   p.addr = (void *)((uint32_t)address & (~PGMASK));
//   e = hash_find (t->spt.pages, &p.hash_elem);
//   return e != NULL ? hash_entry (e, struct page, hash_elem) : NULL;
// }

/* Install_page without actually reading data from disk */
// bool
// mark_page (void *upage, uint8_t *addr,
//            size_t length, block_sector_t sector_no)
// {
//   struct thread *t = thread_current ();
//
//   if (pagedir_get_page (t->pagedir, upage) != NULL)
//     return false;
//
//   return sup_pt_add (t->pagedir, upage, addr, length, sector_no) != NULL;
// }

// /* Create an entry to sup_pt, according to the given info */
// struct page *
// sup_pt_add (uint32_t *pd, void *upage, uint8_t *vaddr, size_t length, block_sector_t sector_no)
// {
//
//   /* Allocate page_struct, i.e., a new entry in sup_pt */
//   struct page *pg = create_page (upage);
//   if (pg == NULL)
//     return NULL;
//
//   insert_page (pg);
//
//   return pg;
//   /* Fill in sup_pt entry info */
// }
