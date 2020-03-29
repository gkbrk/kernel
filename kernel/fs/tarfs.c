#include <stdbool.h>
#include <stddef.h>

#include "../drivers/ata.h"
#include "../libk/alloc.h"
#include "../libk/log.h"
#include "../libk/messaging.h"
#include "../libk/printf.h"
#include "../libk/string.h"
#include "../scheduler.h"
#include "tarfs.h"

static size_t octaltoint(const char *c) {
  size_t value = 0;

  while (isdigit(*c)) {
    value *= 8;
    value += (int)(*c - '0');
    c++;
  }
  return value;
}

static size_t sum(char *buf) {
  size_t total = 0;
  for (size_t i = 0; i < 512; i++) {
    total += buf[i];
  }
  return total;
}

void tarfs_task() {
  while (true) {
    Message *m = message_get(&runningTask->port);
    klog(m->message);

    char *cmd = strsep(&m->message, " ");

    if (streq(cmd, "ls")) {
      char *buf = kmalloc(512);
      uint8_t sec = 1;
      size_t prev_sum = 1;

      while (true) {
        ata_read_sectors(&ata_drives[0], sec, 1, buf);

        if (buf[156] == '0') {
          size_t filesize = octaltoint(&buf[124]);
          kprintf("[%d bytes] %s\n", filesize, buf);

          sec += filesize / 512;
        }

        if (sum(buf) == 0 && prev_sum == 0)
          break;
        prev_sum = sum(buf);
        sec++;
      }

      kmfree(buf);
    } else if (streq(cmd, "cat")) {
      char *name = strsep(&m->message, " ");

      char *buf = kmalloc(512);
      uint8_t sec = 1;
      size_t prev_sum = 1;
      while (true) {
        ata_read_sectors(&ata_drives[0], sec, 1, buf);

        if (buf[156] == '0') {
          size_t filesize = octaltoint(&buf[124]);
          if (streq(name, buf)) {
            kmfree(buf);
            buf = kmalloc(filesize);
            ata_read_sectors(&ata_drives[0], sec + 1, filesize / 512 + 1, buf);
            m->response = buf;
            break;
          }

          sec += filesize / 512;
        }

        if (sum(buf) == 0 && prev_sum == 0)
          break;
        prev_sum = sum(buf);
        sec++;
      }
    }

    if (m->response == NULL)
      m->response = "";
    yield();
  }
}
