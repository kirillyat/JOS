/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
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

void
rtc_init(void) {
  nmi_disable();
  // LAB 4: Your code here

  //configure rtc divider so that interrupts come 2 times a second
  outb(IO_RTC_CMND, NMI_LOCK | RTC_AREG);
  uint8_t areg = inb(IO_RTC_DATA);
  areg = (areg & 0xF0) | 15;
  outb(IO_RTC_CMND, NMI_LOCK | RTC_AREG);
  outb(IO_RTC_DATA, areg);

  //enable RTC periodical interrupts(IRQ 8)
  outb(IO_RTC_CMND, NMI_LOCK | RTC_BREG);
  uint8_t breg = inb(IO_RTC_DATA);
  breg |= RTC_PIE;
  outb(IO_RTC_CMND, NMI_LOCK | RTC_BREG);
  outb(IO_RTC_DATA, breg);

  nmi_enable();//not sure if I need it;
}

uint8_t
rtc_check_status(void) {
  uint8_t status = 0;
  // LAB 4: Your code here
  outb(IO_RTC_CMND, (inb(IO_RTC_CMND) & NMI_LOCK) | RTC_CREG);
  status = inb(IO_RTC_DATA);
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