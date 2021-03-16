#include <kernel/drivers/BGA.h>
#include <kernel/drivers/ps2/mouse.h>
#include <kernel/scheduler.h>
#include <libk/debug.h>

using namespace Kernel;

static void drawRect(Drivers::BGA *bga, int x, int y, size_t width,
                     size_t height, uint8_t r, uint8_t g, uint8_t b) {
  auto xEnd = x + width;
  auto yEnd = y + height;
  for (size_t _x = x; _x < xEnd; _x++)
    for (size_t _y = y; _y < yEnd; _y++)
      if (_x >= 0 && _x <= 800 && _y >= 0 && _y <= 600)
        bga->setPixel(_x, _y, r, g, b);
}

[[noreturn]] void shell_vgademo(char *) {
  dbg() << "Going to VGA-land";
  Drivers::BGA *bga = Drivers::BGA::inst();
  bga->setResolution(800, 600);
  Drivers::Mouse::SetWidth(800);
  Drivers::Mouse::SetHeight(600);

  while (true) {
    int mouseX = Drivers::Mouse::GetX();
    int mouseY = Drivers::Mouse::GetY();

    drawRect(bga, 0, 0, 800, 600, 50, 50, 50);

    drawRect(bga, mouseX - 15, mouseY - 25, 30, 50, 0, 0, 0);
    drawRect(bga, mouseX - 13, mouseY - 23, 26, 46, 255, 255, 255);
    if (Drivers::Mouse::leftBtn)
      drawRect(bga, mouseX - 13, mouseY - 23, 13, 13, 0, 255, 0);
    if (Drivers::Mouse::rightBtn)
      drawRect(bga, mouseX, mouseY - 23, 13, 13, 0, 255, 0);

    bga->flip();
    yield();
  }
}