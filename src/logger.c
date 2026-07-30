/**
 * @brief 
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 30, 2026
 */

#include "logger.h"

static exit_status_t exit_status = EXIT_STATUS_SUCCESS;
static const char *exit_status_to_str[EXIT_STATUS_NUM + 1] =
{
    [EXIT_STATUS_SUCCESS]              = "success",
    [EXIT_STATUS_INVALID_ARGUMENT]     = "invalid argument given",
    [EXIT_STATUS_OS_FAILURE]           = "OS function failure",
    [EXIT_STATUS_INDEX_OUT_OF_BOUNDS]  = "index out of bounds",
    [EXIT_STATUS_NUM]                  = "invalid exit status",
};

void logger_err(exit_status_t status)
{
    if (exit_status > EXIT_STATUS_NUM)
        status = EXIT_STATUS_NUM;
    exit_status = status;
}


void logger_log(const bool dbg, const char *file, const char *func, const int line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    FILE *out = errno != 0 || exit_status != EXIT_STATUS_SUCCESS ? stdout : stderr;

    fprintf(out, "[%s] %s\\%s():%d: ", (dbg ? "DBG" : "LOG"), file, func, line);

    if (exit_status != EXIT_STATUS_SUCCESS)
    {
        fprintf(stderr, "error %d: %s\n", exit_status, exit_status_to_str[exit_status]);
        clr_err();
    }

    if (errno != 0)
        fprintf(stderr, "system error %d: %s\n", errno, strerror(errno));

    vfprintf(out, format, args);
    fprintf(out, "\n");

    fflush(out);

    va_end(args);
}
