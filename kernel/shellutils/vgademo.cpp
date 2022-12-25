#include <kernel/drivers/BGA.h>
#include <kernel/drivers/ps2/keyboard.h>
#include <kernel/scheduler.h>
#include <libk/debug.h>

static void drawRect(Kernel::Drivers::BGA *bga, int x, int y, size_t width,
                     size_t height, uint8_t r, uint8_t g, uint8_t b) {
  auto xEnd = x + width;
  auto yEnd = y + height;

  for (int64_t _x = x; _x < xEnd; _x++)
    for (int64_t _y = y; _y < yEnd; _y++)
      if (_x >= 0 && _x <= 800 && _y >= 0 && _y <= 600)
        bga->setPixel(_x, _y, r, g, b);
}

void vga_demo_task() {
  auto bga = Kernel::Drivers::BGA::inst();
  bga->setResolution(800, 600);

  auto x = 0;
  auto y = 0;

  while (true) {
    auto key = Kernel::Drivers::KeyboardDriver::getRawKeycode();

    if (key.is_some()) {
      auto k = key.unwrap();
      if (k == 0x4B) {
        x -= 10;
      } else if (k == 0x4D) {
        x += 10;
      } else if (k == 0x48) {
        y -= 10;
      } else if (k == 0x50) {
        y += 10;
      }
    }

    drawRect(bga, 0, 0, 800, 600, 0, 0, 0);
    drawRect(bga, x, y, 100, 100, 100, 150, 100);

    bga->flip();
    yield();
  }
}

void shell_vgademo(char *) {
  dbg("vga-demo") << "Going to VGA-land";

  spawnTask(vga_demo_task, "vga-demo");
}