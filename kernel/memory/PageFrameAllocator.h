#ifndef CARDINAL_PAGEFRAMEALLOCATOR_H
#define CARDINAL_PAGEFRAMEALLOCATOR_H

#include <libk/Noncopyable.h>
#include <libk/assert.h>
#include <libk/string.h>
#include <stddef.h>

constexpr size_t PAGE_SIZE = 4096;

static void *align_up(void *addr) {
  auto addr_intptr = reinterpret_cast<size_t>(addr);
  auto page_size_mask = PAGE_SIZE - 1;
  auto aligned_addr = (addr_intptr + page_size_mask) & ~page_size_mask;
  return reinterpret_cast<void *>(aligned_addr);
}

static void *align_down(void *addr) {
  auto addr_intptr = reinterpret_cast<size_t>(addr);
  auto page_size_mask = PAGE_SIZE - 1;
  auto aligned_addr = addr_intptr & ~page_size_mask;
  return reinterpret_cast<void *>(aligned_addr);
}

class PageFrameAllocator {
  MAKE_NONCOPYABLE(PageFrameAllocator);
  MAKE_NONMOVABLE(PageFrameAllocator);

public:
  PageFrameAllocator(void *start_addr, void *end_addr)
      : m_start_addr(align_up(start_addr)), m_end_addr(align_down(end_addr)) {
    ASSERT(m_start_addr < m_end_addr);

    // Calculate the number of pages in the memory range.
    auto num_pages = (reinterpret_cast<size_t>(m_end_addr) -
                      reinterpret_cast<size_t>(m_start_addr)) /
                     PAGE_SIZE;

    // How many pages are needed to store the bitmap?
    m_bitmap_page_count = num_pages / PAGE_SIZE;
    if (num_pages % PAGE_SIZE != 0) {
      m_bitmap_page_count++;
    }

    // Zero out the bitmap.
    for (size_t i = 0; i < num_pages; i++) {
      auto *bit = reinterpret_cast<uint8_t *>(m_start_addr);
      bit[i] = 0;
    }

    m_page_count = num_pages - m_bitmap_page_count;
    ASSERT(m_page_count > 0);
    ASSERT(m_page_count < num_pages);
  }

  void *page_to_addr(size_t page) {
    ASSERT(page < m_page_count);

    // Skip the bitmap pages.
    auto start_addr = reinterpret_cast<size_t>(m_start_addr);
    auto addr = start_addr + (page + m_bitmap_page_count) * PAGE_SIZE;
    return reinterpret_cast<void *>(addr);
  }

  size_t addr_to_page(void *addr) {
    ASSERT(addr >= m_start_addr);
    ASSERT(addr < m_end_addr);

    // Skip the bitmap pages.
    auto start_addr = reinterpret_cast<size_t>(m_start_addr);
    auto page = (reinterpret_cast<size_t>(addr) - start_addr) / PAGE_SIZE;
    return page - m_bitmap_page_count;
  }

  void *allocate_page() {
    auto *bits = reinterpret_cast<uint8_t *>(m_start_addr);

    for (size_t i = 0; i < m_page_count; i++) {
      auto is_allocated = bits[i];
      if (!is_allocated) {
        bits[i] = 1;
        return page_to_addr(i);
      }
    }

    ASSERT_NOT_REACHED;
    return nullptr;
  }

  void *allocate_pages(size_t num_pages) {
    auto *bits = reinterpret_cast<uint8_t *>(m_start_addr);

    for (size_t i = 0; i < m_page_count; i++) {
      auto is_allocated = bits[i];
      if (!is_allocated) {
        bool found = true;
        for (size_t j = 0; j < num_pages; j++) {
          if (bits[i + j]) {
            found = false;
            break;
          }
        }

        if (found) {
          for (size_t j = 0; j < num_pages; j++) {
            bits[i + j] = 1;
          }

          return page_to_addr(i);
        }
      }
    }
  }

  void free_page(void *page) {
    auto page_index = addr_to_page(page);
    auto *bits = reinterpret_cast<uint8_t *>(m_start_addr);
    ASSERT(bits[page_index] == 1);
    bits[page_index] = 0;
  }

  size_t allocatedPages() const {
    size_t total = 0;
    auto *bits = reinterpret_cast<uint8_t *>(m_start_addr);
    for (size_t i = 0; i < m_page_count; i++) {
      auto is_allocated = bits[i];
      if (is_allocated) {
        total++;
      }
    }

    return total;
  }

private:
  size_t m_bitmap_page_count;
  size_t m_page_count;
  void *m_start_addr;
  void *m_end_addr;
};

void init_page_frame_allocator(void *start_addr, void *end_addr);
PageFrameAllocator *get_page_frame_allocator();

#endif
