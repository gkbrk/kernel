#include <kernel/drivers/io.h>
#include <kernel/drivers/ps2/keyboard.h>
#include <kernel/scheduler.h>

namespace Kernel::Drivers {

static volatile uint8_t newScancode = 0;
static volatile bool newKey = false;

template <const size_t BUFSIZE> class KeyboardBuffer {
public:
  void Write(uint8_t key) {
    m_buf[m_head++] = key;
    m_count++;

    if (m_head == BUFSIZE)
      m_head = 0;

    if (m_count > BUFSIZE)
      m_count = BUFSIZE;
  }

  bool Available() const { return m_count > 0; }

  uint8_t Read() {
    auto byte = m_buf[(m_head - m_count) % BUFSIZE];
    m_count--;
    return byte;
  }

private:
  uint8_t m_buf[BUFSIZE];
  size_t m_head = 0;
  size_t m_count = 0;
};

static KeyboardBuffer<512> kbuf;

void KeyboardDriver::kernelKeypress(uint8_t sc) {
  newScancode = sc;
  kbuf.Write(sc);
  newKey = true;
}

Option<uint8_t> KeyboardDriver::getRawKeycode() {
  if (!newKey)
    return {};

  newKey = false;
  return Option<uint8_t>(newScancode);
}

uint8_t KeyboardDriver::spinRawKeycode() {
  auto keyCode = getRawKeycode();

  while (!keyCode.is_some()) {
    yield();
    keyCode = getRawKeycode();
  }

  return keyCode.value();
}

} // namespace Kernel::Drivers

static char scanCodes[128] = {
    0,   27,   '1',  '2', '3',  '4', '5', '6', '7', '8', '9', '0', '-',
    '=', '\b', '\t', 'q', 'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']',  '\n', 0,   'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,    0,   0,   0,   0,   '-', 0,   0,   0,
    '+', 0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0, /*all of other
                                                                    keys are
                                                                    setted as
                                                                    undefined*/
};

char keyboardSpinLoop() {
  while (true) {
    uint8_t scancode = Kernel::Drivers::KeyboardDriver::spinRawKeycode();
    if ((scancode & 128) == 128)
      continue;
    char key = scanCodes[scancode];
    if (key) {
      return key;
    }
  }
}

Option<char> keyboardTry() {
  if (!Kernel::Drivers::kbuf.Available())
    return {};
  uint8_t scancode = Kernel::Drivers::kbuf.Read();
  if ((scancode & 128) == 128)
    return {};
  char key = scanCodes[scancode];
  if (key) {
    return Option(key);
  }
}