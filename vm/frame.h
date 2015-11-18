#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include <stdint.h>
#include <list.h>
#include <hash.h>
#include "devices/block.h"
#include "threads/synch.h"
#include "threads/thread.h"

struct frame
{
  void *phys_addr;              /* Which physical address this frame refers to */
  void *user_addr;                 /* Which virtual address this maps to */
  int owner;                 /* Thread which owns this page */
  struct list_elem elem;
};

void init_frametable (void);

void store_frame (struct frame *);

struct frame *allocate_frame (void *upage, void *tid);

void deallocate_frame (void *phys_addr);

int frame_list_size (void);

#endif /* vm/frame.h */
