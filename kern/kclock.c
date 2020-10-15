/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>

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
