/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/time.h>
#include <kern/kclock.h>
#include <kern/timer.h>
#include <kern/trap.h>
#include <kern/picirq.h>

static void
rtc_timer_init(void) {
  pic_init();
  rtc_init();
}

static void
rtc_timer_pic_interrupt(void) {
  irq_setmask_8259A(irq_mask_8259A & ~(1 << IRQ_CLOCK));
}

static void
rtc_timer_pic_handle(void) {
  rtc_check_status();
  pic_send_eoi(IRQ_CLOCK);
}

struct Timer timer_rtc = {
    .timer_name        = "rtc",
    .timer_init        = rtc_timer_init,
    .enable_interrupts = rtc_timer_pic_interrupt,
    .handle_interrupts = rtc_timer_pic_handle,
};

static int
get_time(void) {
  struct tm time;

  time.tm_sec  = BCD2BIN(mc146818_read(RTC_SEC));
  time.tm_min  = BCD2BIN(mc146818_read(RTC_MIN));
  time.tm_hour = BCD2BIN(mc146818_read(RTC_HOUR));
  time.tm_mday = BCD2BIN(mc146818_read(RTC_DAY));
  time.tm_mon  = BCD2BIN(mc146818_read(RTC_MON));
  time.tm_year = BCD2BIN(mc146818_read(RTC_YEAR));

  return timestamp(&time);
}

int
gettime(void) {
  nmi_disable();
  // LAB 12: your code here
  nmi_enable();
  return 0;
}

void
rtc_init(void) {
  nmi_disable();
  // LAB 4: Your code here
}

uint8_t
rtc_check_status(void) {
  uint8_t status = 0;
  // LAB 4: Your code here

  return status;
}

unsigned
mc146818_read(unsigned reg) {
  outb(IO_RTC_CMND, reg);
  return inb(IO_RTC_DATA);
}

void
mc146818_write(unsigned reg, unsigned datum) {
  outb(IO_RTC_CMND, reg);
  outb(IO_RTC_DATA, datum);
}

unsigned
mc146818_read16(unsigned reg) {
  return mc146818_read(reg) | (mc146818_read(reg + 1) << 8);
}