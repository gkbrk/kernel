#include <kernel/memory/PageFrameAllocator.h>

static PageFrameAllocator *s_page_frame_allocator;

void init_page_frame_allocator(void *start_addr, void *end_addr) {
  s_page_frame_allocator = new PageFrameAllocator(start_addr, end_addr);
}

PageFrameAllocator *get_page_frame_allocator() {
  return s_page_frame_allocator;
}