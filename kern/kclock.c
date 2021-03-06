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

  uint8_t s, m, h, d, M, y, Y, state;
  s = mc146818_read(RTC_SEC);
  m = mc146818_read(RTC_MIN);
  h = mc146818_read(RTC_HOUR);
  d = mc146818_read(RTC_DAY);
  M = mc146818_read(RTC_MON);
  y = mc146818_read(RTC_YEAR);
  Y = mc146818_read(RTC_YEAR_HIGH);

  state = mc146818_read(RTC_BREG);
  if (state & RTC_12H) {
    /* Fixup 12 hour mode */
    h = (h & 0x7F) + 12 * !!(h & 0x80);
  }

  if (!(state & RTC_BINARY)) {
    /* Fixup binary mode */
    s = BCD2BIN(s);
    m = BCD2BIN(m);
    h = BCD2BIN(h);
    d = BCD2BIN(d);
    M = BCD2BIN(M);
    y = BCD2BIN(y);
    Y = BCD2BIN(Y);
  }

  time.tm_sec = s;
  time.tm_min = m;
  time.tm_hour = h;
  time.tm_mday = d;
  time.tm_mon = M - 1;
  time.tm_year = y + Y * 100 - 1900;

  return timestamp(&time);
}

int
gettime(void) {
  nmi_disable();
  // LAB 12: your code here
  int t;
  while (mc146818_read(RTC_AREG) & RTC_UPDATE_IN_PROGRESS);
  if ((t = get_time()) != get_time()) {
    t = get_time();
  }
  nmi_enable();
  return t;
}

void
rtc_init(void) {
  nmi_disable();
  // LAB 4: Your code here

  //configure rtc divider so that interrupts come 2 times a second
  outb(IO_RTC_CMND, NMI_LOCK | RTC_AREG); //говорим CMOS, что выбираем регистр А
  uint8_t areg = inb(IO_RTC_DATA);        //чтение из пsys_gettimeорта 71 содержимого в регистре А
  //areg = (areg & 0xF0) | 15;              //побитовое или с 15, то есть прерывание каждые 500 мс, старшие биты оставляем
  outb(IO_RTC_CMND, NMI_LOCK | RTC_AREG); //говорим CMOS, что выбираем регистр А
  outb(IO_RTC_DATA, areg);                //вывод в порт 71 содержимого регистра А

  //enable RTC periodical interrupts(IRQ 8)
  outb(IO_RTC_CMND, NMI_LOCK | RTC_BREG); //говорим CMOS, что выбираем регистр B
  uint8_t breg = inb(IO_RTC_DATA);        //чтение из порта 71 содержимого в регистре B
  breg |= RTC_PIE;                        //установка бита RTC_PIE в регистре B, разрешение периодических прерываний
  outb(IO_RTC_CMND, NMI_LOCK | RTC_BREG); //говорим CMOS, что выбираем регистр B
  outb(IO_RTC_DATA, breg);                //вывод в порт 71 содержимого регистра B 

  nmi_enable();
}

uint8_t
rtc_check_status(void) {
  uint8_t status = 0;
  // LAB 4: Your code here
  // прочитать значение регистра часов C
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