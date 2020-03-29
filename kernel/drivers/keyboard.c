#include "keyboard.h"
#include "../scheduler.h"
#include "io.h"
#include "serial.h"

char scanCodes[128] = {
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
    while (!(inb(0x64) & 1))
      yield();
    char scancode = inb(0x60);
    // serial_printf("Got key code %d\n", scancode);
    if ((scancode & 128) == 128)
      continue;
    char key = scanCodes[scancode];
    if (key) {
      return key;
    }
  }
}

driverDefinition KEYBOARD_DRIVER = {.name = "PS/2 Keyboard",
                                    .isAvailable = driver_true,
                                    .initialize = driver_true};
