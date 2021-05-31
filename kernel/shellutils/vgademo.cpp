#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/BGA.h>
#include <kernel/drivers/ps2/mouse.h>
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

class VgaDemoTask : public Multitasking::Minitask {
public:
  VgaDemoTask(Drivers::BGA *bga) { m_bga = bga; }

private:
  [[nodiscard]] String name() const override { return String("vga-demo"); }
  bool step() override {
    setDeadline(0.2);
    int mouseX = Drivers::Mouse::GetX();
    int mouseY = Drivers::Mouse::GetY();

    drawRect(m_bga, 0, 0, 800, 600, 50, 50, 50);

    drawRect(m_bga, mouseX - 15, mouseY - 25, 30, 50, 0, 0, 0);

    drawRect(m_bga, mouseX - 13, mouseY - 23, 26, 46, 255, 255, 255);

    if (Drivers::Mouse::leftBtn)
      drawRect(m_bga, mouseX - 13, mouseY - 23, 13, 13, 0, 255, 0);

    if (Drivers::Mouse::rightBtn)
      drawRect(m_bga, mouseX, mouseY - 23, 13, 13, 0, 255, 0);

    m_bga->flip();

    return true;
  }
  Drivers::BGA *m_bga;
};

Multitasking::Minitask *shell_vgademo(char *) {
  dbg("vga-demo") << "Going to VGA-land";
  Drivers::BGA *bga = Drivers::BGA::inst();
  bga->setResolution(800, 600);
  Drivers::Mouse::SetWidth(800);
  Drivers::Mouse::SetHeight(600);

  return new VgaDemoTask(bga);
}