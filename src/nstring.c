/**
 * @brief 
 * @author Karolina Kulak
 * @email caroline1188@wp.pl
 * @date Jul 04, 2026
 */

#include "../inc/nstring.h"

#include <assert.h>
#include <ctype.h>


struct nstring_s_t {
    uint16_t length;    /** string length without '\0' */
    uint16_t capacity;  /** maximum string length without '\0' */
    char string[];      /** nstring content */
};


#define HEADER_SIZE                     4
#define MAX_TEXT_LENGTH                 UINT16_MAX

#define nstring_size(cap)               (HEADER_SIZE + (cap) + 1)
#define nstring_terminate(nstr)         ((nstr)->string[(nstr)->length] = '\0')
#define nstring_struct_to_str(nstr)     ((nstring_t)((nstr)->string))
#define nstring_str_to_struct(str)      ((struct nstring_s_t *)((str) - HEADER_SIZE))


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

    struct nstring_s_t *nstring = malloc(nstring_size(capacity));
    if (nstring == nullptr)
    {
        log("malloc error");
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

    *string = nstring_struct_to_str(nstring);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_dtor(nstring_t string)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    free(nstring_str_to_struct(string));

    return EXIT_STATUS_SUCCESS;
}

/**
 * GETTERS
 */

exit_status_t nstring_get_length(const cnstring_t string)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    return nstring_str_to_struct(string)->length;
}

exit_status_t nstring_get_capacity(const cnstring_t string)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    return nstring_str_to_struct(string)->capacity;
}


/**
 * SIZE MANIPULATION
 */

static exit_status_t nstring_s_resize(struct nstring_s_t **nstring, const uint16_t capacity)
{
    assert(nstring != nullptr);
    assert(*nstring != nullptr);
    assert(capacity > 0);

    if ((*nstring)->capacity != capacity)
    {
        struct nstring_s_t *temp = realloc((*nstring), nstring_size(capacity));
        if (temp == nullptr)
        {
            log("realloc error");
            return EXIT_STATUS_OS_FAILURE;
        }
        *nstring = temp;
        (*nstring)->capacity = capacity;
    }

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_resize(nstring_t *string, const uint16_t capacity)
{
    if (string == nullptr || *string == nullptr || capacity == 0)
    {
        log("string cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);
    exit_status_t status = nstring_s_resize(&nstring, capacity);
    if (exit_err(status))
    {
        log("string or text cannot null");
        return status;
    }

    *string = nstring_struct_to_str(nstring);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_fit(nstring_t *string)
{
    return nstring_resize(string, nstring_get_length(*string));
}


/**
 * METHODS
 */

static void nstring_s_append(struct nstring_s_t *nstring, const char *text, const uint16_t text_len)
{
    assert(nstring != nullptr);
    assert(text != nullptr);
    assert(text_len != 0);

    memmove(&nstring->string[nstring->length], text, text_len);
    nstring->length += text_len;
    nstring_terminate(nstring);
}

exit_status_t nstring_append(nstring_t *string, const char *text)
{
    if (string == nullptr || *string == nullptr || text == nullptr)
    {
        log("string or text cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    const uint16_t text_len = strlen(text);
    if (nstring->length + text_len > nstring->capacity)
    {
        exit_status_t status = nstring_s_resize(&nstring, nstring->length + text_len);
        if (exit_err(status))
        {
            log("string or text cannot null");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    nstring_s_append(nstring, text, text_len);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}

static void nstring_s_insert(struct nstring_s_t *nstring, const char *text, const uint16_t index, const uint16_t text_len)
{
    assert(nstring != nullptr);
    assert(text != nullptr);
    assert(text_len != 0);

    memmove(&nstring->string[index + text_len], &nstring->string[index], nstring->length - index);
    memmove(&nstring->string[index], text, text_len);
    nstring->length += text_len;
    nstring_terminate(nstring);
}

exit_status_t nstring_insert(nstring_t *string, const char *text, const uint16_t index)
{
    if (string == nullptr || *string == nullptr || text == nullptr)
    {
        log("string or text cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    if (index > nstring->length + 1)
    {
        log("index out of bounds");
        return EXIT_STATUS_INDEX_OUT_OF_BOUNDS;
    }

    const uint16_t text_len = strlen(text);
    if (nstring->length + text_len > nstring->capacity)
    {
        exit_status_t status = nstring_s_resize(&nstring, nstring->length + text_len);
        if (exit_err(status))
        {
            log("string or text cannot null");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    if (index == nstring->length + 1)
        nstring_s_append(nstring, text, text_len);
    else
        nstring_s_insert(nstring, text, index, text_len);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_rtrim(nstring_t *string)
{
    if (string == nullptr || *string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    uint16_t new_len = nstring->length - 1;
    for (; new_len > 0; new_len --)
    {
        if (!isspace(nstring->string[new_len]))
            break;
    }

    nstring->length = new_len + 1;
    nstring_terminate(nstring);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}