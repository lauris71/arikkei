#ifndef __ARIKKEI_STRLIB_H__
#define __ARIKKEI_STRLIB_H__

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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * All methods return the number of bytes that would have been copied if there would have been enough room
 * Destination can be NULL
 */

uint64_t arikkei_memcpy (uint8_t *d, uint64_t d_len, const uint8_t *s, uint64_t s_len);
/* NULL is valid string */
/* Does not terminate sequence with 0 */
uint64_t arikkei_memcpy_str (uint8_t *d, uint64_t d_len, const uint8_t *s);
/* Returns the number of content bytes (excluding the terminating 0) */
/* Unless len is 0, terminating 0 is always written */
uint64_t arikkei_strncpy (uint8_t *d, uint64_t d_len, const uint8_t *s);
/* All or nothing - only copies data if there is room for everything */
uint64_t arikkei_strncpy_aon (uint8_t *d, uint64_t d_len, const uint8_t *s);

/*
 * If lens is NULL or lens[i] <= 0 or sep_len < 0 corresponding string is assumed to be zero-terminated
 */
/* NULL is treated as empty string */
uint64_t arikkei_strcpy_join (uint8_t *d, uint64_t d_len,
	const uint8_t *srcs[], unsigned int n_srcs, const int64_t lens[],
	const uint8_t *sep, int64_t sep_len);
uint8_t *arikkei_strdup_join (const uint8_t *srcs[], unsigned int n_srcs, const int64_t lens[],
	const uint8_t *sep, int64_t sep_len);

/* Return number of bytes consumed, NULL is empty string, val can be NULL  */
unsigned int arikkei_strtoll (const uint8_t *s, uint64_t len, int64_t *val);
unsigned int arikkei_strtoull (const uint8_t *s, uint64_t len, uint64_t *val);
unsigned int arikkei_strtod_simple (const uint8_t *s, uint64_t len, double *val);
unsigned int arikkei_strtod_exp (const uint8_t *s, uint64_t len, double *val);

unsigned int arikkei_itoa (uint8_t *d, uint64_t len, int64_t val);
/**
 * @brief Convert floating point value to simple (non-logarithmic) string
 * 
 * Writes floating point value to buffer as simple string:
 * [-]INTEGRAL_PART[.FRACTIONAL_PART]
 * If the resulting string plus the terminating 0 do not fit into buffer, or if the buffer
 * is NULL, nothing is written.
 * Special values are written as 'Infinity', '-Infinity' and 'NaN'.
 * 
 * @param d destination buffer 
 * @param len the length of destination buffer
 * @param val floating point argument
 * @param precision the requested number of significant digits. The actual number may be bigger because
 * the integral part is not rounded, or lower if fractional part is truncated by max_frac (max 16)
 * @param min_frac the minimum number of fractional digits to write (will be written even if the requested
 * precision is achieved with less), max 16
 * @param max_frac the maxium number of fractional digits to write (the precision will be truncated if
 * not achieved before max_frac is achieved), max 64
 * @return the number of bytes that was or should have been written, excluding terminating 0
 */
unsigned int arikkei_dtoa_simple (uint8_t *d, uint64_t len, double val,
	unsigned int precision, unsigned int min_frac, unsigned int max_frac);
/* If the whole number including terminating 0 does not fit do not write anything */
/* Return the number of content bytes (excluding the terminating 0) */
unsigned int arikkei_dtoa_exp (uint8_t *d, uint64_t len, double val,
			       unsigned int precision, int min_exp, int max_exp);

/* Returns 0 if value is not unicode */
/**
 * @brief get the number of utf8 bytes
 * 
 * Get the number of bytes needed to encode give unicode value in utf8
 * @param uval unicode value
 * @return uint64_t number of bytes or 0 if the value is not unicode
 */
uint64_t arikkei_unicode_get_utf8_nbytes (unsigned int uval);
uint64_t arikkei_unicode_to_utf8 (unsigned int uval, uint8_t *d, uint64_t size);
uint64_t arikkei_unicode_get_utf16_nshorts (unsigned int uval);
uint64_t arikkei_unicode_to_utf16 (unsigned int uval, uint16_t *d, uint64_t size);

/* Return -1 and keep pointer if error */
int arikkei_utf8_get_unicode (const uint8_t **sp, uint64_t s_len);
/* Unpaired ligature is treated as error */
/* The reason is to make strnlen and strncpy consistent */
int arikkei_utf16_get_unicode (const uint16_t **sp, uint16_t s_len);

/* Do not check validity */
uint64_t arikkei_utf16_strlen (const uint16_t *s);
uint64_t arikkei_utf16_strcpy (uint16_t *d, const uint16_t *s);
uint64_t arikkei_utf16_strncpy (uint16_t *d, const uint16_t *s, uint64_t d_len_shorts);
uint16_t *arikkei_utf16_strdup (const uint16_t *s);
uint16_t *arikkei_utf16_strndup (const uint16_t *s, uint64_t s_len);

/* NULL is treated as zero-length string, error as string-terminator */
uint64_t arikkei_utf8_strlen_chars (const uint8_t *s);
uint64_t arikkei_utf8_strnlen_chars (const uint8_t *s, uint64_t s_len);
uint64_t arikkei_utf8_strlen_utf16 (const uint8_t *s);
uint64_t arikkei_utf8_strnlen_utf16 (const uint8_t *s, uint64_t s_len);
uint64_t arikkei_utf16_strlen_chars (const uint16_t *s);
uint64_t arikkei_utf16_strnlen_chars (const uint16_t *s, uint64_t s_len);
uint64_t arikkei_utf16_strlen_utf8 (const uint16_t *s);
uint64_t arikkei_utf16_strnlen_utf8 (const uint16_t *s, uint64_t s_len);

/* Returns the number of content bytes/shorts (excluding the terminating 0) */
uint64_t arikkei_utf8_to_utf16_strcpy (uint16_t *d, const uint8_t *s);
/* Unless len is 0, terminating 0 is always written */
uint64_t arikkei_utf8_to_utf16_strncpy (uint16_t *d, const uint8_t *s, uint64_t d_len_shorts);
uint16_t *arikkei_utf8_to_utf16_strdup (const uint8_t *s);
uint16_t *arikkei_utf8_to_utf16_strndup (const uint8_t *s, uint64_t s_len);

uint64_t arikkei_utf16_to_utf8_strcpy (uint8_t *d, const uint16_t *s);
uint64_t arikkei_utf16_to_utf8_strncpy (uint8_t *d, const uint16_t *s, uint64_t d_len_bytes);
uint8_t *arikkei_utf16_to_utf8_strdup (const uint16_t *s);
uint8_t *arikkei_utf16_to_utf8_strndup (const uint16_t *s, uint64_t s_len);

#define arikkei_utf8_to_ucs2_strdup arikkei_utf8_to_utf16_strdup
#define arikkei_utf8_to_ucs2_strcpy arikkei_utf8_to_utf16_strcpy
#define arikkei_utf8_to_ucs2_strncpy arikkei_utf8_to_utf16_strncpy
#define arikkei_ucs2_get_utf8_nbytes arikkei_utf16_strlen_utf8
#define arikkei_ucs2_n_get_utf8_nbytes arikkei_utf16_strnlen_utf8
#define arikkei_ucs2_strlen arikkei_utf16_strlen
#define arikkei_ucs2_to_utf8_strcpy arikkei_utf16_to_utf8_strcpy
#define arikkei_ucs2_to_utf8_strdup arikkei_utf16_to_utf8_strdup
#define arikkei_ucs2_strdup arikkei_utf16_strdup
#define arikkei_ucs2_strncpy arikkei_utf16_strncpy

#ifdef __cplusplus
};
#endif

#endif
