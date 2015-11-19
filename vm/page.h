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
  // Related user address
  void *user_addr;

  // Indication of where the page is located
  enum location loc;

  // If in frame
  struct frame *fs;
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
struct page *get_spte (void *upage);

// void map_page_to_frame (struct page *, struct frame *);
bool load_file (struct page *spte);

#endif /* vm/page.h */
