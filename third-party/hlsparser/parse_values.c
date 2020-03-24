/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"

/**
 * Copies the next line from \a src into the pointer \a dest
 *
 * @param src The source of the multi-line text
 * @param dest The destination pointer that will be assigned the output
 * @param size The length of src
 */
int parse_line_to_str(const char *src, char **dest, size_t size) {
  if (!src) {
    return 0;
  }

  const char *begin = src;
  const char *end = src;

  // find the end of the TAG line
  while (!(*end == '\n' || (*end == '\r' && end[1] == '\n') || *end == '\0' ||
           end == &src[size])) {
    ++end;
  }

  // attach a new string and assign it to the output value
  size_t dest_size = end - begin;
  if (dest_size > 0 && dest) {
    *dest = str_utils_ndup(begin, dest_size);
  }

  return dest_size;
}

/**
 * Parses the number represented as a string from \a src and writes it
 * into the \a dest pointer
 *
 * @param src The text source to read from
 * @param dest The destination pointer to write the output value into
 * @param size The length of \a src
 */
int parse_str_to_int(const char *src, int *dest, size_t size) {
  const char *pt = src;
  int value = 0;
  int sign = 1;

  if (!src || size == 0) {
    return 0;
  }

  // parse the minus sign
  if (*pt == '-') {
    sign = -1;
    ++pt;
  }

  // parse the number
  while (*pt >= '0' && *pt <= '9' && pt < &src[size]) {
    value = (value * 10) + (*pt - '0');
    ++pt;
  }

  // set the value
  if (dest) {
    *dest = value * sign;
  }
  // return how how forward we moved in the string
  return pt - src;
}

/**
 * Parses the floating-point number represented as a string from \a src and
 * writes it into the pointer \a dest
 *
 * @param src The text source to read
 * @param dest The destination to write the value into
 * @param size The length of the text source
 */
int parse_str_to_float(const char *src, float *dest, size_t size) {
  const char *pt = src;
  float value = 0.f;
  float fraction = 0.f;
  float sign = 1.f;

  if (!src || size == 0) {
    return 0;
  }

  // parse the minus sign
  if (*pt == '-') {
    sign = -1.f;
    ++pt;
  }

  // parse the floating point number before the decimal point
  while (*pt >= '0' && *pt <= '9' && pt < &src[size]) {
    value = (value * 10) + (*pt - '0');
    ++pt;
  }

  // parse the decimal point
  if (pt < &src[size] && *pt == '.') {
    ++pt;
    float divis = 1.f;
    while (*pt >= '0' && *pt <= '9' && pt < &src[size]) {
      fraction = (fraction * 10.f) + (float)(*pt - '0');
      divis *= 10.f;
      ++pt;
    }
    fraction = fraction / divis;
  }

  // set the value
  if (dest) {
    *dest = sign * (value + fraction);
  }
  // return how how forward we moved in the string
  return pt - src;
}

/**
 * Parses the  number represented as a string from \a src and writes it into the
 * pointer \a dest
 *
 * @param src The source of the text
 * @param dest The destination pointer where the value will be written
 * @param size The length od the source text
 */
int parse_date(const char *src, uint64_t *dest, size_t size) {
  /*
  YYYY 			(eg 1997)
  Year and month:
  YYYY-MM 			(eg 1997-07)
  Complete date:
  YYYY-MM-DD 			(eg 1997-07-16)
  Complete date plus hours and minutes:
  YYYY-MM-DDThh:mmTZD         (eg 1997-07-16T19:20+01:00)
  Complete date plus hours, minutes and seconds:
  YYYY-MM-DDThh:mm:ssTZD 	(eg 1997-07-16T19:20:30+01:00)
  Complete date plus hours, minutes, seconds and a decimal fraction of a second
  YYYY-MM-DDThh:mm:ss.sTZD 	(eg 1997-07-16T19:20:30.45+01:00)
  */

  if (!size || !src || src[0] == '\0') {
    return 0;
  }

  const char *pt = src;
  const char *end = &src[size];

  int year = 0, month = 0, day = 0, hours = 0, mins = 0, tzd = 0;
  float secs = 0.f;
  int is_utc = 0;

  // Parse the year
  int len = parse_str_to_int(pt, &year, size - (src - pt));
  pt += len;

  // parse the month
  if (len == 4) {
    if (*pt == '-' && pt < end) {
      ++pt;
      len = parse_str_to_int(pt, &month, size - (pt - src));
      pt += len;
    } else {
      // the year is the minimum requirement,
      // setup default values if we get this far
      month = 1;
      hours = 24;
    }
  } else {
    return 0;
  }

  // parse the day
  if (len == 2) {
    if (*pt == '-' && pt < end) {
      ++pt;
      len = parse_str_to_int(pt, &day, size - (pt - src));
      pt += len;
    } else {
      hours = 24;
      len = 0;
    }
  } else {
    len = 0;
  }

  // parse the hours, minutes, seconds and fraction of a second
  if (len == 2 && *pt == 'T' && pt < end) {
    ++pt;
    len = parse_str_to_int(pt, &hours, size - (pt - src));
    pt += len;
    // parse the minutes
    if (len == 2 && *pt == ':' && pt < end) {
      ++pt;
      len = parse_str_to_int(pt, &mins, size - (pt - src));
      pt += len;
      // parse the seconds
      if (len == 2) {
        if (*pt == ':' && pt < end) {
          ++pt;
          len = parse_str_to_float(pt, &secs, size - (pt - src));
          pt += len;
        }
      } else {
        len = 0;
      }
    } else {
      len = 0;
    }
  } else {
    len = 0;
  }

  // parse the TimeZone
  if (len > 0) {
    // parse the time zone sign
    if (*pt == 'Z') {
      is_utc = 1;
    } else if (*pt == '-') {
      tzd = -1;
      ++pt;
    } else if (*pt == '+') {
      tzd = 1;
      ++pt;
    }
    // parse the time zone if the sign was valid
    if (tzd != 0 && !is_utc) {
      int tzd_hrs = 0;
      int tzd_mins = 0;
      // parse the hours
      len = parse_str_to_int(pt, &tzd_hrs, end - pt);
      pt += len;
      if (len == 2 && *pt == ':' && pt < end) {
        ++pt;
        // parse the minutes
        len = parse_str_to_int(pt, &tzd_mins, end - pt);
        if (len == 2) {
          tzd *= (tzd_hrs * 60) + tzd_mins;
          pt += len;
        }
      }
    }
  }

  // join it all together...
  int is_leap_year =
      (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
  // calculate the number of leap year, leave one off it the specified
  // year is a leap year, we'll calculate for that in the swtich below
  int leap_years = (year / 4) - (year / 100) + (year / 400);
  leap_years = is_leap_year ? leap_years - 1 : leap_years;

  // work out the total number of days
  int total_days = (year * 365) + leap_years;
  switch (month) {
  case 1:
    total_days += day;
    break;
  case 2:
    total_days += 31 + day;
    break;
  case 3:
    total_days += (is_leap_year ? 60 : 59) + day;
    break;
  case 4:
    total_days += (is_leap_year ? 91 : 90) + day;
    break;
  case 5:
    total_days += (is_leap_year ? 121 : 120) + day;
    break;
  case 6:
    total_days += (is_leap_year ? 152 : 151) + day;
    break;
  case 7:
    total_days += (is_leap_year ? 182 : 181) + day;
    break;
  case 8:
    total_days += (is_leap_year ? 213 : 212) + day;
    break;
  case 9:
    total_days += (is_leap_year ? 244 : 243) + day;
    break;
  case 10:
    total_days += (is_leap_year ? 274 : 273) + day;
    break;
  case 11:
    total_days += (is_leap_year ? 305 : 304) + day;
    break;
  case 12:
    total_days += (is_leap_year ? 335 : 334) + day;
    break;
  }

  uint64_t time =
      ((((((total_days * 24ULL) + (hours)) * 60ULL) + mins) * 60ULL) *
       1000ULL) +
      (uint64_t)(secs * 1000.f);
  time -= (tzd * 60000ULL); // convert time zone minutes into ms
  time -= 62167219200000;   // Minus ms since Midnight 1/1/1970

  if (dest) {
    *dest = time;
  }

  return pt - src;
}

int parse_attrib_str(const char *src, char **dest, size_t size) {
  if (!src || !size) {
    return 0;
  }

  const char *pt = src;
  int open = 0;
  const char *start = pt;
  const char *end = pt;

  while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {

    if (*pt == '\"') {
      open = !open;

      if (open) {
        start = pt + 1;
      } else {
        end = pt;
        ++pt;
        break;
      }
    }

    ++pt;
  }

  // allocate a new string and assign it to the output values
  if (dest && end > start) {
    *dest = str_utils_ndup(start, end - start);
  }

  // return the length of parsed values
  return pt - src;
}

int parse_attrib_data(const char *src, char **dest, size_t size) {
  const char *pt = src;
  uint8_t buffer[16];
  int buf_size = 16;
  int idx = 0;

  if (!src || !size) {
    return 0;
  }

  // the data HAS to start with '0x' or '0X'
  if (*pt == '0' && (pt[1] == 'x' || pt[1] == 'X')) {
    pt += 2;
    const char *end = &src[size];
    uint8_t value = 0;

    // every 2 bytes calculate the next value
    for (int i = 0; pt < end; ++i) {
      uint8_t tmp_value = 0;
      if (*pt >= '0' && *pt <= '9') {
        tmp_value = *pt - '0';
      } else if (*pt >= 'a' && *pt <= 'f') {
        tmp_value = (*pt - 'a') + 10;
      } else if (*pt >= 'A' && *pt <= 'F') {
        tmp_value = (*pt - 'A') + 10;
      } else {
        // there is a problem, we could just be at the end
        // of the data, break out!
        break;
      }
      ++pt;

      // every 2nd character finishes the byte
      if (i % 2 == 0) {
        value = tmp_value;
      } else {
        value = (value * 16) + tmp_value;
        // add the next value to the buffer
        buffer[idx] = value;
        ++idx;
        // if we've exhausted the buffer, add it to the output
        if (idx == buf_size) {
          if (!(*dest)) {
            *dest = str_utils_ndup((char *)buffer, idx);
          } else {
            *dest = str_utils_nappend(*dest, (char *)buffer, idx);
          }
          // reset the buffer index
          idx = 0;
        }
      }
    }
  }

  // add any remaining data to the string
  if (dest && idx > 0) {
    if (!(*dest)) {
      *dest = str_utils_ndup((char *)buffer, idx);
    } else {
      *dest = str_utils_nappend(*dest, (char *)buffer, idx);
    }
  }

  // return the length of parsed values
  return pt - src;
}
