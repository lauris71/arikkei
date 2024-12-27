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
#include <stdlib.h>
#include <ctype.h>

#include <arikkei/arikkei-strlib.h>

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

ArikkeiToken *arikkei_token_set_from_string(ArikkeiToken *token, const uint8_t *cdata);
ArikkeiToken *arikkei_token_set_from_data(ArikkeiToken *token, const uint8_t *cdata, uint64_t start, uint64_t end);
ArikkeiToken *arikkei_token_set_from_token(ArikkeiToken *token, const ArikkeiToken *src);

#define arikkei_token_is_valid(t) ((t)->cdata != NULL)
#define arikkei_token_is_empty(t) (!(t)->cdata || ((t)->len == 0))

unsigned int arikkei_token_is_equal(const ArikkeiToken *token, const ArikkeiToken *other);
unsigned int arikkei_token_is_equal_string(const ArikkeiToken *token, const unsigned char *str);

uint8_t *arikkei_token_strdup(const ArikkeiToken *token);
uint64_t arikkei_token_strcpy(const ArikkeiToken *token, uint8_t *d);
/*
 * Unlike strncpy the terminating \0 is enforced
 * Returns the number of characters copied (excluding \0)
 */
uint64_t arikkei_token_strncpy(const ArikkeiToken *token, uint8_t *d, uint64_t size);
int arikkei_token_strcmp(const ArikkeiToken *token, const uint8_t *str);
int arikkei_token_strncmp(const ArikkeiToken *token, const uint8_t *str, uint64_t size);

ArikkeiToken *arikkei_token_get_first_line(const ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_get_line(const ArikkeiToken *token, ArikkeiToken *dst, uint64_t start);
ArikkeiToken *arikkei_token_next_line(const ArikkeiToken *token, ArikkeiToken *dst, const ArikkeiToken *line);
ArikkeiToken *arikkei_token_get_token(const ArikkeiToken *token, ArikkeiToken *dst, uint64_t start, unsigned int space_is_separator);
ArikkeiToken *arikkei_token_next_token(const ArikkeiToken *token, ArikkeiToken *dst, const ArikkeiToken *prev, unsigned int space_is_separator);

unsigned int arikkei_token_tokenize(ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, unsigned int space_is_separator, unsigned int multi);
unsigned int arikkei_token_tokenize_ws(ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, const unsigned char *ws, unsigned int multi);

ArikkeiToken *arikkei_token_strip_start(ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_start_ws(ArikkeiToken *token, ArikkeiToken *dst, const unsigned char *ws);
ArikkeiToken *arikkei_token_strip_end(ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_end_ws(ArikkeiToken *token, ArikkeiToken *dst, const unsigned char *ws);
ArikkeiToken *arikkei_token_strip(ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_ws(ArikkeiToken *token, ArikkeiToken *dst, const unsigned char *ws);

unsigned char *arikkei_token_strconcat(const ArikkeiToken *tokens, int size, const unsigned char *separator);

#ifdef __cplusplus
}
#endif

#endif





