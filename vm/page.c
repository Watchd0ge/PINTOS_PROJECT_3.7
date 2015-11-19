#include "vm/frame.h"
#include "vm/page.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "devices/block.h"
#include "filesys/inode.h"
#include <stdbool.h>
#include <hash.h>
#include <string.h>

static bool install_page (void *upage, void *kpage, bool writable);
void map_page_to_frame (struct page *pg, struct frame *fs);

static unsigned
page_hash_func (const struct hash_elem *h_elem, void *aux UNUSED)
{
  struct page *pg = hash_entry(h_elem, struct page, elem);
  return hash_int((int) pg->user_addr);
}

static bool
page_less_func (const struct hash_elem *h_elem_a, const struct hash_elem *h_elem_b, void *aux UNUSED)
{
  struct page *pga = hash_entry(h_elem_a, struct page, elem);
  struct page *pgb = hash_entry(h_elem_b, struct page, elem);
  if (pga->user_addr < pgb->user_addr)
    {
      return true;
    }
  return false;
}

void
sup_page_table_init (struct hash * h) {
  hash_init (h, page_hash_func, page_less_func, NULL);
}

bool
create_file_page(struct file *file, int32_t ofs, uint8_t *upage, uint32_t read_bytes, uint32_t zero_bytes, bool writable)
{
  struct page * sup_pt_entry = malloc (sizeof(struct page));
  if (sup_pt_entry == NULL) {
    PANIC ("NOT ENOUGH KERNEL MEMORY\n");
  } else {
    sup_pt_entry->user_addr   = upage;
    sup_pt_entry->loc         = FILE;
    sup_pt_entry->fs          = NULL;
    sup_pt_entry->phys_addr   = NULL;
    sup_pt_entry->file        = file;
    sup_pt_entry->offset      = ofs;
    sup_pt_entry->read_bytes  = read_bytes;
    sup_pt_entry->zero_bytes  = zero_bytes;
    sup_pt_entry->writable    = writable;
    return (hash_insert(&thread_current()->spt, &sup_pt_entry->elem) == NULL); // NULL means successful entry
  }
}

struct page *
get_spte (void *upage)
{
  struct page pg;
  struct hash_elem *e;

  pg.user_addr = upage;
  e = hash_find (&thread_current()->spt, &pg.elem);
  return e != NULL ? hash_entry (e, struct page, elem) : NULL;
}

void
map_page_to_frame (struct page *pg, struct frame *fs)
{
  /* ADD THE UPAGE TO THE FRAME */
  fs->user_addr = pg->user_addr;

  /* UPDATE THE PAGE ENTRY TO REFLECT MAPPING TO A FRAME */
  pg->loc = FRAME;
  pg->phys_addr = fs->phys_addr;
  pg->file = NULL;
  pg->offset = 0;
  pg->read_bytes = 0;
  pg->zero_bytes = 0;
}


bool load_file (struct page *spte)
{
  // uint8_t *frame = frame_alloc(flags, spte);
  struct frame * fs = allocate_frame(spte->user_addr);
  if (!fs)
    {
      PANIC ("NO FRAMES!!!\n");
      return false;
    }
  if (spte->read_bytes > 0)
    {
      if ((int) spte->read_bytes != file_read_at(spte->file, fs->phys_addr, spte->read_bytes, spte->offset))
        {
          deallocate_frame(fs->phys_addr);
          return false;
        }
      memset(fs->phys_addr + spte->read_bytes, 0, spte->zero_bytes);
    }

  if (!install_page(spte->user_addr, fs->phys_addr, spte->writable))
    {
      deallocate_frame(fs->phys_addr);
      return false;
    }
  map_page_to_frame (spte, fs);
  return true;
}

static bool
install_page (void *upage, void *kpage, bool writable)
{
  struct thread *t = thread_current ();

  /* Verify that there's not already a page at that virtual
     address, then map our page there. */
  return (pagedir_get_page (t->pagedir, upage) == NULL
          && pagedir_set_page (t->pagedir, upage, kpage, writable));
}
