#include <stdbool.h>
#include <stddef.h>

#include "../drivers/ata.h"
#include <libk/StringBuilder.h>
#include <libk/alloc.h>
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

namespace Kernel::FS {

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

// FS API

bool TarFS::open(String, String) { return true; }

template <typename T> static constexpr T max(T v1, T v2) {
  if (v1 >= v2) {
    return v1;
  } else {
    return v2;
  }
}

bool TarFS::readDir(String path, void (*function)(DirEntry)) {
  uint8_t *buf = (uint8_t *)(kmalloc(512));
  uint8_t sec = 1;
  size_t prev_sum = 1;

  while (true) {
    Drivers::ataDrives[0].read_sectors(sec, 1, buf);

    DirEntry dir;

    auto type = buf[156];

    size_t filesize = octaltoint(&buf[124]);
    dir.size = filesize;

    if (type == '0') {
      auto name = (char *)buf;
      dir.path = String(name, strlen(name));

      if (dir.path.starts_with(path))
        function(dir);
    }
    sec += filesize / 512 + 2;

    if (sum(buf) == 0 && prev_sum == 0)
      break;
    prev_sum = sum(buf);
  }

  kmfree(buf);
  return true;
}

TarFS *TarFS::s_inst;

} // namespace Kernel::FS
