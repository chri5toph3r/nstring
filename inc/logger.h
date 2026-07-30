/**
 * @brief 
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 30, 2026
 */

#ifndef INC_LOGGER_H
#define INC_LOGGER_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#define MODE_DEBUG

typedef enum : int32_t
{
    EXIT_STATUS_INDEX_OUT_OF_BOUNDS = -3,
    EXIT_STATUS_OS_FAILURE = -2,
    EXIT_STATUS_INVALID_ARGUMENT = -1,
    EXIT_STATUS_SUCCESS = 0,
} exit_status_t;


void logger_log(bool dbg, const char *file, const char *func, int line, const char *format, ...);

#define exit_err(status)    (status < 0)
#define log(format, ...)    logger_log(false, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);

#ifdef MODE_DEBUG

#define logdbg(format, ...) logger_log(true, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);

#else

#define logdbg(format, ...)

#endif  // MODE_DEBUG

#endif  // INC_LOGGER_H