#include <hash.h>
#include <list.h>
#include "frame.h"
#include <stdio.h>
#include "userprog/pagedir.h"
#include "threads/thread.h"
#include "threads/pte.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/init.h"

#include <stdio.h>
#define NO_OWNER -1

struct frame * find_in_frame_list (void *upage, tid_t tid);

/* Frame Table */
struct list frame_list;
struct lock frame_list_lock;

/* Lock for frame eviction */
struct lock evict_lock;

/* Initialize supplemental page table and frame table */
void
init_frametable (void)
{
  list_init (&frame_list);
  lock_init (&frame_list_lock);
}

void
store_frame (struct frame *f) {
  list_push_back (&frame_list, &f->elem);
}

struct frame *
allocate_frame (void *upage) {
  struct frame *fs = malloc (sizeof (struct frame));
  if (fs != NULL) {
    fs->phys_addr = palloc_get_page (PAL_USER | PAL_ZERO);
    if (fs->phys_addr != NULL) {
      fs->user_addr = upage;
      store_frame (fs);
      return fs;
    } else {
      free (fs);
      return find_in_frame_list (upage, tid);
    }
  } else {
    return NULL;
  }
}

struct frame *
find_in_frame_list (void *upage, tid_t tid) {
  struct list_elem * next  = list_begin (&frame_list);
  struct frame *fs = NULL;
  while (next != list_tail(&frame_list)) {
    fs = list_entry (next, struct frame, elem);
    if (fs->user_addr == NULL) {
      list_remove (next);
      break;
    } else {
      next = list_next (next);
    }
  }

  /* Allocate the available frame */
  if (next == list_tail(&frame_list)) {
    // TODO: EVICTION
    PANIC ("THERE ARE NO FREE PAGES IN THE FRAME TABLE\n");
  } else {
    list_push_back (&frame_list, &fs->elem);
    fs->user_addr = upage;
    // fs->owner = tid;
    return fs;
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
      fs->owner = NO_OWNER;
      fs->user_addr = NULL;
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
