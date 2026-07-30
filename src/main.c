/**
 * @brief Main entry point for NString project
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 19, 2026
 */

#include "nstring.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
    nstring_t nstring = nullptr;
    nstring_ctor(&nstring, "    Hello", 0);
    if (nstring == nullptr)
    {
        log("nstring ctor failed\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\"\n", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);

    exit_status_t status = nstring_append(&nstring, "World    ");
    if (exit_err(status))
    {
        log("nstring append error\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\"\n", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);

    status = nstring_insert(&nstring, " ", 9);
    if (exit_err(status))
    {
        log("nstring insert error\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\"\n", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);

    status = nstring_rtrim(&nstring);
    if (exit_err(status))
    {
        log("nstring rtrim error\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\"\n", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);

    status = nstring_fit(&nstring);
    if (exit_err(status))
    {
        log("nstring rtrim error\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\"\n", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);

    status = nstring_ltrim(&nstring);
    if (exit_err(status))
    {
        log("nstring rtrim error\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\"\n", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);

    status = nstring_find_char(nstring, 'x', 0);
    if (status == EXIT_STATUS_NO_RESULT)
    {
        printf("not found\n");
    }
    if (exit_err(status))
    {
        log("nstring find char error\n");
        return -1;
    }
    printf("[%d/%d]: \"%s\" | 'x' on index %d ('%c')\n",
        nstring_get_length(nstring), nstring_get_capacity(nstring), nstring,
        status, nstring[status]);

    nstring_dtor(&nstring);

    return 0;
}