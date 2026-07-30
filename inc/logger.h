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

typedef enum : uint8_t
{
    EXIT_STATUS_SUCCESS = 0,
    EXIT_STATUS_INVALID_ARGUMENT,
    EXIT_STATUS_OS_FAILURE,
    EXIT_STATUS_INDEX_OUT_OF_BOUNDS,

    EXIT_STATUS_NUM
} exit_status_t;


void logger_err(exit_status_t status);
void logger_log(bool dbg, const char *file, const char *func, int line, const char *format, ...);


#define set_err(code)       logger_err(code);
#define clr_err()           logger_err(EXIT_STATUS_SUCCESS);
#define log(format, ...)    logger_log(false, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);

#ifdef MODE_DEBUG

#define logdbg(format, ...) logger_log(true, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);

#else

#define logdbg(format, ...)

#endif  // MODE_DEBUG

#endif  // INC_LOGGER_H