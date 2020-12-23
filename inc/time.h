#include <inc/stdio.h>
#include <inc/assert.h>

#ifndef JOS_INC_TIME_H
#define JOS_INC_TIME_H

struct tm {
  int tm_sec;  /* Seconds.     [0-60] */
  int tm_min;  /* Minutes.     [0-59] */
  int tm_hour; /* Hours.       [0-23] */
  int tm_mday; /* Day.         [1-31] */
  int tm_mon;  /* Month.       [0-11] */
  int tm_year; /* Year - 1900.  */
};

bool
is_leap_year(int year) {
  return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

int
d_to_s(int d) {
  return d * 24 * 60 * 60;
}

int
timestamp(struct tm *time) {
  int result = 0, year, month;
  for (year = 1970; year < time->tm_year + 2000; year++) {
    result += d_to_s(365 + is_leap_year(year));
  }
  int months[] = {31, 28 + is_leap_year(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for (month = 0; month < time->tm_mon; month++) {
    result += d_to_s(months[month]);
  }
  result += d_to_s(time->tm_mday) + time->tm_hour * 60 * 60 + time->tm_min * 60 + time->tm_sec;
  return result;
}

void
mktime(int time, struct tm *tm) {
  int year   = 70;
  int month  = 0;
  int day    = 0;
  int hour   = 0;
  int minute = 0;

  while (time > d_to_s(365 + is_leap_year(1900 + year))) {
    time -= d_to_s(365 + is_leap_year(1900 + year));
    year++;
  }
  tm->tm_year = year;

  int months[] = {31, 28 + is_leap_year(1900 + year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  while (time > d_to_s(months[month])) {
    time -= d_to_s(months[month]);
    month++;
  }
  tm->tm_mon = month;

  while (time > d_to_s(1)) {
    time -= d_to_s(1);
    day++;
  }
  tm->tm_mday = day;

  while (time >= 60 * 60) {
    time -= 60 * 60;
    hour++;
  }

  tm->tm_hour = hour;

  while (time >= 60) {
    time -= 60;
    minute++;
  }

  tm->tm_min = minute;
  tm->tm_sec = time;
}

void
print_datetime(struct tm *tm) {
  cprintf("%04d-%02d-%02d %02d:%02d:%02d\n",
          tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
          tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void
snprint_datetime(char *buf, int size, struct tm *tm) {
  assert(size >= 10 + 1 + 8 + 1);
  snprintf(buf, size,
           "%04d-%02d-%02d %02d:%02d:%02d",
           tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec);
}

#endif
