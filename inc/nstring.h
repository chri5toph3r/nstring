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

char *nstring_ctor(const char *text, uint16_t capacity);
int nstring_dtor(char *string);

int32_t nstring_get_length(const char *string);
int32_t nstring_get_capacity(const char *string);

int nstring_append(char *string, const char *text);
int nstring_insert(char *string, const char *text, uint16_t index);

#endif  // INC_NSTRING_H