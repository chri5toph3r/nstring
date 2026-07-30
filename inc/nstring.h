/**
 * @brief 
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 04, 2026
 */

#ifndef INC_NSTRING_H
#define INC_NSTRING_H

#include <stdint.h>
#include <stdlib.h>
#include "logger.h"

exit_status_t nstring_ctor(char **string, const char *text, uint16_t capacity);
exit_status_t nstring_dtor(char *string);

exit_status_t nstring_get_length(const char *string);
exit_status_t nstring_get_capacity(const char *string);

exit_status_t nstring_append(char *string, const char *text);
exit_status_t nstring_insert(char *string, const char *text, uint16_t index);

#endif  // INC_NSTRING_H