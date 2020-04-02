#include <stdbool.h>
#include <stddef.h>

#include "../drivers/ata.h"
#include "../libk/StringBuilder.h"
#include "../libk/alloc.h"
#include "tarfs.h"

static size_t octaltoint(const uint8_t *c) {
  size_t value = 0;

  while (isdigit(*c)) {
    value *= 8;
    value += (int)(*c - '0');
    c++;
  }
  return value;
}

static size_t sum(uint8_t *buf) {
  size_t total = 0;
  for (size_t i = 0; i < 512; i++) {
    total += buf[i];
  }
  return total;
}

namespace Kernel::Filesystem {

Vector<String> TarFS::listFiles() const {
  Vector<String> files;

  uint8_t *buf = static_cast<uint8_t *>(kmalloc(512));
  uint8_t sec = 1;
  size_t prev_sum = 1;

  while (true) {
    Drivers::ataDrives[0].read_sectors(sec, 1, buf);

    if (buf[156] == '0') {
      size_t filesize = octaltoint(&buf[124]);
      StringBuilder b;
      b.append('[');
      b.append(filesize);
      b.append(" bytes] ");
      b.append((char *)buf);
      files.push(b.to_string());

      sec += filesize / 512;
    }

    if (sum(buf) == 0 && prev_sum == 0)
      break;
    prev_sum = sum(buf);
    sec++;
  }

  kmfree(buf);
  return files;
}

String TarFS::readFile(const char *name) const {
  uint8_t *buf = static_cast<uint8_t *>(kmalloc(512));

  uint8_t sec = 1;
  size_t prev_sum = 1;

  while (true) {
    Drivers::ataDrives[0].read_sectors(sec, 1, buf);

    if (buf[156] == '0') {
      size_t filesize = octaltoint(&buf[124]);
      if (streq(name, (char *)buf)) {
        kmfree(buf);
        buf = static_cast<uint8_t *>(kmalloc(filesize));

        Drivers::ataDrives[0].read_sectors(sec + 1, filesize / 512 + 1, buf);
        String s((char *)buf, filesize);
        kmfree(buf);
        return s;
        break;
      }

      sec += filesize / 512;
    }

    if (sum(buf) == 0 && prev_sum == 0)
      break;
    prev_sum = sum(buf);
    sec++;
  }

  return String(NULL, 0);
}

TarFS *TarFS::s_inst;

} // namespace Kernel::Filesystem
