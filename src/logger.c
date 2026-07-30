/**
 * @brief 
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 30, 2026
 */

#include "logger.h"


void logger_log(const bool dbg, const char *file, const char *func, const int line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    FILE *out = errno != 0 ? stdout : stderr;

    fprintf(out, "[%s] %s\\%s():%d: ", (dbg ? "DBG" : "LOG"), file, func, line);

    if (errno != 0)
        fprintf(stderr, "system error %d: %s\n", errno, strerror(errno));

    vfprintf(out, format, args);
    fprintf(out, "\n");

    fflush(out);

    va_end(args);
}
