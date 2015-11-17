#include <hash.h>
#include <list.h>
#include "frame.h"
// #include "swap.h"
#include "userprog/pagedir.h"
#include "threads/thread.h"
#include "threads/pte.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/init.h"

#include <stdio.h>

/* Supplemental Page Table is global. */
struct hash sup_pt;
struct lock sup_pt_lock;

/* Frame Table */
struct list frame_list;
struct lock frame_list_lock;

/* Lock for frame eviction */
struct lock evict_lock;

/* "Hand" in clock algorithm for frame eviction */
struct frame_struct* evict_pointer;

/* Initialize supplemental page table and frame table */
void
init_frametable (void)
{
  //hash_init (&sup_pt, sup_pt_hash_func, sup_pt_less_func, NULL);
  list_init (&frame_list);
  //lock_init (&sup_pt_lock);
  lock_init (&frame_list_lock);
  //lock_init (&evict_lock);
  //evict_pointer = NULL;
}

void
store_frame (struct frame *f) {
  list_push_back (&frame_list, &f->elem);
}

void *
allocate_frame (void *upage, tid_t tid) {
  struct list_elem * next  = list_begin (&frame_list);
  struct frame *fs = NULL;

  /* Find an available frame */
  while (next != NULL) {
    fs = list_entry (next, struct frame, elem);
    if (fs->owner == NULL && fs->v_addr == NULL) {
      list_remove (next);
      break;
    } else {
      next = list_next (next);
    }
  }

  /* Allocate the available frame */
  if (next == NULL) {
    PANIC ("THERE ARE NO FREE PAGES IN THE FRAME TABLE\n");
  } else {
    list_push_back (&frame_list, &fs->elem);
    fs->v_addr = upage;
    fs->owner = tid;
    return fs->phys_addr;
  }
}

void
deallocate_frame (void *kpage) {
  struct list_elem * next  = list_begin (&frame_list);
  struct frame *fs = NULL;

  /* Find the frame */
  while (next != NULL) {
    fs = list_entry (next, struct frame, elem);
    if (fs->phys_addr == kpage) {
      fs->owner = NULL;
      fs->v_addr = NULL;
      return;
    } else {
      next = list_next (next);
    }
  }
  if (next == NULL) { PANIC ("TRIED TO DEALLOCATE FRAME BUT FAILED!!!\n");}
}

int
frame_list_size () {
  return list_size(&frame_list);
}
