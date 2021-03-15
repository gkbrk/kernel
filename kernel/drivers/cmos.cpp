#include <stdint.h>

#include <kernel/Random.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/io.h>
#include <kernel/scheduler.h>
#include <libk/StringBuilder.h>
#include <libk/alloc.h>

namespace Kernel::Drivers {

static const size_t CURRENT_YEAR = 2020;

static unsigned char cmos_second;
static unsigned char cmos_minute;
static unsigned char cmos_hour;
static unsigned char cmos_day;
static unsigned char cmos_month;
static unsigned int cmos_year;
static int century_register = 0x00;

static unsigned char get_RTC_register(int reg) {
  outb(0x70, reg);
  return inb(0x71);
}

uint64_t CMOS::rdtsc() {
  uint64_t ret;
  asm volatile("rdtsc" : "=A"(ret));
  return ret;
}

enum { cmos_address = 0x70, cmos_data = 0x71 };

static int get_update_in_progress_flag() {
  outb(cmos_address, 0x0A);
  return (inb(cmos_data) & 0x80);
}

static CMOS s_inst;

CMOS::CMOS() = default;

CMOS *CMOS::inst() { return &s_inst; }

bool CMOS::isAvailable() { return true; }

bool CMOS::initialize() {
  updateTime();
  return true;
}

void CMOS::updateTime() {
  unsigned char century;
  unsigned char last_second;
  unsigned char last_minute;
  unsigned char last_hour;
  unsigned char last_day;
  unsigned char last_month;
  unsigned char last_year;
  unsigned char last_century;
  unsigned char registerB;

  // Note: This uses the "read registers until you get the same values twice in
  // a row" technique
  //       to avoid getting dodgy/inconsistent values due to RTC updates

  while (get_update_in_progress_flag())
    yield(); // Make sure an update isn't in progress
  cmos_second = get_RTC_register(0x00);
  cmos_minute = get_RTC_register(0x02);
  cmos_hour = get_RTC_register(0x04);
  cmos_day = get_RTC_register(0x07);
  cmos_month = get_RTC_register(0x08);
  cmos_year = get_RTC_register(0x09);
  if (century_register != 0) {
    century = get_RTC_register(century_register);
  }

  do {
    last_second = cmos_second;
    last_minute = cmos_minute;
    last_hour = cmos_hour;
    last_day = cmos_day;
    last_month = cmos_month;
    last_year = cmos_year;
    last_century = century;

    while (get_update_in_progress_flag())
      ; // Make sure an update isn't in progress
    cmos_second = get_RTC_register(0x00);
    cmos_minute = get_RTC_register(0x02);
    cmos_hour = get_RTC_register(0x04);
    cmos_day = get_RTC_register(0x07);
    cmos_month = get_RTC_register(0x08);
    cmos_year = get_RTC_register(0x09);
    if (century_register != 0) {
      century = get_RTC_register(century_register);
    }
    yield();
  } while ((last_second != cmos_second) || (last_minute != cmos_minute) ||
           (last_hour != cmos_hour) || (last_day != cmos_day) ||
           (last_month != cmos_month) || (last_year != cmos_year) ||
           (last_century != century));

  registerB = get_RTC_register(0x0B);

  // Convert BCD to binary values if necessary

  if (!(registerB & 0x04)) {
    cmos_second = (cmos_second & 0x0F) + ((cmos_second / 16) * 10);
    cmos_minute = (cmos_minute & 0x0F) + ((cmos_minute / 16) * 10);
    cmos_hour = ((cmos_hour & 0x0F) + (((cmos_hour & 0x70) / 16) * 10)) |
                (cmos_hour & 0x80);
    cmos_day = (cmos_day & 0x0F) + ((cmos_day / 16) * 10);
    cmos_month = (cmos_month & 0x0F) + ((cmos_month / 16) * 10);
    cmos_year = (cmos_year & 0x0F) + ((cmos_year / 16) * 10);
    if (century_register != 0) {
      century = (century & 0x0F) + ((century / 16) * 10);
    }
  }

  Kernel::Random::feed_data(rdtsc());
  Kernel::Random::feed_data(cmos_second);

  // Convert 12 hour clock to 24 hour clock if necessary

  if (!(registerB & 0x02) && (cmos_hour & 0x80)) {
    cmos_hour = ((cmos_hour & 0x7F) + 12) % 24;
  }

  // Calculate the full (4-digit) year

  if (century_register != 0) {
    cmos_year += century * 100;
  } else {
    cmos_year += (CURRENT_YEAR / 100) * 100;
    if (cmos_year < CURRENT_YEAR)
      cmos_year += 100;
  }
}

String CMOS::formattedString() {
  StringBuilder b;
  b.append((size_t)cmos_hour);
  b.append(":");
  b.append((size_t)cmos_minute);
  b.append(":");
  b.append((size_t)cmos_second);
  return b.to_string();
}

} // namespace Kernel::Drivers
