#include <stdarg.h>
#include <stddef.h>

#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/io.h>
#include <kernel/drivers/serial.h>
#include <libk/printf.h>
#include <libk/spinlock.h>

static Spinlock lock;

static int serial_sent() { return inb(COM1 + 5) & 0x20; }

template <const size_t BUFSIZE>
class SerialWriteTask : public Kernel::Multitasking::Minitask {
public:
  void Write(uint8_t byte) {
    unblock();
    m_buf[m_head++] = byte;
    m_count++;

    if (m_head == BUFSIZE)
      m_head = 0;

    if (m_count > BUFSIZE)
      m_count = BUFSIZE;
  }

private:
  String name() const override { return String("serial-writer"); }

  bool step() override {
    if (m_count == 0) {
      block();
      return true;
    }

    if (serial_sent() == 0) {
      setDeadline(0.05);
      return true;
    }

    auto byte = m_buf[(m_head - m_count) % BUFSIZE];
    m_count--;
    outb(COM1, byte);

    setDeadline(0.1);
    return true;
  }
  uint8_t m_buf[BUFSIZE] = {0};
  size_t m_head = 0;
  size_t m_count = 0;
};

static SerialWriteTask<4096> *writeTask;

namespace Kernel::Drivers {

static Serial s_inst;

Serial::Serial() = default;

Serial *Serial::inst() { return &s_inst; }

bool Serial::isAvailable() { return true; }

bool Serial::initialize() {
  outb(COM1 + 1, 0x00); // Disable all interrupts
  outb(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
  outb(COM1 + 1, 0x00); //                  (hi byte)
  outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set

  Kernel::Multitasking::TaskRunner::SpawnTask(writeTask);
  return true;
}

} // namespace Kernel::Drivers

void serial_lock() { lock.lock(); }

void serial_unlock() { lock.unlock(); }

void serial_force_unlock() { lock.force_unlock(); }

static bool serial_received() { return inb(COM1 + 5) & 1; }

char serial_read() {
  while (serial_received() == 0)
    yield();

  return inb(COM1);
}

void serial_write_char(char a) {
  if (writeTask == nullptr) {
    writeTask = new SerialWriteTask<4096>();
  }
  writeTask->Write(a);
}

void serial_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    serial_write_char(data[i]);
  }
}

void serial_writestring(const char *data) { serial_write(data, strlen(data)); }

void serial_printf(const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  vsprintf(nullptr, serial_write_char, s, ap);
  va_end(ap);
}
