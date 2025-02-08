#define __ARIKKEI_TOKEN_C__

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

// Disable VS2005 nagging
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdlib.h>

#include "arikkei-token.h"

unsigned int
arikkei_token_is_equal(const ArikkeiToken lhs, const ArikkeiToken rhs) {
    if (lhs.len) {
        if (rhs.len != lhs.len) return 0;
        return !memcmp(lhs.cdata, rhs.cdata, lhs.len);
    } else {
        return !rhs.len;
    }
}

uint8_t *
arikkei_token_strdup(const ArikkeiToken token) {
    uint8_t *b = malloc(token.len + 1);
    memcpy(b, token.cdata, token.len);
    b[token.len] = 0;
    return b;
}

uint64_t
arikkei_token_strcpy(const ArikkeiToken token, uint8_t *b) {
    memcpy(b, token.cdata, token.len);
    b[token.len] = 0;
    return token.len;
}

uint64_t
arikkei_token_strncpy(const ArikkeiToken token, uint8_t *b, uint64_t size) {
    if (!size) return 0;
    uint64_t len = (token.len < (size - 1)) ? token.len : size - 1;
    memcpy(b, token.cdata, len);
    b[len] = 0;
    return len;
}

int
arikkei_token_strcmp(const ArikkeiToken token, const uint8_t *str) {
    return arikkei_token_strncmp(token, str, strlen((const char *) str));
}

int
arikkei_token_strncmp(const ArikkeiToken lhs, const uint8_t *rhs, uint64_t rhs_len) {
    if (!lhs.len) {
        if (rhs_len) {
            uint64_t clen;
            int cval;
            clen = (lhs.len < rhs_len) ? lhs.len : rhs_len;
            cval = memcmp(lhs.cdata, rhs, clen);
            if (cval) return cval;
            if (lhs.len < rhs_len) return -1;
            if (lhs.len > rhs_len) return 1;
            return 0;
        } else {
            return 1;
        }
    } else {
        if (rhs_len > 0) {
            return -1;
        } else {
            return 0;
        }
    }
}

ArikkeiToken
arikkei_token_get_line(const ArikkeiToken token, uint64_t start) {
    if (start < token.len) {
        const uint8_t *p = token.cdata;
        uint64_t e = start;
        while ((e < token.len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
        return (ArikkeiToken) {p + start, e - start};
    } else {
        return (ArikkeiToken) {token.cdata + token.len, 0};
    }
}

ArikkeiToken
arikkei_token_next_line(const ArikkeiToken token, const ArikkeiToken line) {
    uint64_t start = line.cdata + line.len - token.cdata;
    if (start < token.len) {
        const uint8_t *p = token.cdata;
        uint64_t s = start;
        while ((s < token.len) && ((p[s] < 32) && (p[s] != 9))) s += 1;
        uint64_t e = s;
        while ((e < token.len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
        return (ArikkeiToken) {p + start, e - start};
    } else {
        return (ArikkeiToken) {token.cdata + token.len, 0};
    }
}

ArikkeiToken
arikkei_token_get_token(const ArikkeiToken token, uint64_t start, unsigned int space_is_separator) {
    if (start < token.len) {
        const uint8_t *p = token.cdata;
        while ((start < token.len) && ((p[start] < 32) || ((p[start] == 32) && space_is_separator))) start += 1;
        uint64_t e = start;
        while ((e < token.len) && ((p[e] > 32) || ((p[e] == 32) && !space_is_separator))) e += 1;
        return (ArikkeiToken) {p + start, e - start};
    } else {
        return (ArikkeiToken) {token.cdata + token.len, 0};
    }
}

ArikkeiToken
arikkei_token_next_token(const ArikkeiToken token, const ArikkeiToken prev, unsigned int space_is_separator) {
    return arikkei_token_get_token(token, prev.cdata + prev.len - token.cdata, space_is_separator);
}

unsigned int
arikkei_token_tokenize(ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, unsigned int space_is_separator, unsigned int multi) {
    const uint8_t *p;
    uint64_t s;
    int ntokens;
    if (arikkei_token_is_empty(token)) return 0;
    ntokens = 0;
    p = token->cdata;
    s = 0;
    while ((s < token->len) && (ntokens < maxtokens)) {
        uint64_t e;
        e = s;
        while ((e < token->len) && ((p[e] > 32) || ((p[e] == 32) && !space_is_separator))) e += 1;
        if (ntokens == (maxtokens - 1)) {
            while ((e < token->len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
        }
        arikkei_token_set_from_data(tokens + ntokens, token->cdata, s, e);
        s = e + 1;
        if (multi) {
            while ((s < token->len) && ((p[s] < 32) || ((p[s] == 32) && space_is_separator))) s += 1;
        }
        ntokens += 1;
    }
    return ntokens;
}

unsigned int
arikkei_token_tokenize_ws(ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, const uint8_t *ws, unsigned int multi) {
    uint64_t len, s;
    int ntokens;
    if (arikkei_token_is_empty(token)) return 0;
    len = strlen((const char *) ws);
    ntokens = 0;
    s = 0;
    while ((s < token->len) && (ntokens < maxtokens)) {
        uint64_t e;
        if (ntokens != (maxtokens - 1)) {
            e = s;
            while (e < token->len) {
                uint64_t i;
                for (i = 0; i < len; i++) {
                    if (token->cdata[e] == ws[i]) break;
                }
                if (i < len) break;
                e += 1;
            }
        } else {
            e = token->len;
        }
        arikkei_token_set_from_data(tokens + ntokens, token->cdata, s, e);
        s = e + 1;
        if (multi) {
            while (s < token->len) {
                uint64_t i;
                for (i = 0; i < len; i++) {
                    if (token->cdata[s] == ws[i]) break;
                }
                if (i < len) break;
                s += 1;
            }
        }
        ntokens += 1;
    }
    return ntokens;
}

ArikkeiToken *
arikkei_token_strip_start(const ArikkeiToken *token, ArikkeiToken *dst) {
    const uint8_t *p;
    uint64_t s;
    p = token->cdata;
    s = 0;
    if (p) {
        while ((s < token->len) && (p[s] <= 32)) s += 1;
    }
    arikkei_token_set_from_data(dst, token->cdata, s, token->len);
    return dst;
}

ArikkeiToken *
arikkei_token_strip_start_ws(ArikkeiToken *token, ArikkeiToken *dst, const uint8_t *ws) {
    uint64_t len, s;
    len = strlen((const char *) ws);
    s = 0;
    if (token->cdata) {
        while (s < token->len) {
            uint64_t i;
            for (i = 0; i < len; i++) {
                if (token->cdata[s] == ws[i]) break;
            }
            if (i >= len) break;
            s += 1;
        }
    }
    arikkei_token_set_from_data(dst, token->cdata, s, token->len);
    return dst;
}

ArikkeiToken *
arikkei_token_strip_end(ArikkeiToken *token, ArikkeiToken *dst) {
    const uint8_t *p;
    int e;
    p = token->cdata;
    e = (int) token->len - 1;
    if (p) {
        while ((e >= 0) && (p[e] <= 32)) e -= 1;
    }
    arikkei_token_set_from_data(dst, token->cdata, 0, e + 1);
    return dst;
}

ArikkeiToken *
arikkei_token_strip_end_ws(ArikkeiToken *token, ArikkeiToken *dst, const uint8_t *ws) {
    uint64_t len;
    int e;
    len = strlen((const char *) ws);
    e = (int) token->len - 1;
    if (token->cdata) {
        while (e >= 0) {
            uint64_t i;
            for (i = 0; i < len; i++) {
                if (token->cdata[e] == ws[i]) break;
            }
            if (i >= len) break;
            e -= 1;
        }
    }
    arikkei_token_set_from_data(dst, token->cdata, 0, e + 1);
    return dst;
}

ArikkeiToken *
arikkei_token_strip(ArikkeiToken *token, ArikkeiToken *dst) {
    const uint8_t *p;
    int s;
    int e;
    p = token->cdata;
    s = 0;
    e = (int) token->len - 1;
    if (p) {
        while ((s < (int) token->len) && (p[s] <= 32)) s += 1;
        while ((e >= s) && (p[e] <= 32)) e -= 1;
    }
    arikkei_token_set_from_data(dst, token->cdata, s, e + 1);
    return dst;
}

ArikkeiToken *
arikkei_token_strip_ws(ArikkeiToken *token, ArikkeiToken *dst, const uint8_t *ws) {
    uint64_t len;
    int s, e;
    len = strlen((const char *) ws);
    s = 0;
    e = (int) token->len - 1;
    if (token->cdata) {
        while (s < (int) token->len) {
            uint64_t i;
            for (i = 0; i < len; i++) {
                if (token->cdata[s] == ws[i]) break;
            }
            if (i >= len) break;
            s += 1;
        }
        while (e >= s) {
            uint64_t i;
            for (i = 0; i < len; i++) {
                if (token->cdata[e] == ws[i]) break;
            }
            if (i >= len) break;
            e -= 1;
        }
    }
    arikkei_token_set_from_data(dst, token->cdata, s, e + 1);
    return dst;
}

uint8_t *
arikkei_token_strconcat(const ArikkeiToken *tokens, int size, const uint8_t *separator) {
    uint8_t *str, *p;
    uint64_t slen, len;
    int i;
    slen = strlen((const char *) separator);
    len = 1;
    for (i = 0; i < size; i++) len += tokens[i].len;
    len += (size - 1) * slen;
    str = malloc(len + 1);
    p = str;
    for (i = 0; i < size; i++) {
        if ((i > 0) && (slen > 0)) {
            strncpy((char *) p, (const char *) separator, slen);
            p += slen;
        }
        p += arikkei_token_strcpy(tokens[i], p);
    }
    return str;
}

uint8_t *
arikkei_token_join(const ArikkeiToken tokens[], int n_tokens, const ArikkeiToken separator)
{
    uint8_t *str, *p;
    uint64_t len;
    int i;
    len = 0;
    if (n_tokens) {
        for (i = 0; i < n_tokens; i++) len += tokens[i].len;
        len += (n_tokens - 1) * separator.len;
    }
    str = malloc(len + 1);
    p = str;
    for (i = 0; i < n_tokens; i++) {
        if (i > 0) {
            memcpy(p, separator.cdata, separator.len);
            p += separator.len;
        }
        memcpy(p, tokens[i].cdata, tokens[i].len);
        p += tokens[i].len;
    }
    return str;
}






