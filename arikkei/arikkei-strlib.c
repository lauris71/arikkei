#define __ARIKKEI_STRLIB_C__

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

#include <math.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "arikkei-strlib.h"

uint64_t
arikkei_memcpy (uint8_t *d, uint64_t d_len, const uint8_t *s, uint64_t s_len)
{
	uint64_t len = s_len;
	if (len > d_len) len = d_len;
	if ((len > 0) && d) memcpy (d, s, len);
	return s_len;
}

uint64_t
arikkei_memcpy_str (uint8_t *d, uint64_t d_len, const uint8_t *s)
{
	uint64_t s_len = (s) ? strlen ((const char *) s) : 0;
	arikkei_memcpy (d, d_len, s, s_len);
	return s_len;
}

uint64_t
arikkei_strncpy (uint8_t *d, uint64_t d_len, const uint8_t *s)
{
	uint64_t len = arikkei_memcpy_str (d, d_len, s);
	if (d && d_len) {
		uint64_t l_len = (len >= d_len) ? d_len - 1 : len;
		d[l_len] = 0;
	}
	return len;
}

uint64_t
arikkei_strncpy_aon (uint8_t *d, uint64_t d_len, const uint8_t *s)
{
	uint64_t len = (s) ? strlen((const char *) s) : 0;
	if (d && (d_len > len)) {
		memcpy(d, s, len);
		d[len] = 0;
	}
	return len;
}

uint64_t
arikkei_strcpy_join (uint8_t *d, uint64_t d_len,
	const uint8_t *srcs[], unsigned int n_srcs, const int64_t lens[],
	const uint8_t *sep, int64_t sep_len)
{
	if (sep_len < 0) {
		sep_len = (sep) ? strlen ((const char *) sep) : 0;
	}
	uint64_t len = 0;
	uint64_t p = 0;
	unsigned int i;
	for (i = 0; i < n_srcs; i++) {
		uint64_t s_len;
		if (i > 0) {
			s_len = sep_len;
			len += s_len;
			if (d && d_len) {
				if ((p + s_len) > (d_len - 1)) s_len = d_len - 1 - p;
				if (d) memcpy(d + p, sep, s_len);
				p += s_len;
			}
		}
		if (lens && (lens[i] >= 0)) {
			s_len = lens[i];
		} else if (srcs[i]) {
			s_len = strlen ((const char *) srcs[i]);
		} else {
			s_len = 0;
		}
		len += s_len;
		if (d && d_len) {
			if ((p + s_len) > (d_len - 1)) s_len = d_len - 1 - p;
			if (d) memcpy(d + p, srcs[i], s_len);
			p += s_len;
		}
	}
	if (d && d_len) {
		d[p] = 0;
	}
	return len;
}

uint8_t *
arikkei_strdup_join (const uint8_t *srcs[], unsigned int n_srcs, const int64_t lens[],
	const uint8_t *sep, int64_t sep_len)
{
	uint64_t len = arikkei_strcpy_join(NULL, 0, srcs, n_srcs, lens, sep, sep_len);
	uint8_t *d = (uint8_t *) malloc(len + 1);
	arikkei_strcpy_join(d, len + 1, srcs, n_srcs, lens, sep, sep_len);
	return d;
}

unsigned int
arikkei_strtoll (const uint8_t *s, uint64_t len, int64_t *val)
{
	int sign = 1;
	unsigned int p = 0;
	if (!s || !len) return 0;
	uint64_t max = INT64_MAX;
	if (s[0] == '-') {
		sign = -1;
		p = 1;
		max += 1;
	} else if (s[0] == '+') {
		p = 1;
	}
	if ((p >= len) || (s[p] < '0') || (s[p] > '9')) return 0;
	uint64_t lval = 0;
	while ((p < len) && (s[p] >= '0') && (s[p] <= '9')) {
		int64_t nval = lval * 10 + (s[p] - '0');
		if (nval > max) break;
		lval = nval;
		p += 1;
	}
	if (val) *val = lval * sign;
	return p;
}

unsigned int
arikkei_strtoull (const uint8_t *s, uint64_t len, uint64_t *val)
{
	unsigned int p = 0;
	if (!s || !len) return 0;
	if (s[0] == '+') {
		p = 1;
	}
	if ((p >= len) || (s[p] < '0') || (s[p] > '9')) return 0;
	uint64_t lval = 0;
	while ((p < len) && (s[p] >= '0') && (s[p] <= '9')) {
		uint64_t p = lval * 10;
		uint64_t q = (s[p] - '0');
		if ((UINT64_MAX - p) < q) break;
		lval = p + q;
		p += 1;
		if (lval > (UINT64_MAX / 10)) break;
	}
	if (val) *val = lval;
	return p;
}

unsigned int
arikkei_strtod_simple (const uint8_t *s, uint64_t len, double *val)
{
	unsigned int p = 0;
	double sign = 1.0;
	double int_v = 0.0;
	double frac_v = 0.0;
	double denom = 1.0;
	unsigned int valid = 0;
	if (!s || !len) return 0;
	if (!strncmp((const char *) s, "NaN", 3)) {
		if (val) *val = NAN;
		return 3;
	}
	if (s[p] == '-') {
		sign = -1;
		p = 1;
	} else if (s[p] == '+') {
		p = 1;
	}
	if (!strncmp((const char *) s + p, "Infinity", 8)) {
		if (val) *val = sign * INFINITY;
		return p + 8;
	}
	if (p >= len) return 0;
	while ((p < len) && (s[p] >= '0') && (s[p] <= '9')) {
		int_v = 10.0 * int_v + (s[p] - '0');
		valid = 1;
		p += 1;
	}
	if ((p < len) && (s[p] == '.')) {
		p += 1;
		while ((p < len) && (s[p] >= '0') && (s[p] <= '9')) {
			frac_v = 10.0 * frac_v + (s[p] - '0');
			denom *= 10;
			valid = 1;
			p += 1;
		}
		frac_v = frac_v / denom;
	}
	if (!valid) return 0;
	if (val) *val = sign * (int_v + frac_v);
	return p;
}

unsigned int
arikkei_strtod_exp (const uint8_t *s, uint64_t len, double *val)
{
	double lval;
	unsigned int p = arikkei_strtod_simple (s, len, &lval);
	if (!p || (p >= len)) return 0;
	if ((s[p] == 'e') || (s[p] == 'E')) {
		int64_t exval;
		unsigned int exlen = arikkei_strtoll (s + p + 1, len - p - 1, &exval);
		if (exlen) {
			p = p + 1 + exlen;
			lval = lval * pow (10.0, (double) exval);
		}
	}
	if (val) *val = lval;
	return p;
}

unsigned int
arikkei_itoa (uint8_t *d, uint64_t len, int64_t val)
{
	unsigned int negative = 0;
	uint8_t b[32];
	unsigned int bpos = 31;
	if (val < 0) {
		/* Have to handle specific case if val = INT64_MIN */
		negative = 1;
		b[bpos] = '0' - (val % 10);
		val /= 10;
		val = -val;
	} else {
		b[bpos] = '0' + (val % 10);
		val /= 10;
	}
	while (val != 0) {
		b[--bpos] = '0' + (val % 10);
		val /= 10;
	}
	unsigned int rlen = negative + (32 - bpos);
	if (d && (rlen < len)) {
		unsigned int p = 0;
		if (negative) d[p++] = '-';
		memcpy (d + p, &b[bpos], 32 - bpos);
		d[rlen] = 0;
	}
	return rlen;
}

unsigned int
arikkei_dtoa_simple (uint8_t *d, uint64_t len, double val, unsigned int precision, unsigned int min_frac, unsigned int max_frac)
{
	/* Special cases */
	if (!isfinite(val)) {
		if (isnan(val)) {
			return arikkei_strncpy_aon(d, len, (const uint8_t *) "NaN");
		}
		if (val > 0) {
			return arikkei_strncpy_aon(d, len, (const uint8_t *) "Infinity");
		} else {
			return arikkei_strncpy_aon(d, len, (const uint8_t *) "-Infinity");
		}
		return 0;
	}
	/* Clean up precision */
	if (precision > 16) precision = 16;
	if (min_frac > 16) min_frac = 16;
	if (max_frac > 64) max_frac = 64;
	if (max_frac < min_frac) max_frac = min_frac;

	unsigned int negative = 0;
	if (val < 0) {
		negative = 1;
		val = -val;
	}

	double int_val;
	double frac_val = modf(val, &int_val);

	/* Find epsilon */
	/* Integral part */
	unsigned int nonzero = 0;
	unsigned int l_prec = precision;
	if (int_val >= 1.0) {
		nonzero = 1;
		while (int_val >= 1.0) {
			if (l_prec) l_prec -= 1;
			int_val /= 10;
		}
	}
	/* Fractional part */
	unsigned int l_min_frac = min_frac;
	unsigned int l_max_frac = max_frac;
	unsigned int n_frac_digits = 0;
	while (((l_prec > 0) && (l_max_frac > 0)) || (l_min_frac > 0)) {
		frac_val = modf(frac_val * 10, &int_val);
		n_frac_digits += 1;
		if (int_val) nonzero = 1;
		if (nonzero && (l_prec > 0)) l_prec -= 1;
		if (l_max_frac > 0) l_max_frac -= 1;
		if (l_min_frac > 0) l_min_frac -= 1;
	}
	double epsilon = pow(10, -((double) n_frac_digits)) / 2;

	/* Recalculate parts */
	frac_val = modf(val + epsilon, &int_val);

	uint8_t b[512];
	unsigned int bi = 512;
	unsigned int di = 0;

	/* We have to re-iterate because the actual digit structure may have changed (0.999 -> 1.000) */
	/* Write integral digits at the end of buffer */
	if (int_val >= 1.0) {
		nonzero = 1;
		while (int_val != 0) {
			if (precision) precision -= 1;
			b[--bi] = '0' + (uint8_t) fmod(int_val, 10);
			int_val = floor(int_val / 10);
		}
	} else {
		nonzero = 0;
		b[--bi] = '0';
	}
	unsigned int int_digits = 512 - bi;

	while (((precision > 0) && (max_frac > 0)) || (min_frac > 0)) {
		frac_val = modf(frac_val * 10, &int_val);
		uint8_t frac_digit = (uint8_t) int_val;
		b[di++] = '0' + frac_digit;
		if (int_val) nonzero = 1;
		if (nonzero && (precision > 0)) precision -= 1;
		if (max_frac > 0) max_frac -= 1;
		if (min_frac > 0) min_frac -= 1;
	}
	unsigned int frac_digits = di;

	/* [SIGN] INTEGRAL [DOT FRACTIONAL] */
	unsigned int s_len = negative + int_digits + (frac_digits > 0) + frac_digits;

	if (d && (s_len < len)) {
		if (negative) *d++ = '-';
		memcpy(d, &b[bi], int_digits);
		d += int_digits;
		if (frac_digits) {
			*d++ = '.';
			memcpy(d, &b[0], frac_digits);
		}
		d[s_len] = 0;
	}
	return s_len;
}

unsigned int
arikkei_dtoa_exp (uint8_t *d, uint64_t len, double val,
		  unsigned int precision, int min_exp, int max_exp)
{
	if (val == 0.0) {
		return arikkei_dtoa_simple (d, len, val, precision, 0, precision);
	}
	double eval = floor (log10 (fabs (val)));
	if (((int) eval >= min_exp) && ((int) eval <= max_exp)) {
		return arikkei_dtoa_simple (d, len, val, precision, 0, precision);
	}
	val = val / pow (10.0, eval);
	uint8_t b[512];
	uint64_t s_len = arikkei_dtoa_simple (b, len, val, precision, 1, precision);
	b[s_len++] = 'e';
	s_len += arikkei_itoa (&b[s_len], len - s_len, (int) eval);
	if (d && (s_len < len)) {
		memcpy(d, b, s_len);
		d[s_len] = 0;
	}
	return s_len;
}

uint64_t
arikkei_unicode_get_utf8_nbytes (unsigned int uval)
{
	if (uval < 0x80) return 1;
	if (uval < 0x800) return 2;
	if (uval < 0x10000) return 3;
	if (uval < 0x110000) return 4;
	return 0;
}

uint64_t
arikkei_unicode_to_utf8 (unsigned int uval, uint8_t *d, uint64_t size)
{
	if ((uval < 0x80) && (size >= 1)) {
		d[0] = (uint8_t) uval;
		return 1;
	} else if ((uval < 0x800) && (size >= 2)) {
		d[0] = 0xc0 | (uval >> 6);
		d[1] = 0x80 | (uval & 0x3f);
		return 2;
	} else if ((uval < 0x10000) && (size >= 3)) {
		d[0] = 0xe0 | (uval >> 12);
		d[1] = 0x80 | ((uval >> 6) & 0x3f);
		d[2] = 0x80 | (uval & 0x3f);
		return 3;
	} else if ((uval < 0x110000) && (size >= 4)) {
		d[0] = 0xf0 | (uval >> 18);
		d[1] = 0x80 | ((uval >> 12) & 0x3f);
		d[2] = 0x80 | ((uval >> 6) & 0x3f);
		d[3] = 0x80 | (uval & 0x3f);
		return 4;
	}
	return 0;
}

uint64_t
arikkei_unicode_get_utf16_nshorts (unsigned int uval)
{
	if (uval < 0x10000) return 1;
	if (uval < 0x110000) return 2;
	return 0;
}

uint64_t
arikkei_unicode_to_utf16 (unsigned int uval, uint16_t *d, uint64_t size)
{
	if ((uval < 0x10000) && (size >= 1)) {
		d[0] = (uint16_t) uval;
		return 1;
	} else if ((uval <= 0x110000) && (size >= 2)) {
		d[0] = 0xd800 | ((uval - 0x10000) >> 10);
		d[1] = 0xdc00 | ((uval - 0x10000) & 0x3ff);
		return 2;
	}
	return 0;
}

int
arikkei_utf8_get_unicode (const uint8_t **sp, uint64_t s_len)
{
	if (!s_len) return -1;
	const uint8_t *s = *sp;
	if ((s[0] & 0x80) == 0x0) {
		*sp += 1;
		return s[0];
	} else if (((s[0] & 0xe0) == 0xc0) && (s_len >= 2) && ((s[1] & 0xc0) == 0x80)) {
		*sp += 2;
		return ((s[0] & 0x1f) << 6) | (s[1] & 0x3f);
	} else if (((*s & 0xf0) == 0xe0) && (s_len >= 3) && ((s[1] & 0xc0) == 0x80) && ((s[2] & 0xc0) == 0x80)) {
		*sp += 3;
		return ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f);
	} else if (((*s & 0xf8) == 0xf0) && (s_len >= 4) && ((s[1] & 0xc0) == 0x80) && ((s[2] & 0xc0) == 0x80) && ((s[3] & 0xc0) == 0x80)) {
		*sp += 4;
		return ((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
	}
	return -1;
}

int
arikkei_utf16_get_unicode (const uint16_t **sp, uint16_t s_len)
{
	if (!s_len) return -1;
	const uint16_t *s = *sp;
	if ((s[0] < 0xd800) || (s[0] >= 0xdc00)) {
		*sp += 1;
		return s[0];
	} else if (s_len >= 2) {
		*sp += 2;
		return (((s[0] & 0x3ff) << 10) | (s[1] & 0x3ff)) + 0x10000;
	}
	return -1;
}

uint64_t
arikkei_utf8_strlen_chars (const uint8_t *s)
{
	if (!s) return 0;
	uint64_t len = 0;
	int uval = arikkei_utf8_get_unicode(&s, 4);
	while (uval > 0) {
		len += 1;
		uval = arikkei_utf8_get_unicode(&s, 4);
	}
	return len;
}

uint64_t
arikkei_utf8_strnlen_chars (const uint8_t *s, uint64_t s_len)
{
	if (!s || !s_len) return 0;
	uint64_t len = 0;
	const uint8_t *e = s + s_len;
	int uval = arikkei_utf8_get_unicode(&s, e - s);
	while (uval > 0) {
		len += 1;
		uval = arikkei_utf8_get_unicode(&s, e - s);
	}
	return len;
}

uint64_t
arikkei_utf8_strlen_utf16 (const uint8_t *s)
{
	if (!s) return 0;
	uint64_t len = 0;
	int uval = arikkei_utf8_get_unicode(&s, 4);
	while (uval > 0) {
		len += arikkei_unicode_get_utf16_nshorts(uval);
		uval = arikkei_utf8_get_unicode(&s, 4);
	}
	return len;
}

uint64_t
arikkei_utf8_strnlen_utf16 (const uint8_t *s, uint64_t s_len)
{
	if (!s || !s_len) return 0;
	uint64_t len = 0;
	const uint8_t *e = s + s_len;
	int uval = arikkei_utf8_get_unicode(&s, e - s);
	while (uval > 0) {
		len += arikkei_unicode_get_utf16_nshorts(uval);
		uval = arikkei_utf8_get_unicode(&s, e - s);
	}
	return len;
}

uint64_t
arikkei_utf16_strlen_chars (const uint16_t *s)
{
	if (!s) return 0;
	uint64_t len = 0;
	int uval = arikkei_utf16_get_unicode(&s, 2);
	while (uval > 0) {
		len += 1;
		uval = arikkei_utf16_get_unicode(&s, 2);
	}
	return len;
}

uint64_t
arikkei_utf16_strnlen_chars (const uint16_t *s, uint64_t s_len)
{
	if (!s || !s_len) return 0;
	uint64_t len = 0;
	const uint16_t *e = s + s_len;
	int uval = arikkei_utf16_get_unicode(&s, e - s);
	while (uval > 0) {
		len += 1;
		uval = arikkei_utf16_get_unicode(&s, e - s);
	}
	return len;
}

uint64_t
arikkei_utf16_strlen_utf8 (const uint16_t *s)
{
	if (!s) return 0;
	uint64_t len = 0;
	int uval = arikkei_utf16_get_unicode(&s, 2);
	while (uval > 0) {
		len += arikkei_unicode_get_utf8_nbytes(uval);
		uval = arikkei_utf16_get_unicode(&s, 2);
	}
	return len;
}

uint64_t
arikkei_utf16_strnlen_utf8 (const uint16_t *s, uint64_t s_len)
{
	if (!s || !s_len) return 0;
	uint64_t len = 0;
	const uint16_t *e = s + s_len;
	int uval = arikkei_utf16_get_unicode(&s, e - s);
	while (uval > 0) {
		len += arikkei_unicode_get_utf8_nbytes(uval);
		uval = arikkei_utf16_get_unicode(&s, e - s);
	}
	return len;
}

uint64_t
arikkei_utf8_to_utf16_strcpy (uint16_t *d, const uint8_t *s)
{
	uint64_t len = 0;
	if (s) {
		int uval = arikkei_utf8_get_unicode(&s, 4);
		while (uval > 0) {
			uint64_t n_shorts = arikkei_unicode_to_utf16(uval, d + len, 2);
			if (!n_shorts) break;
			len += n_shorts;
			uval = arikkei_utf8_get_unicode(&s, 4);
		}
	}
	d[len] = 0;
	return len;
}

uint64_t
arikkei_utf8_to_utf16_strncpy (uint16_t *d, const uint8_t *s, uint64_t d_len_shorts)
{
	if (!d_len_shorts) return 0;
	d_len_shorts -= 1;
	uint64_t len = 0;
	if (s) {
		int uval = arikkei_utf8_get_unicode(&s, 4);
		while ((uval > 0) && d_len_shorts) {
			uint64_t n_shorts = arikkei_unicode_to_utf16 (uval, d + len, d_len_shorts);
			if (!n_shorts) break;
			len += n_shorts;
			d_len_shorts -= n_shorts;
			uval = arikkei_utf8_get_unicode(&s, 4);
		}
	}
	d[len] = 0;
	return len;
}

uint16_t *
arikkei_utf8_to_utf16_strdup (const uint8_t *s)
{
	uint16_t *d;
	uint64_t len;
	len = arikkei_utf8_strlen_utf16 (s);
	d = malloc((len + 1) * 2);
	arikkei_utf8_to_utf16_strcpy (d, s);
	return d;
}

uint16_t *
arikkei_utf8_to_utf16_strndup (const uint8_t *s, uint64_t s_len)
{
	uint16_t *d;
	uint64_t len;
	len = arikkei_utf8_strnlen_utf16 (s, s_len);
	d = malloc ((len + 1) * 2);
	arikkei_utf8_to_utf16_strncpy (d, s, s_len);
	return d;
}

uint64_t
arikkei_utf16_to_utf8_strcpy (uint8_t *d, const uint16_t *s)
{
	uint64_t len = 0;
	if (s) {
		int uval = arikkei_utf16_get_unicode(&s, 2);
		while (uval > 0) {
			uint64_t n_bytes = arikkei_unicode_to_utf8(uval, d + len, 4);
			if (!n_bytes) break;
			len += n_bytes;
			uval = arikkei_utf16_get_unicode(&s, 2);
		}
	}
	d[len] = 0;
	return len;
}

uint64_t
arikkei_utf16_to_utf8_strncpy (uint8_t *d, const uint16_t *s, uint64_t d_len_bytes)
{
	if (!d_len_bytes) return 0;
	d_len_bytes -= 1;
	uint64_t len = 0;
	if (s) {
		int uval = arikkei_utf16_get_unicode(&s, 2);
		while ((uval > 0) && d_len_bytes) {
			uint64_t n_bytes = arikkei_unicode_to_utf8 (uval, d + len, d_len_bytes);
			if (!n_bytes) break;
			len += n_bytes;
			d_len_bytes -= n_bytes;
			uval = arikkei_utf16_get_unicode(&s, 2);
		}
	}
	d[len] = 0;
	return len;
}

uint8_t *
arikkei_utf16_to_utf8_strdup (const uint16_t *s)
{
	uint8_t *d;
	uint64_t len;
	len = arikkei_utf16_strlen_utf8 (s);
	d = malloc(len + 1);
	arikkei_utf16_to_utf8_strcpy (d, s);
	return d;
}

uint8_t *
arikkei_utf16_to_utf8_strndup (const uint16_t *s, uint64_t s_len)
{
	uint8_t *d;
	uint64_t len;
	len = arikkei_utf16_strnlen_utf8(s, s_len);
	d = malloc(len + 1);
	arikkei_utf16_to_utf8_strncpy(d, s, s_len);
	return d;
}

uint64_t
arikkei_utf16_strlen (const uint16_t *s)
{
	if (!s) return 0;
	uint64_t len = 0;
	while(s[len]) len += 1;
	return len;
}

uint64_t
arikkei_utf16_strcpy (uint16_t *d, const uint16_t *s)
{
	uint64_t len = 0;
	if (s) {
		int uval = arikkei_utf16_get_unicode(&s, 2);
		while (uval > 0) {
			uint64_t n_bytes = arikkei_unicode_to_utf16(uval, d + len, 2);
			if (!n_bytes) break;
			len += n_bytes;
			uval = arikkei_utf16_get_unicode(&s, 2);
		}
	}
	d[len] = 0;
	return len;
}

uint64_t
arikkei_utf16_strncpy (uint16_t * d, const uint16_t *s, uint64_t d_len_shorts)
{
	if (!d_len_shorts) return 0;
	d_len_shorts -= 1;
	uint64_t len = 0;
	if (s) {
		int uval = arikkei_utf16_get_unicode(&s, 2);
		while ((uval > 0) && d_len_shorts) {
			uint64_t n_shorts = arikkei_unicode_to_utf16 (uval, d + len, d_len_shorts);
			if (!n_shorts) break;
			len += n_shorts;
			d_len_shorts -= n_shorts;
			uval = arikkei_utf16_get_unicode(&s, 2);
		}
	}
	d[len] = 0;
	return len;
}

uint16_t *
arikkei_utf16_strdup (const uint16_t *s)
{
	uint64_t len = arikkei_utf16_strlen(s) + 1;
	uint16_t *d = (uint16_t *) malloc(len * 2);
	arikkei_utf16_strcpy(d, s);
	return d;
}

uint16_t *
arikkei_utf16_strndup (const uint16_t *s, uint64_t s_len)
{
	uint64_t len = 0;
	if (s) {
		while(s[len] && (len < s_len)) len += 1;
	}
	len += 1;
	uint16_t *d = (uint16_t *) malloc(len * 2);
	arikkei_utf16_strncpy(d, s, s_len);
	return d;
}

