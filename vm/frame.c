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
