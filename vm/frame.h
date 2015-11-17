#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include <stdint.h>
#include <list.h>
#include <hash.h>
#include "devices/block.h"
#include "threads/synch.h"

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
struct frame_struct
{
  uint32_t flag;                /* Flag bits */
  uint8_t *vaddr;               /* Virtual address if on memory */
  size_t length;                /* Length of meaningful contents */
  block_sector_t sector_no;     /* Sector # if on disk or swap */
  struct lock frame_lock;       /* Lock for protecting data in frame */
  struct list pte_list;         /* A list of pte's representing
                                   user pages sharing this frame */
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

void sup_pt_init (void);
