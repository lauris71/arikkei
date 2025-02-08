#ifndef __ARIKKEI_TOKEN_H__
#define __ARIKKEI_TOKEN_H__

/*
 * Arikkei
 *
 * Basic datatypes and code snippets
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 *
 */

#include <stdint.h>
#include <string.h>

#include <arikkei/arikkei-strlib.h>
#include <arikkei/arikkei-utils.h>

#ifndef MIN
#define MIN(l,r) (((r) < (l)) ? (r) : (l))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * As you can guess, it was C++ library initially
 * In plain C is is not half as nice, but still quite useful
 */

typedef struct _ArikkeiToken ArikkeiToken;

struct _ArikkeiToken {
    const uint8_t *cdata;
    uint64_t len;
};

static inline ArikkeiToken
arikkei_token_from_string(const uint8_t *cdata) {
    return (ArikkeiToken) {cdata, (cdata) ? strlen((const char *) cdata) : 0};
}

static inline ArikkeiToken *
arikkei_token_set_from_string(ArikkeiToken *token, const uint8_t *cdata)
{
    token->cdata = cdata;
    token->len = (cdata) ? strlen((const char *) cdata) : 0;
    return token;
}

static inline ArikkeiToken
arikkei_token_from_data(const uint8_t *cdata, uint64_t start, uint64_t end)
{
    return (ArikkeiToken) {cdata + start, end - start};
}

static inline ArikkeiToken *
arikkei_token_set_from_data(ArikkeiToken *token, const uint8_t *cdata, uint64_t start, uint64_t end)
{
    token->cdata = cdata + start;
    token->len = end - start;
    return token;
}

#define arikkei_token_is_valid(t) ((t)->cdata != NULL)
#define arikkei_token_is_empty(t) (!(t)->cdata || !(t)->len)

int arikkei_token_strcmp(const ArikkeiToken token, const uint8_t *str);
int arikkei_token_strncmp(const ArikkeiToken lhs, const uint8_t *rhs, uint64_t rhs_len);

unsigned int arikkei_token_is_equal(const ArikkeiToken lhs, const ArikkeiToken rhs);
static inline unsigned int
arikkei_token_is_equal_str(const ArikkeiToken token, const uint8_t *rhs)
{
    return !arikkei_token_strcmp(token, rhs);
}
static inline int
arikkei_token_compare(const ArikkeiToken lhs, const ArikkeiToken rhs)
{
    return arikkei_token_strncmp(lhs, rhs.cdata, rhs.len);
}

uint8_t *arikkei_token_strdup(const ArikkeiToken token);
uint64_t arikkei_token_strcpy(const ArikkeiToken token, uint8_t *d);
/*
 * Unlike strncpy the terminating \0 is enforced
 * Returns the number of characters copied (excluding \0)
 */
uint64_t arikkei_token_strncpy(const ArikkeiToken token, uint8_t *d, uint64_t size);

/* Line parser */

ArikkeiToken arikkei_token_get_line(const ArikkeiToken token, uint64_t start);
static inline ArikkeiToken
arikkei_token_get_first_line(const ArikkeiToken token) {
    return arikkei_token_get_line(token, 0);
}
ArikkeiToken arikkei_token_next_line(const ArikkeiToken token, const ArikkeiToken line);

ArikkeiToken arikkei_token_get_token(const ArikkeiToken token, uint64_t start, unsigned int space_is_separator);
ArikkeiToken arikkei_token_next_token(const ArikkeiToken token, const ArikkeiToken prev, unsigned int space_is_separator);

unsigned int arikkei_token_tokenize(ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, unsigned int space_is_separator, unsigned int multi);
unsigned int arikkei_token_tokenize_ws(ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, const uint8_t *ws, unsigned int multi);

ArikkeiToken *arikkei_token_strip_start(const ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_start_ws(ArikkeiToken *token, ArikkeiToken *dst, const uint8_t *ws);
ArikkeiToken *arikkei_token_strip_end(ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_end_ws(ArikkeiToken *token, ArikkeiToken *dst, const uint8_t *ws);
ArikkeiToken *arikkei_token_strip(ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_ws(ArikkeiToken *token, ArikkeiToken *dst, const uint8_t *ws);

uint8_t *arikkei_token_strconcat(const ArikkeiToken *tokens, int size, const uint8_t *separator);

uint8_t *arikkei_token_join(const ArikkeiToken tokens[], int n_tokens, const ArikkeiToken separator);

#ifdef __cplusplus
}
#endif

#endif





