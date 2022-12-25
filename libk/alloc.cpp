#include <stddef.h>

#include <kernel/memory/PageFrameAllocator.h>
#include <libk/alloc.h>
#include <libk/assert.h>

void *kmalloc(size_t size) {
  if (size == 0) {
    return nullptr;
  }

  auto total_size = sizeof(uint8_t) + size;

  if (total_size <= PAGE_SIZE) {
    auto *page = get_page_frame_allocator()->allocate_page();

    auto *page_u8 = reinterpret_cast<uint8_t *>(page);
    page_u8[0] = 0;

    return page_u8 + 1;
  } else {
    auto *page =
        get_page_frame_allocator()->allocate_pages(total_size / PAGE_SIZE + 1);

    auto *page_u8 = reinterpret_cast<uint8_t *>(page);
    page_u8[0] = total_size / PAGE_SIZE;

    return page_u8 + 1;
  }
}

void kmfree(void *ptr) {
  if (ptr == nullptr) {
    return;
  }

  auto *page = reinterpret_cast<uint8_t *>(ptr) - 1;
  auto page_count = page[0] + 1;

  for (size_t i = 0; i < page_count; i++) {
    auto *page_to_free = reinterpret_cast<void *>(page + i * PAGE_SIZE);
    get_page_frame_allocator()->free_page(page_to_free);
  }
}

template <typename T> static T *make_forever() {
  return new (kmalloc(sizeof(T))) T();
}

template <typename T> static constexpr T min(T v1, T v2) {
  if (v1 <= v2) {
    return v1;
  } else {
    return v2;
  }
}

void *kmrealloc(void *ptr, size_t size) {
  if (ptr == nullptr) {
    return kmalloc(size);
  }

  ASSERT(size > 0);
  auto new_buf = kmalloc(size);

  auto *existing = reinterpret_cast<uint8_t *>(ptr) - 1;
  auto existing_size = (static_cast<size_t>(existing[0]) + 1) * PAGE_SIZE;

  auto copySize = min(size, existing_size - 1);
  memcpy(new_buf, ptr, copySize);
  kmfree(ptr);

  return new_buf;
}

void *operator new(size_t size) { return kmalloc(size); }
void operator delete(void *ptr) { return kmfree(ptr); }
void operator delete(void *ptr, size_t) { return kmfree(ptr); }
void *operator new[](size_t size) { return kmalloc(size); }
void operator delete[](void *ptr) { return kmfree(ptr); }
void operator delete[](void *ptr, size_t) { return kmfree(ptr); }
void *operator new(size_t, void *ptr) { return ptr; }
void *operator new[](size_t, void *ptr) { return ptr; }
