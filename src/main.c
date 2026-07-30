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
    char *nstring = nstring_ctor("", 0);
    if (nstring == nullptr)
    {
        log("nstring ctor failed\n");
        return -1;
    }

    printf("[%d/%d]: %s", nstring_get_length(nstring), nstring_get_capacity(nstring), nstring);    

    return 0;
}