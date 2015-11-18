#ifndef PAGE_H
#define PAGE_H

#include <hash.h>
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/off_t.h"
#include "filesys/inode.h"
#include "devices/block.h"
#include "vm/frame.h"

enum location {
  FILE,
  SWAP,
  FRAME
};

struct page {
  enum location loc;
  void *user_addr;

  // If in frame
  void *phys_addr;

  // If in file
  struct file *file;
  size_t offset;
  size_t read_bytes;
  size_t zero_bytes;

  bool writable;
  struct hash_elem elem;
};

bool create_file_page (struct file *file, int32_t ofs, uint8_t *upage, uint32_t read_bytes, uint32_t zero_bytes, bool writable);
void sup_page_table_init (struct hash * h_table);
struct page *get_spte (uint8_t *upage);

void map_page_to_frame (struct page *, struct frame *);

#endif /* vm/page.h */

// #ifndef VM_PAGE_H
// #define VM_PAGE_H
//
// #include <hash.h>
// #include "vm/frame.h"
//
// #define FILE 0
// #define SWAP 1
// #define MMAP 2
// #define HASH_ERROR 3
//
// // 256 KB
// #define MAX_STACK_SIZE (1 << 23)
//
// struct sup_page_entry {
//   uint8_t type;
//   void *uva;
//   bool writable;
//
//   bool is_loaded;
//   bool pinned;
//
//   // For files
//   struct file *file;
//   size_t offset;
//   size_t read_bytes;
//   size_t zero_bytes;
//
//   // For swap
//   size_t swap_index;
//
//   struct hash_elem elem;
// };
//
// void page_table_init (struct hash *spt);
// void page_table_destroy (struct hash *spt);
//
// bool load_page (struct sup_page_entry *spte);
// bool load_mmap (struct sup_page_entry *spte);
// bool load_swap (struct sup_page_entry *spte);
// bool load_file (struct sup_page_entry *spte);
// bool add_file_to_page_table (struct file *file, int32_t ofs, uint8_t *upage,
// 			     uint32_t read_bytes, uint32_t zero_bytes,
// 			     bool writable);
// bool add_mmap_to_page_table(struct file *file, int32_t ofs, uint8_t *upage,
// 			    uint32_t read_bytes, uint32_t zero_bytes);
// bool grow_stack (void *uva);
// struct sup_page_entry* get_spte (void *uva);

// #endif /* vm/page.h */

// #ifndef PAGE_H
// #define PAGE_H
//
// #include <hash.h>
// #include "filesys/file.h"
// #include "filesys/filesys.h"
// #include "filesys/off_t.h"
// #include "filesys/inode.h"
// #include "devices/block.h"
//
// #define NEW_PAGE 1
// #define UNMAPPED_PAGE 2
// #define ZERO_PAGE 4
// #define FILE_READ_PAGE 8
// #define FILE_READ_PARTIAL 16
// #define SETUP_STACK 32
// /* Very similar to the frame table entries
//    Each thread has its own supplemental page table (spt)
//    spt keeps track of all pages accessed/modified by thread
// */
//
// enum location{
//     FRAME,
//     SWAP,
//     DISK
// };
//
// struct sup_page_table {
//    /* Our SPT is a hash table containing pages */
//    struct hash *pages;
// };
//
// /* Each page contains the address of the actual page
//    and the associated bits. i.e. it is actually a pte
// */
// struct page
// {
//    struct hash_elem hash_elem;
//    /* User Virtual Address of actual page */
//    void *addr;
//    /* Kernel virtual address of the frame*/
//    void *kaddr;
//    /* Dirty set when page modified*/
//    uint8_t dirty;
//    /* Set when page has been read/written after creation */
//    uint8_t accessed;
//   //  uint8_t flags;
//    /* Thread owner */
//    struct thread * owner;
//    /* pointer to a file (if any) the page will access*/
//    struct file * file;
//    off_t ofs;
//    /* location of page */
//    enum location loc;
//    bool writable;
// };
//
//  // struct page * create_page(void *addr, int flags);
//  struct page * create_page(void *addr, enum location);
//  struct page * create_unmapped_page (void *addr, uint8_t flags);
//  void insert_page (struct page * upage);
//  void map_frame_to_page (void *addr, void *frame);
//  struct page * map_page_to_frame (void *addr, int flags);
//  void set_page_accessed (struct page * page);
//  void set_page_dirty (struct page * page);
//
//  unsigned page_hash (const struct hash_elem *p_, void *aux);
//  bool page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux);
//  struct page * page_lookup (void *address);
//
//  bool mark_page (void *upage, uint8_t *addr, size_t length, block_sector_t sector_no);
//
// #endif
