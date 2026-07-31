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

#define nstring_find(x)     _Generic((x),               \
                            char    : nstring_find_char \
                            char*   : nstring_find_str  \
                            )(x)

#define nstring_count(x)    _Generic((x),                   \
                            char    : nstring_count_char    \
                            char*   : nstring_count_str     \
                            )(x)

#define nstring_replace(x)  _Generic((x),                   \
                            char    : nstring_replace_char  \
                            char*   : nstring_replace_str   \
                            )(x)

#define nstring_replace_all(x)  _Generic((x),                       \
                                char    : nstring_replace_all_char  \
                                char*   : nstring_replace_all_str   \
                                )(x)


exit_status_t nstring_ctor(nstring_t *string, const char *text, uint16_t capacity);
exit_status_t nstring_dtor(nstring_t *string);

exit_status_t nstring_copy(nstring_t *string, cnstring_t other);
exit_status_t nstring_move(nstring_t *src, nstring_t *dst);

exit_status_t nstring_get_length(cnstring_t string);
exit_status_t nstring_get_capacity(cnstring_t string);

exit_status_t nstring_resize(nstring_t *string, uint16_t capacity);
exit_status_t nstring_fit(nstring_t *string);

exit_status_t nstring_append(nstring_t *string, const char *text);
exit_status_t nstring_insert(nstring_t *string, const char *text, uint16_t index);

exit_status_t nstring_rtrim(const nstring_t *string);
exit_status_t nstring_ltrim(const nstring_t *string);
exit_status_t nstring_trim(const nstring_t *string);

exit_status_t nstring_starts_with(cnstring_t string, const char *substr);
exit_status_t nstring_ends_with(cnstring_t string, const char *substr);

exit_status_t nstring_find_char(cnstring_t string, char c, uint16_t start);
exit_status_t nstring_find_str(cnstring_t string, const char *substr, uint16_t start);

exit_status_t nstring_count_char(nstring_t *string, char c);
exit_status_t nstring_count_str(nstring_t *string, const char *substr);

exit_status_t nstring_replace_char(nstring_t *string, char c, uint16_t start);
exit_status_t nstring_replace_str(nstring_t *string, const char *substr, uint16_t start);

exit_status_t nstring_replace_all_char(nstring_t *string, char c);
exit_status_t nstring_replace_all_str(nstring_t *string, const char *substr);

#endif  // INC_NSTRING_H