#include <kernel/drivers/terminal.h>
#include <kernel/memory/PageFrameAllocator.h>
#include <libk/printf.h>

void shell_mem(char *) {
  size_t page_frame_used = get_page_frame_allocator()->allocatedPages() * 4096;
  size_t mem_usage_kb = page_frame_used / 1000;

  basic_serial_printf("[MemUsage] Physical mem usage is %d KB\n", mem_usage_kb);

  Kernel::Drivers::VGATerminal::lock();
  kprintf("Memory usage: %d KB\n", mem_usage_kb);
  Kernel::Drivers::VGATerminal::unlock();
}