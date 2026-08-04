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


#define NSTRING_HEADER_SIZE             4
#define NSTRING_MAX_TEXT_LENGTH         UINT16_MAX

#define nstring_size(cap)               (NSTRING_HEADER_SIZE + (cap) + 1)
#define nstring_terminate(nstr)         ((nstr)->string[(nstr)->length] = '\0')
#define nstring_struct_to_str(nstr)     ((nstring_t)((nstr)->string))
#define nstring_str_to_struct(str)      ((struct nstring_s_t *)((str) - NSTRING_HEADER_SIZE))


exit_status_t nstring_ctor(nstring_t * restrict string, const char * restrict text, uint16_t capacity)
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

exit_status_t nstring_dtor(nstring_t *string)
{
    if (string == nullptr || *string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_SUCCESS;
    }

    free(nstring_str_to_struct(*string));

    *string = nullptr;

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_copy(nstring_t *string, const cnstring_t other)
{
    if (string == nullptr || *string == nullptr || other == nullptr)
    {
        log("string and other cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    const struct nstring_s_t *another = nstring_str_to_struct(other);

    exit_status_t status = nstring_ctor(string, another->string, another->capacity);
    if (exit_err(status))
    {
        log("nstring ctor error");
        return status;
    }

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_move(nstring_t *src, nstring_t *dst)
{
    if (src == nullptr || *src == nullptr || dst == nullptr)
    {
        log("src nor dst cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    *dst = *src;
    *src = nullptr;

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
        struct nstring_s_t *temp = realloc(*nstring, nstring_size(capacity));
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
        log("string nor text cannot null");
        return status;
    }

    *string = nstring_struct_to_str(nstring);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_fit(nstring_t *string)
{
    return nstring_resize(string, nstring_get_length(*string));
}


exit_status_t nstring_clear(const nstring_t *string)
{
    if (string == nullptr || *string == nullptr)
    {
        log("string cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    nstring->length = 0;
    nstring_terminate(nstring);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_set(nstring_t * restrict string, const char * restrict text)
{
    if (string == nullptr || *string == nullptr || text == nullptr)
    {
        log("string nor text cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    const size_t text_len = strlen(text);
    if (text_len > nstring->capacity)
    {
        exit_status_t status = nstring_s_resize(&nstring, text_len);
        if (exit_err(status))
        {
            log("nstring struct resize error");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    memcpy(nstring->string, text, text_len);
    nstring->length = text_len;
    nstring_terminate(nstring);

    return EXIT_STATUS_SUCCESS;
}

/**
 * INSERTING
 */

static void nstring_s_append_char(struct nstring_s_t *nstring, const char c)
{
    assert(nstring != nullptr);
    assert(c != '\0');

    nstring->string[nstring->length] = c;
    nstring->length ++;
    nstring_terminate(nstring);
}

static void nstring_s_append(struct nstring_s_t *nstring, const char *text, const uint16_t text_len)
{
    assert(nstring != nullptr);
    assert(text != nullptr);
    assert(text_len != 0);

    memmove(&nstring->string[nstring->length], text, text_len);
    nstring->length += text_len;
    nstring_terminate(nstring);
}

exit_status_t nstring_append_char(nstring_t *string, const char c)
{
    if (string == nullptr || *string == nullptr || c == '\0')
    {
        log("string cannot null and c cannot be a null-terminator");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    if (nstring->length + 1 > nstring->capacity)
    {
        exit_status_t status = nstring_s_resize(&nstring, nstring->length + 1);
        if (exit_err(status))
        {
            log("nstring resize error");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    nstring_s_append_char(nstring, c);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_append(nstring_t *string, const char *text)
{
    if (string == nullptr || *string == nullptr || text == nullptr)
    {
        log("string nor text cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    const uint16_t text_len = strlen(text);
    if (nstring->length + text_len > nstring->capacity)
    {
        exit_status_t status = nstring_s_resize(&nstring, nstring->length + text_len);
        if (exit_err(status))
        {
            log("nstring resize error");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    if (text_len == 1)
        nstring_s_append_char(nstring, text[0]);
    else
        nstring_s_append(nstring, text, text_len);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}

static void nstring_s_insert_str(struct nstring_s_t *nstring, const char *text, const uint16_t index, const uint16_t text_len)
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
        log("string nor text cannot null");
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
            log("nstring resize error");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    if (index == nstring->length + 1)
        nstring_s_append(nstring, text, text_len);
    else
        nstring_s_insert_str(nstring, text, index, text_len);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_rtrim(const nstring_t *string)
{
    if (string == nullptr || *string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    uint16_t new_len = nstring->length - 1;
    for (; new_len > 0 && isspace(nstring->string[new_len]); new_len --)
    { }

    nstring->length = new_len + 1;
    nstring_terminate(nstring);

    logdbg("%s", nstring->string);

    return EXIT_STATUS_SUCCESS;
}

/**
 * WHITESPACE TRIMMING
 */

exit_status_t nstring_ltrim(const nstring_t *string)
{
    if (string == nullptr || *string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    uint16_t ltrim_len = 0;
    for (; ltrim_len < nstring->length && isspace(nstring->string[ltrim_len]); ltrim_len ++)
    { }

    nstring->length = nstring->length - ltrim_len;
    memmove(nstring->string, nstring->string + ltrim_len, nstring->length + 1);

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_trim(const nstring_t *string)
{
    if (string == nullptr || *string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    exit_status_t status = nstring_rtrim(string);
    if (exit_err(status))
    {
        log("nstring rtrim error");
        return status;
    }

    status = nstring_ltrim(string);
    if (exit_err(status))
    {
        log("nstring ltrim error");
        return status;
    }

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_starts_with(const cnstring_t string, const char *substr)
{
    if (string == nullptr || substr == nullptr)
    {
        log("string nor substr cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    const struct nstring_s_t *nstring = nstring_str_to_struct(string);

    const size_t substr_len = strlen(substr);
    if (substr_len > nstring->length)
    {
        log("substr length out of bounds");
        return EXIT_STATUS_INDEX_OUT_OF_BOUNDS;
    }

    for (uint16_t idx = 0; idx < (uint16_t)substr_len; idx ++)
    {
        if (nstring->string[idx] != substr[idx])
            return false;
    }

    return true;
}

exit_status_t nstring_ends_with(const cnstring_t string, const char *substr)
{
    if (string == nullptr || substr == nullptr)
    {
        log("string nor substr cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    const struct nstring_s_t *nstring = nstring_str_to_struct(string);

    const size_t substr_len = strlen(substr);
    if (substr_len > nstring->length)
    {
        log("substr length out of bounds");
        return EXIT_STATUS_INDEX_OUT_OF_BOUNDS;
    }

    for (uint16_t idx = nstring->length, sub_idx = (uint16_t)substr_len;
        sub_idx > 0; idx ++, sub_idx ++)
    {
        if (nstring->string[idx] != substr[idx])
            return false;
    }

    return true;
}

exit_status_t nstring_find_char(const cnstring_t string, const char c, const uint16_t start)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    if (c == '\0')
    {
        log("character cannot be a null-terminator");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(string);

    if (start > nstring->length)
    {
        log("start index out of bounds");
        return EXIT_STATUS_INDEX_OUT_OF_BOUNDS;
    }

    for (uint16_t idx = start; idx < nstring->length; idx ++)
    {
        if (nstring->string[idx] == c)
            return idx;
    }

    return EXIT_STATUS_NOT_FOUND;
}

exit_status_t nstring_count_char(nstring_t *string, const char c)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    if (c == '\0')
    {
        log("character cannot be a null-terminator");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(string);

    uint16_t count = 0;
    for (uint16_t idx = 0; idx < nstring->length; idx ++)
    {
        if (nstring->string[idx] == c)
            count ++;
    }

    return count;
}

exit_status_t nstring_replace_char(nstring_t *string, const char c, const uint16_t start)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    if (c == '\0')
    {
        log("character cannot be a null-terminator");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(string);

    if (start > nstring->length)
    {
        log("start index out of bounds");
        return EXIT_STATUS_INDEX_OUT_OF_BOUNDS;
    }

    for (uint16_t idx = start; idx < nstring->length; idx ++)
    {
        if (nstring->string[idx] == c)
        {
            nstring->string[idx] = c;
            return EXIT_STATUS_SUCCESS;
        }
    }

    return EXIT_STATUS_NOT_FOUND;
}

exit_status_t nstring_replace_all_char(nstring_t *string, const char c)
{
    if (string == nullptr)
    {
        log("string cannot be null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    if (c == '\0')
    {
        log("character cannot be a null-terminator");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(string);

    for (uint16_t idx = 0; idx < nstring->length; idx ++)
    {
        if (nstring->string[idx] == c)
            nstring->string[idx] = c;
    }

    return EXIT_STATUS_SUCCESS;
}


/**
 * FORMATTING
 */

typedef enum { NONE, COMMA, UNDERSCORE} nstring_fmt_grp;

struct nstring_fmt_ctx
{
    struct nstring_s_t **nstring;
    const char **format;
    va_list *args;
    enum : uint8_t { LEFT, CENTER, RIGHT /*, EQ */ } align_dir;  // '<' '^' '>' /* '=' */
    enum : uint8_t { MINUS, PLUS, SPACE } sign;  // '-' '+' ' '
    char align_char;    // <any char>
    uint16_t width;     // digit+
    nstring_fmt_grp width_grp;
    uint16_t prec;      // digit+
    nstring_fmt_grp prec_grp;
    bool prec_fit;      // no prec specified
    bool alt_form;      // '#'
};

exit_status_t nstring_s_fmt_f_spec_align(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    // "... {:b<c ..."
    //        ^
    context->align_char = **context->format;

    // "... {:b<c ..."
    //        -^
    (*context->format) ++;

    switch (**context->format)
    {
    case '<':
        context->align_dir = LEFT;
        break;
    case '^':
        context->align_dir = CENTER;
        break;
    case '=':  // TODO: to be supported later
        // context->align_dir = EQ;
        // break;
    case '>':
        context->align_dir = RIGHT;
        break;
    default:
        log("invalid alignment direction symbol");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    // "... {:b<c ..."
    //         -^
    (*context->format) ++;

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_s_fmt_f_spec_sign(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    switch (**context->format)
    {
    case '-':
        context->sign = MINUS;
        break;
    case '+':
        context->sign = PLUS;
        break;
    case ' ':
        context->sign = SPACE;
        break;
    default:
        log("invalid sign symbol");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    // "... {:a ..."
    //        ^
    // "... {:b<c ..."
    //          ^
    (*context->format) ++;

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_s_fmt_f_spec_width(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    size_t width = 0;
    for(; **context->format != '\0'; (*context->format) ++)
    {
        if (isdigit(**context->format))
            width = width * 10 + (**context->format - '0');
        else
        {
            if (**context->format == '_')
            {
                context->width_grp = UNDERSCORE;
                (*context->format) ++;
            }
            else if (**context->format == ',')
            {
                context->width_grp = COMMA;
                (*context->format) ++;
            }
            else
                context->width_grp = NONE;

            break;
        }
    }

    if (width > NSTRING_MAX_TEXT_LENGTH)
    {
        log("width bigger than allowed text length");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    context->width = (uint16_t)width;

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_s_fmt_f_spec_prec(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    context->prec_fit = !isdigit(**context->format);
    if (!context->prec_fit)
    {
        size_t prec = (**context->format - '0');
        for((*context->format) ++; isdigit(**context->format); (*context->format) ++)
        {
            prec = prec * 10 + (**context->format - '0');
        }

        if (prec > NSTRING_MAX_TEXT_LENGTH)
        {
            log("width bigger than allowed text length");
            return EXIT_STATUS_INVALID_ARGUMENT;
        }

        context->prec = (uint16_t)prec;
    }

    if (**context->format == '_')
    {
        context->width_grp = UNDERSCORE;
        (*context->format) ++;
    }
    else if (**context->format == ',')
    {
        context->width_grp = COMMA;
        (*context->format) ++;
    }
    else
        context->width_grp = NONE;

    return EXIT_STATUS_SUCCESS;
}



exit_status_t nstring_s_fmt_f_spec_type_numeric(struct nstring_fmt_ctx *context, bool is_unsigned)
{
    assert(context != nullptr);

    if (is_unsigned)
    {
        uint64_t number = (*(*context->format + 1) == 'l') ?
                va_arg(*context->args, uint64_t)
                : va_arg(*context->args, uint32_t);
    }
    else
    {
        int64_t number = (*(*context->format + 1) == 'l') ?
                va_arg(*context->args, int64_t)
                : va_arg(*context->args, int32_t);
    }

}

exit_status_t nstring_s_fmt_f_spec_type(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    bool is_unsigned = false;
    if (**context->format == 'u')
    {
        is_unsigned = true;
        (*context->format) ++;
    }

    switch (**context->format)
    {
    case 'b':  // [u]b[l]
    case 'c':  // char
        char c = va_arg(*context->args, int);
        break;
    case 'd':  // [u]d[l]
        break;
    case 'e':  // e
    case 'E':  // E
    case 'f':  // f
    case 'F':  // F
        double f = va_arg(*context->args, double);
        break;
    case 'g':  // TODO:
    case 'G':  // TODO:
    case 'n':  // TODO:
    case 'o':  // [u]o[l]
    case 'p':  // void *
        void *p = va_arg(*context->args, void *);
        break;
    case 's':  // const char *
        const char *s = va_arg(*context->args, char *);
        break;
    case 'x':  // [u]x[l]
    case 'X':  // [u]X[l]
    case '%':  // %
        break;
    default:
        log("invalid sign symbol");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    (*context->format) ++;

    return EXIT_STATUS_SUCCESS;
}

exit_status_t nstring_s_fmt_f_spec(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    exit_status_t status = EXIT_STATUS_INVALID_ARGUMENT;
    // "... {:b< ..."
    //        ^?
    switch (*(*context->format + 1))
    {
    case '<':
    case '^':
    case '>':
    case '=':  // TODO: to be supported later
        status = nstring_s_fmt_f_spec_align(context);
        if (exit_err(status))
        {
            log("nstring format alignment error")
            return status;
        }
    default:
        break;
    }

    switch (**context->format)
    {
    case '+':
    case '-':
    case ' ':
        status = nstring_s_fmt_f_spec_sign(context);
        if (exit_err(status))
        {
            log("nstring format sign error")
            return status;
        }
        break;
    default:
        break;
    }

    if (**context->format == 'z')
    {
        // TODO: to be supported later
        (*context->format) ++;
    }

    context->alt_form = false;
    if (**context->format == '#')
    {
        context->alt_form = true;
        (*context->format) ++;
    }

    if (**context->format == '0')
    {
        // TODO: to be supported later
        // context->align_dir = EQ;
        // context->align_char = '0';
        (*context->format) ++;
    }

    if (isdigit(**context->format))
    {
        status = nstring_s_fmt_f_spec_width(context);
        if (exit_err(status))
        {
            log("nstring format width error")
            return status;
        }
    }

    if (**context->format == '.')
    {
        (*context->format) ++;
        status = nstring_s_fmt_f_spec_prec(context);
        if (exit_err(status))
        {
            log("nstring format alignment error")
            return status;
        }
    }



    return status;
}

exit_status_t nstring_s_fmt_repl_field(struct nstring_fmt_ctx *context)
{
    assert(context != nullptr);

    exit_status_t status = EXIT_STATUS_INVALID_ARGUMENT;
    switch (**context->format)
    {
    case '{':
        status = nstring_append_char((nstring_t*)&(*context->nstring)->string, '{');
        if (exit_err(status))
        {
            log("nstring append char error")
        }
        return status;
    case ':':
        // "... {:a ..."
        //       -^
        (*context->format) ++;
        status = nstring_s_fmt_f_spec(context);
        if (exit_err(status))
        {
            log("nstring format format spec error")
        }
        return status;
    default:
        log("invalid format");
        break;
    }

    return status;
}

exit_status_t nstring_from_format(nstring_t *string, const char *format, ...)
{
    if (string == nullptr || *string == nullptr || format == nullptr)
    {
        log("string nor format cannot null");
        return EXIT_STATUS_INVALID_ARGUMENT;
    }

    struct nstring_s_t *nstring = nstring_str_to_struct(*string);

    exit_status_t status = EXIT_STATUS_SUCCESS;

    const size_t format_len = strlen(format);
    if (format_len > nstring->capacity)
    {
        status = nstring_s_resize(&nstring, 2 * format_len);
        if (exit_err(status))
        {
            log("nstring struct resize error");
            return status;
        }
    }

    if (nstring->length > 0)
    {
        status = nstring_s_resize(&nstring, format_len);
        if (exit_err(status))
        {
            log("nstring struct resize error");
            return status;
        }
        *string = nstring_struct_to_str(nstring);
    }

    va_list args;
    for (va_start(args, format); *format != '\0'; format ++)
    {
        if (*format == '{')
        {
            struct nstring_fmt_ctx context = {0};

            context.nstring = &nstring;
            context.format = &format;
            context.args = &args;

            format ++;
            status = nstring_s_fmt_repl_field(&context);
            if (exit_err(status))
            {
                log("nstring format replacement field error")
                goto FORMAT_END;
            }
        }
        else
        {
            status = nstring_append_char(string, *format);
            if (exit_err(status))
            {
                log("nstring append char error")
                goto FORMAT_END;
            }
        }
    }

FORMAT_END:
    va_end(args);

    return status;
}
