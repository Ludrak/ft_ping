#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <sys/time.h>
#include <stdint.h>

time_t  get_difference_time(const struct timeval tv1, const struct timeval tv2);

#endif