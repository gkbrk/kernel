#include <stddef.h>

#include <kernel/drivers/ata.h>
#include <kernel/fs/tarfs.h>
#include <libk/StringBuilder.h>
#include <libk/alloc.h>

static size_t octaltoint(const uint8_t *c) {
  size_t value = 0;

  while (isdigit(*c)) {
    value *= 8;
    value += (int)(*c - '0');
    c++;
  }
  return value;
}

static size_t sum(const uint8_t *buf) {
  size_t total = 0;
  for (size_t i = 0; i < 512; i++) {
    total += buf[i];
  }
  return total;
}

namespace Kernel::FS {

String TarFS::readFile(const char *name) {
  auto *buf = static_cast<uint8_t *>(kmalloc(512));

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
      }

      sec += filesize / 512;
    }

    if (sum(buf) == 0 && prev_sum == 0)
      break;
    prev_sum = sum(buf);
    sec++;
  }

  return String(nullptr, 0);
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

struct DirStream {
  String path;
  size_t sector;
};

void *TarFS::openDir(String path) {
  auto ds = new DirStream;
  ds->path = path;
  ds->sector = 1;
  return ds;
}

void TarFS::closeDir(void *ptr) {
  auto ds = (DirStream *)ptr;
  delete ds;
}

bool TarFS::readDir(void *dirStream, DirEntry *dir) {
  auto ds = (DirStream *)dirStream;
  uint8_t buf[512] = {0};
  size_t prev_sum = 1;

  while (true) {
    Drivers::ataDrives[0].read_sectors(ds->sector, 1, buf);

    auto type = buf[156];

    size_t filesize = octaltoint(&buf[124]);
    ds->sector += filesize / 512 + 2;
    dir->size = filesize;

    if (type == '0') {
      auto name = (char *)buf;
      dir->path = String(name, strlen(name));

      if (dir->path.starts_with(ds->path))
        return true;
    }

    if (sum(buf) == 0 && prev_sum == 0)
      return false;
    prev_sum = sum(buf);
  }
}

TarFS *TarFS::s_inst;

} // namespace Kernel::FS
