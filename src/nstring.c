/**
 * @brief 
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 04, 2026
 */

#include "../inc/nstring.h"

#include <assert.h>


struct nstring_t {
    uint16_t length;    /** string length without '\0' */
    uint16_t capacity;  /** maximum string length without '\0' */
    char string[];      /** nstring content */
};


#define HEADER_SIZE             4
#define MAX_TEXT_LENGTH         UINT16_MAX

#define nstring_size(cap)       (HEADER_SIZE + cap + 1)
#define nstring_terminate(nstr) (nstr->string[nstr->length] = '\0')
#define nstring_to_str(nstr)    ((char *)(nstr->string))
#define str_to_nstring(str)     ((struct nstring_t *)(str - HEADER_SIZE))


exit_status_t nstring_ctor(char **string, const char *text, uint16_t capacity)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    uint16_t text_len = 0;
    if (text != nullptr)
    {
        text_len = strlen(text);
        capacity = text_len > capacity ? text_len : capacity;
    }

    if (capacity == 0)
    {
        log("nstring capacity cannot be 0");
        *string = nullptr;
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_t *nstring = calloc(0, nstring_size(capacity));
    if (nstring == nullptr)
    {
        log("calloc error");
        *string = nullptr;
        return EXIT_STATUS_OS_FAILURE;
    }

    nstring->length = text_len;
    nstring->capacity = capacity;

    if (text != nullptr)
    {
        memcpy(nstring->string, text, text_len);
        nstring_terminate(nstring);
    }

    *string = nstring->string;

    return EXIT_STATUS_SUCCESS;
}

static int nstring__resize(struct nstring_t *nstring, const uint16_t capacity)
{
    assert(nstring != nullptr);
    assert(capacity > 0);

    if (nstring->capacity != capacity)
    {
        nstring->capacity = capacity;
        nstring = realloc(nstring->string, nstring_size(nstring->capacity));
    }

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_dtor(char *string)
{
    if (string == nullptr)
    {
        log("string is null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    free(str_to_nstring(string));

    return EXIT_STATUS_SUCCESS;
}

/**
 * GETTERS
 */

exit_status_t nstring_get_length(const char *string)
{
    if (string == nullptr)
    {
        log("string is null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    return str_to_nstring(string)->length;
}

exit_status_t nstring_get_capacity(const char *string)
{
    if (string == nullptr)
    {
        log("string is null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    return str_to_nstring(string)->capacity;
}


/**
 * METHODS
 */

static void nstring__append(struct nstring_t *nstring, const char *text, const uint16_t text_len)
{
    assert(nstring != nullptr);
    assert(text != nullptr);
    assert(text_len != 0);

    memmove(nstring->string, text, nstring->length);
    nstring->length += text_len;
    nstring_terminate(nstring);
}

exit_status_t nstring_append(char *string, const char *text)
{
    if (string == nullptr || text == nullptr)
    {
        log("string or text is null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_t *nstring = str_to_nstring(string);

    const uint16_t text_len = strlen(text);
    if (nstring->length + text_len > nstring->capacity)
    {
        nstring__resize(nstring, nstring->length + text_len);
    }

    nstring__append(nstring, text, text_len);

    return EXIT_STATUS_SUCCESS;
}

static void nstring__insert(struct nstring_t *nstring, const char *text, const uint16_t index, const uint16_t text_len)
{
    assert(nstring != nullptr);
    assert(text != nullptr);
    assert(text_len != 0);

    memmove(&nstring->string[index + text_len], &nstring->string[index], text_len);
    memmove(&nstring->string[index], text, text_len);
    nstring->length += text_len;
    nstring_terminate(nstring);
}

exit_status_t nstring_insert(char *string, const char *text, const uint16_t index)
{
    if (string == nullptr || text == nullptr)
    {
        log("string or text is null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_t *nstring = str_to_nstring(string);

    if (index > nstring->length + 1)
    {
        log("index out of bounds");
        return EXIT_STATUS_INDEX_OUT_OF_BOUNDS;
    }

    const uint16_t text_len = strlen(text);
    if (nstring->length + text_len > nstring->capacity)
    {
        nstring__resize(nstring, nstring->length + text_len);
    }

    if (index == nstring->length + 1)
        nstring__append(nstring, text, text_len);
    else
        nstring__insert(nstring, text, index, text_len);

    return EXIT_STATUS_SUCCESS;
}