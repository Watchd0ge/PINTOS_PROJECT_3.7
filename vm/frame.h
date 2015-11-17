#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include <stdint.h>
#include <list.h>
#include <hash.h>
#include "devices/block.h"
#include "threads/synch.h"
#include "threads/thread.h"

/* Position of a frame */
#define POS_SWAP 		0x1
#define POS_DISK		0x2
#define POS_MEM			0x3
#define POSBITS			0x3
#define POSMASK			~POSBITS

/* Content type of a frame */
#define TYPE_Executable 	0x4
#define TYPE_MMFile 		0x8
#define TYPE_Stack		0xc
#define TYPEBITS		0xc
#define TYPEMASK		~TYPEBITS

/* Property bits of a frame */
#define FS_READONLY		0x10
#define FS_DIRTY		0x20
#define FS_ACCESS		0x40
#define FS_ZERO			0x80

#define FS_PINNED		0x10000

#define SECTOR_ERROR		SIZE_MAX

/* A frame structure corresponds to exactly one frame,
   tracking the frame whether it on memeory, disk, or swap.
   Unit structure making up frame table */
struct frame
{
  // which Thread
  // which physical Address
  // which user address
  void *phys_addr;              /* Which physical address this frame refers to */
  void *v_addr;                 /* Which virtual address this maps to */
  tid_t *owner;         /* Thread which owns this page */
  struct list_elem elem;
};

/* A page structure corresponds to on user virtual page,
   it is specific to each process, and maybe more than one page
   strucutre point to a single frame.
   Unit structure making up supplemental page table */
struct page_struct
{
  uint32_t key;
  struct frame_struct *fs;
  struct hash_elem elem;
};

typedef struct {
    struct frame *f_ptr; // or char strVal[20];
    void *addr;
} tTuple;

void init_frametable (void);

void store_frame (struct frame *);

void *allocate_frame (void *upage, tid_t tid);

void deallocate_frame (void *phys_addr);

#endif /* vm/frame.h */
