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

typedef char * nstring_t;
typedef const char * cnstring_t;

exit_status_t nstring_ctor(nstring_t *string, const char *text, uint16_t capacity);
exit_status_t nstring_dtor(nstring_t *string);

exit_status_t nstring_get_length(cnstring_t string);
exit_status_t nstring_get_capacity(cnstring_t string);

exit_status_t nstring_resize(nstring_t *string, uint16_t capacity);
exit_status_t nstring_fit(nstring_t *string);

exit_status_t nstring_append(nstring_t *string, const char *text);
exit_status_t nstring_insert(nstring_t *string, const char *text, uint16_t index);

exit_status_t nstring_rtrim(const nstring_t *string);
exit_status_t nstring_ltrim(const nstring_t *string);
exit_status_t nstring_trim(const nstring_t *string);

exit_status_t nstring_find_char(cnstring_t string, char c, uint16_t start);

#endif  // INC_NSTRING_H