#define __TEST_C__

#include <stdio.h>
#include <string.h>

#include <arikkei/arikkei-strlib.h>

#include "unity/unity.h"

static void test_memcpy();
static void test_join();
static void test_strtoll();
static void test_strtod();
static void test_itoa();
static void test_dtoa();
static void test_utf8();
static void test_utf16();

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

int
main(int argc, const char *argv[])
{
    UNITY_BEGIN();
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "memcpy")) {
            RUN_TEST(test_memcpy);
        } else if (!strcmp(argv[i], "join")) {
            RUN_TEST(test_join);
        } else if (!strcmp(argv[i], "strtoll")) {
            RUN_TEST(test_strtoll);
        } else if (!strcmp(argv[i], "strtod")) {
            RUN_TEST(test_strtod);
        } else if (!strcmp(argv[i], "itoa")) {
            RUN_TEST(test_itoa);
        } else if (!strcmp(argv[i], "dtoa")) {
            RUN_TEST(test_dtoa);
        } else if (!strcmp(argv[i], "utf8")) {
            RUN_TEST(test_utf8);
        } else if (!strcmp(argv[i], "utf8")) {
            RUN_TEST(test_utf16);
        }
    }
    return UNITY_END();
}

static void
test_memcpy()
{
    static const uint8_t *s = (const uint8_t *) "abcdefghijklmnopqrst";
    static const uint8_t *s10 = (const uint8_t *) "abcdefghij";
    uint8_t d[32], t[32];
    /* memcpy */
    TEST_ASSERT_EQUAL_UINT64(0, arikkei_memcpy(NULL, 0, s, 0));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_memcpy(NULL, 0, s, 20));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_memcpy(d, 20, s, 20));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, s, 20);
    memset(d, 1, 32);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_memcpy_str(NULL, 0, s));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_memcpy_str(d, 0, s));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_memcpy_str(d, 10, s));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, s, 10);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_memcpy_str(d, 20, s));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, s, 20);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy(NULL, 0, s));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy_aon(NULL, 0, s));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy(d, 0, s));
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy_aon(d, 0, s));
    memset(d, 1, 32);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy(d, 11, s));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(s10, d, 11);
    memset(d, 1, 32);
    memset(t, 1, 32);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy_aon(d, 11, s));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(t, d, 32);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strncpy(d, 21, s));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(s, d, 21);
}

static void
test_join()
{
    static const char *strs1[] = { "alpha", "beta", "gamma" };
    static int64_t lens1[] = { 5, 4, 5 };
    static const char *strs2[] = { "alpha", NULL, "beta", "gamma" };
    static int64_t lens2[] = { -1, -1, -1, -1 };
    uint8_t d[256];
    /* No lens, no sep */
    TEST_ASSERT_EQUAL_UINT64(14, arikkei_strcpy_join(NULL, 0, (const uint8_t **) strs1, 3, NULL, NULL, 0));
    TEST_ASSERT_EQUAL_UINT64(14, arikkei_strcpy_join(d, 256, (const uint8_t **) strs1, 3, NULL, NULL, 0));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alphabetagamma", 15);
    TEST_ASSERT_EQUAL_UINT64(14, arikkei_strcpy_join(d, 256, (const uint8_t **) strs2, 4, NULL, NULL, 0));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alphabetagamma", 15);
    TEST_ASSERT_EQUAL_UINT64(14, arikkei_strcpy_join(d, 10, (const uint8_t **) strs2, 4, NULL, NULL, 0));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alphabeta", 10);
    /* Lens, no sep */
    TEST_ASSERT_EQUAL_UINT64(14, arikkei_strcpy_join(d, 256, (const uint8_t **) strs1, 3, lens1, NULL, 0));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alphabetagamma", 15);
    TEST_ASSERT_EQUAL_UINT64(14, arikkei_strcpy_join(d, 256, (const uint8_t **) strs2, 4, lens2, NULL, 0));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alphabetagamma", 15);
    /* Sep */
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strcpy_join(d, 256, (const uint8_t **) strs1, 3, lens1, (const uint8_t *) "-!-", 3));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alpha-!-beta-!-gamma", 21);
    TEST_ASSERT_EQUAL_UINT64(18, arikkei_strcpy_join(d, 256, (const uint8_t **) strs1, 3, lens1, (const uint8_t *) "-!-", 2));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alpha-!beta-!gamma", 19);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strcpy_join(d, 256, (const uint8_t **) strs1, 3, lens1, (const uint8_t *) "-!-", -1));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alpha-!-beta-!-gamma", 21);
    TEST_ASSERT_EQUAL_UINT64(20, arikkei_strcpy_join(d, 15, (const uint8_t **) strs1, 3, lens1, (const uint8_t *) "-!-", -1));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(d, (const uint8_t *) "alpha-!-beta-!", 15);
}

static void
test_strtoll()
{
    int64_t val;
    /* Strtod simple */
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll(NULL, 0, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll(NULL, 123, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "", 0, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "", 123, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "-", 10, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "+", 10, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "-+1", 10, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "abc", 3, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) "abc", 123, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtoll((const uint8_t *) ".", 1, &val));
    TEST_ASSERT_EQUAL_UINT32(1, arikkei_strtoll((const uint8_t *) "1.abc", 1, &val));
    TEST_ASSERT_EQUAL_UINT32(1, arikkei_strtoll((const uint8_t *) "1.abc", 5, &val));
    TEST_ASSERT_EQUAL_INT64(1, val);
    TEST_ASSERT_EQUAL_UINT32(10, arikkei_strtoll((const uint8_t *) "0000000001", 10, &val));
    TEST_ASSERT_EQUAL_INT64(1, val);
    TEST_ASSERT_EQUAL_UINT32(11, arikkei_strtoll((const uint8_t *) "+1234567890", 11, &val));
    TEST_ASSERT_EQUAL_INT64(1234567890LL, val);
    /* INT64_MAX */
    TEST_ASSERT_EQUAL_UINT32(19, arikkei_strtoll((const uint8_t *) "9223372036854775807", 19, &val));
    TEST_ASSERT_EQUAL_INT64(9223372036854775807LL, val);
    TEST_ASSERT_EQUAL_UINT32(20, arikkei_strtoll((const uint8_t *) "-9223372036854775808", 20, &val));
    TEST_ASSERT_EQUAL_INT64(-9223372036854775807LL - 1, val);
    /* Overflow */
    TEST_ASSERT_EQUAL_UINT32(18, arikkei_strtoll((const uint8_t *) "9223372036854775817", 19, &val));
    TEST_ASSERT_EQUAL_INT64(922337203685477581LL, val);
    TEST_ASSERT_EQUAL_UINT32(19, arikkei_strtoll((const uint8_t *) "-9223372036854775817", 20, &val));
    TEST_ASSERT_EQUAL_INT64(-922337203685477581LL, val);
    TEST_ASSERT_EQUAL_UINT32(18, arikkei_strtoll((const uint8_t *) "9223372036854775808", 19, &val));
    TEST_ASSERT_EQUAL_INT64(922337203685477580LL, val);
    TEST_ASSERT_EQUAL_UINT32(19, arikkei_strtoll((const uint8_t *) "-9223372036854775809", 20, &val));
    TEST_ASSERT_EQUAL_INT64(-922337203685477580LL, val);
}

static void
test_strtod()
{
    double val;
    /* Strtod simple */
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple(NULL, 0, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple(NULL, 123, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "", 0, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "", 123, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "-", 10, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "+", 10, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "-+1", 10, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "abc", 3, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) "abc", 123, &val));
    TEST_ASSERT_EQUAL_UINT32(0, arikkei_strtod_simple((const uint8_t *) ".", 1, &val));
    TEST_ASSERT_EQUAL_UINT32(2, arikkei_strtod_simple((const uint8_t *) "1.abc", 2, &val));
    TEST_ASSERT_EQUAL_UINT32(2, arikkei_strtod_simple((const uint8_t *) "1.abc", 5, &val));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, val);
    TEST_ASSERT_EQUAL_UINT32(2, arikkei_strtod_simple((const uint8_t *) ".5abc", 2, &val));
    TEST_ASSERT_EQUAL_UINT32(2, arikkei_strtod_simple((const uint8_t *) ".5abc", 5, &val));
    TEST_ASSERT_EQUAL_DOUBLE(0.5, val);
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_strtod_simple((const uint8_t *) "2.5", 3, &val));
    TEST_ASSERT_EQUAL_DOUBLE(2.5, val);
    TEST_ASSERT_EQUAL_UINT32(4, arikkei_strtod_simple((const uint8_t *) "-2.5", 4, &val));
    TEST_ASSERT_EQUAL_DOUBLE(-2.5, val);
    TEST_ASSERT_EQUAL_UINT32(4, arikkei_strtod_simple((const uint8_t *) "+2.5", 4, &val));
    TEST_ASSERT_EQUAL_DOUBLE(2.5, val);
    TEST_ASSERT_EQUAL_UINT32(14, arikkei_strtod_simple((const uint8_t *) "123456789.0625abcdef", 14, &val));
    TEST_ASSERT_EQUAL_DOUBLE(123456789.0625, val);
    TEST_ASSERT_EQUAL_UINT32(25, arikkei_strtod_simple((const uint8_t *) "12345678901234567890.0625", 25, &val));
    TEST_ASSERT_EQUAL_DOUBLE(1.2345678901234567890e19, val);
    /* Specials */
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_strtod_simple((const uint8_t *) "NaN", 3, &val));
    TEST_ASSERT(isnan(val));
    TEST_ASSERT_EQUAL_UINT32(8, arikkei_strtod_simple((const uint8_t *) "Infinity", 8, &val));
    TEST_ASSERT_EQUAL_DOUBLE(INFINITY, val);
    TEST_ASSERT_EQUAL_UINT32(9, arikkei_strtod_simple((const uint8_t *) "+Infinity", 9, &val));
    TEST_ASSERT_EQUAL_DOUBLE(INFINITY, val);
    TEST_ASSERT_EQUAL_UINT32(9, arikkei_strtod_simple((const uint8_t *) "-Infinity", 9, &val));
    TEST_ASSERT_EQUAL_DOUBLE(-INFINITY, val);
    /* strtod exp */
    TEST_ASSERT_EQUAL_UINT32(24, arikkei_strtod_exp((const uint8_t *) "1.2345678901234567890e19", 24, &val));
    TEST_ASSERT_EQUAL_DOUBLE(1.2345678901234567890e19, val);
    TEST_ASSERT_EQUAL_UINT32(24, arikkei_strtod_exp((const uint8_t *) "1.2345678901234567890e19abc", 32, &val));
    TEST_ASSERT_EQUAL_DOUBLE(1.2345678901234567890e19, val);
    TEST_ASSERT_EQUAL_UINT32(25, arikkei_strtod_exp((const uint8_t *) "1.2345678901234567890e-10abc", 32, &val));
    TEST_ASSERT_EQUAL_DOUBLE(1.2345678901234567890e-10, val);
}

static void
test_itoa()
{
    uint8_t d[32];
    /* NULL destination and short length */
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_itoa(NULL, 0, 123));
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_itoa(NULL, 10, 123));
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_itoa((uint8_t *) d, 1, 123));
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_itoa((uint8_t *) d, 4, 123));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123", d, 4);
    /* Numbers */
    TEST_ASSERT_EQUAL_UINT32(1, arikkei_itoa((uint8_t *) d, 32, 0));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "0", d, 2);
    TEST_ASSERT_EQUAL_UINT32(1, arikkei_itoa((uint8_t *) d, 32, 1));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "1", d, 2);
    TEST_ASSERT_EQUAL_UINT32(2, arikkei_itoa((uint8_t *) d, 32, -1));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "-1", d, 3);
    TEST_ASSERT_EQUAL_UINT32(9, arikkei_itoa((uint8_t *) d, 32, 123456789));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123456789", d, 10);
    TEST_ASSERT_EQUAL_UINT32(10, arikkei_itoa((uint8_t *) d, 32, -123456789));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "-123456789", d, 11);
    TEST_ASSERT_EQUAL_UINT32(19, arikkei_itoa((uint8_t *) d, 32, INT64_MAX));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "9223372036854775807", d, 20);
    TEST_ASSERT_EQUAL_UINT32(20, arikkei_itoa((uint8_t *) d, 32, INT64_MIN));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "-9223372036854775808", d, 21);
}

static void
test_dtoa()
{
    uint8_t d[512];
    /* NULL destination and short length */
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple(NULL, 0, 123, 3, 0, 3));
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple(NULL, 10, 123, 3, 0, 3));
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple((uint8_t *) d, 1, 123, 3, 0, 3));

    TEST_ASSERT_EQUAL_UINT32(8, arikkei_dtoa_simple((uint8_t *) d, 35, INFINITY, 30, 30, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "Infinity", d, 8);
    TEST_ASSERT_EQUAL_UINT32(9, arikkei_dtoa_simple((uint8_t *) d, 35, -INFINITY, 30, 30, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "-Infinity", d, 8);
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple((uint8_t *) d, 35, NAN, 30, 30, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "NaN", d, 3);

    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple((uint8_t *) d, 4, 123, 3, 0, 3));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123", d, 4);
    TEST_ASSERT_EQUAL_UINT32(6, arikkei_dtoa_simple((uint8_t *) d, 4, 123, 3, 2, 3));
    TEST_ASSERT_EQUAL_UINT32(6, arikkei_dtoa_simple((uint8_t *) d, 10, 123, 3, 2, 3));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.00", d, 6);
    TEST_ASSERT_EQUAL_UINT32(7, arikkei_dtoa_simple((uint8_t *) d, 10, 123, 30, 2, 3));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.000", d, 7);
    TEST_ASSERT_EQUAL_UINT32(17, arikkei_dtoa_simple((uint8_t *) d, 48, 123, 16, 2, 16));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.0000000000000", d, 17);
    TEST_ASSERT_EQUAL_UINT32(20, arikkei_dtoa_simple((uint8_t *) d, 35, 123, 30, 30, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.0000000000000000", d, 20);

    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple((uint8_t *) d, 35, 123.456, 3, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123", d, 3);
    TEST_ASSERT_EQUAL_UINT32(5, arikkei_dtoa_simple((uint8_t *) d, 35, 123.456, 4, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.5", d, 5);
    TEST_ASSERT_EQUAL_UINT32(7, arikkei_dtoa_simple((uint8_t *) d, 35, 123.456, 6, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.456", d, 7);
    TEST_ASSERT_EQUAL_UINT32(10, arikkei_dtoa_simple((uint8_t *) d, 35, 123.456, 9, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123.456000", d, 10);
    TEST_ASSERT_EQUAL_UINT32(11, arikkei_dtoa_simple((uint8_t *) d, 35, -123.4560001, 9, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "-123.456000", d, 11);

    TEST_ASSERT_EQUAL_UINT32(4, arikkei_dtoa_simple((uint8_t *) d, 35, 0.999999, 3, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "1.00", d, 4);
    TEST_ASSERT_EQUAL_UINT32(8, arikkei_dtoa_simple((uint8_t *) d, 35, 0.999999, 6, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "0.999999", d, 8);
    TEST_ASSERT_EQUAL_UINT32(3, arikkei_dtoa_simple((uint8_t *) d, 35, 123.000123, 3, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "123", d, 3);
    TEST_ASSERT_EQUAL_UINT32(8, arikkei_dtoa_simple((uint8_t *) d, 35, 0.000123, 3, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "0.000123", d, 8);
    TEST_ASSERT_EQUAL_UINT32(7, arikkei_dtoa_simple((uint8_t *) d, 35, 0.0009999, 3, 0, 30));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *) "0.00100", d, 7);
}

static const uint32_t uval_1s_last = 0xffff;
static const uint16_t utf16_1_last[] = {0xffff, 0x00 };
static const uint32_t uval_2s_last = 0x10ffff;
static const uint16_t utf16_2_last[] = {0xdbff, 0xdfff, 0x00 };

static const uint16_t utf16[] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
    0xd8ff, 0xdcff,
    0x00
};

static const uint16_t utf16_e[] = {
    0x65, 0xd8ff, 0x67, 0xdcff, 0x69,
    0x00
};

static void test_utf16()
{
    /* Utf-16 encoding length */
    TEST_ASSERT_EQUAL_INT64(1, arikkei_unicode_get_utf16_nshorts(0));
    TEST_ASSERT_EQUAL_INT64(1, arikkei_unicode_get_utf16_nshorts(uval_1s_last));
    TEST_ASSERT_EQUAL_INT64(2, arikkei_unicode_get_utf16_nshorts(uval_1s_last + 1));
    TEST_ASSERT_EQUAL_INT64(2, arikkei_unicode_get_utf16_nshorts(uval_2s_last));
    TEST_ASSERT_EQUAL_INT64(0, arikkei_unicode_get_utf16_nshorts(uval_2s_last + 1));

    /* Unicode to utf16 */
    uint16_t b[64];
    TEST_ASSERT_EQUAL_INT64(1, arikkei_unicode_to_utf16(uval_1s_last, b, 64));
    TEST_ASSERT_EQUAL_CHAR_ARRAY(utf16_1_last, b, 1);
    TEST_ASSERT_EQUAL_INT64(2, arikkei_unicode_to_utf16(uval_2s_last, b, 64));
    TEST_ASSERT_EQUAL_CHAR_ARRAY(utf16_2_last, b, 2);

    /* Utf16 to unicode */
    const uint16_t *s, *p;
    s = p = utf16_1_last;
    TEST_ASSERT_EQUAL_INT64(uval_1s_last, arikkei_utf16_get_unicode(&p, 2));
    TEST_ASSERT_EQUAL_PTR(s + 1, p);
    s = p = utf16_2_last;
    TEST_ASSERT_EQUAL_INT64(uval_2s_last, arikkei_utf16_get_unicode(&p, 3));
    TEST_ASSERT_EQUAL_PTR(s + 2, p);

    /* uft16 strlen */
    TEST_ASSERT_EQUAL(0, arikkei_utf16_strlen_chars(NULL));
    TEST_ASSERT_EQUAL(0, arikkei_utf16_strnlen_chars(NULL, 10));
    TEST_ASSERT_EQUAL(11, arikkei_utf16_strlen_chars(utf16));
    TEST_ASSERT_EQUAL(11, arikkei_utf16_strnlen_chars(utf16, 20));
    TEST_ASSERT_EQUAL(11, arikkei_utf16_strnlen_chars(utf16, 1000000));
    TEST_ASSERT_EQUAL(8, arikkei_utf16_strnlen_chars(utf16, 8));

    /* Errors */
    s = p = utf16_e + 1;
    TEST_ASSERT_EQUAL_INT64(-1, arikkei_utf16_get_unicode(&p, 2));
    TEST_ASSERT_EQUAL_PTR(s, p);
    s = p = utf16_e + 3;
    TEST_ASSERT_EQUAL_INT64(-1, arikkei_utf16_get_unicode(&p, 2));
    TEST_ASSERT_EQUAL_PTR(s, p);
    TEST_ASSERT_EQUAL(1, arikkei_utf16_strlen_chars(utf16_e));
    TEST_ASSERT_EQUAL(1, arikkei_utf16_strnlen_chars(utf16_e, 5));
}

static const uint32_t uval_1_last = 0x7f;
static const uint8_t utf8_1_last[] = {0b01111111, 0x00 };
static const uint32_t uval_2_last = 0x7ff;
static const uint8_t utf8_2_last[] = {0b11011111, 0b10111111, 0x00 };
static const uint32_t uval_3_last = 0xffff;
static const uint8_t utf8_3_last[] = {0b11101111, 0b10111111, 0b10111111, 0x00 };
static const uint32_t uval_4_last = 0x10ffff;
static const uint8_t utf8_4_last[] = {0b11110100, 0b10001111, 0b10111111, 0b10111111, 0x00 };
static const uint8_t error_vals[] = { 0x00, 0b01111111, 0b11011111 };

static const uint8_t utf8[] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
    0b01111111,
    0b11011111, 0b10111111,
    0b11101111, 0b10111111, 0b10111111,
    0b11110100, 0b10001111, 0b10111111, 0b10111111,
    0x00
};

static void test_utf8()
{
    /* Utf-8 encoding length */
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(0), 1);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_1_last), 1);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_1_last + 1), 2);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_2_last), 2);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_2_last + 1), 3);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_3_last), 3);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_3_last + 1), 4);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_get_utf8_nbytes(uval_4_last), 4);
    TEST_ASSERT_EQUAL_INT64(0, arikkei_unicode_get_utf8_nbytes(uval_4_last + 1));

    /* Unicode to utf8 */
    uint8_t b[64];
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_to_utf8(uval_1_last, b, 64), 1);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(utf8_1_last, b, 1);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_to_utf8(uval_2_last, b, 64), 2);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(utf8_2_last, b, 2);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_to_utf8(uval_3_last, b, 64), 3);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(utf8_3_last, b, 3);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_to_utf8(uval_4_last, b, 64), 4);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(utf8_4_last, b, 4);
    TEST_ASSERT_EQUAL_INT64(arikkei_unicode_to_utf8(uval_4_last + 1, b, 64), 0);

    /* Utf8 to unicode */
    const uint8_t *s, *p;
    s = p = utf8_1_last;
    TEST_ASSERT_EQUAL_INT64(uval_1_last, arikkei_utf8_get_unicode(&p, 2));
    TEST_ASSERT_EQUAL_PTR(s + 1, p);
    s = p = utf8_2_last;
    TEST_ASSERT_EQUAL_INT64(uval_2_last, arikkei_utf8_get_unicode(&p, 3));
    TEST_ASSERT_EQUAL_PTR(s + 2, p);
    s = p = utf8_3_last;
    TEST_ASSERT_EQUAL_INT64(uval_3_last, arikkei_utf8_get_unicode(&p, 4));
    TEST_ASSERT_EQUAL_PTR(s + 3, p);
    s = p = utf8_4_last;
    TEST_ASSERT_EQUAL_INT64(uval_4_last, arikkei_utf8_get_unicode(&p, 5));
    TEST_ASSERT_EQUAL_PTR(s + 4, p);

    /* Invalid utf8 to unicode */
    for (int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            uint8_t c[32];
            memcpy((char *) c, (const char *) utf8_4_last, 32);
            c[i + 1] = error_vals[j];
            const uint8_t *p = &c[0];
            TEST_ASSERT_EQUAL_INT64(-1, arikkei_utf8_get_unicode(&p, 5));
            TEST_ASSERT_EQUAL_PTR(&c, p);
        }
    }

    /* uft8 strlen */
    TEST_ASSERT_EQUAL(0, arikkei_utf8_strlen_chars(NULL));
    TEST_ASSERT_EQUAL(0, arikkei_utf8_strnlen_chars(NULL, 20));
    TEST_ASSERT_EQUAL(14, arikkei_utf8_strlen_chars(utf8));
    TEST_ASSERT_EQUAL(14, arikkei_utf8_strnlen_chars(utf8, 20));
    TEST_ASSERT_EQUAL(14, arikkei_utf8_strnlen_chars(utf8, 1000000));
    TEST_ASSERT_EQUAL(10, arikkei_utf8_strnlen_chars(utf8, 10));
    TEST_ASSERT_EQUAL(11, arikkei_utf8_strnlen_chars(utf8, 11));
    TEST_ASSERT_EQUAL(11, arikkei_utf8_strnlen_chars(utf8, 12));
    TEST_ASSERT_EQUAL(12, arikkei_utf8_strnlen_chars(utf8, 13));
    TEST_ASSERT_EQUAL(12, arikkei_utf8_strnlen_chars(utf8, 14));
    TEST_ASSERT_EQUAL(12, arikkei_utf8_strnlen_chars(utf8, 15));
    TEST_ASSERT_EQUAL(13, arikkei_utf8_strnlen_chars(utf8, 16));
    TEST_ASSERT_EQUAL(13, arikkei_utf8_strnlen_chars(utf8, 17));
    TEST_ASSERT_EQUAL(13, arikkei_utf8_strnlen_chars(utf8, 18));
    TEST_ASSERT_EQUAL(13, arikkei_utf8_strnlen_chars(utf8, 19));
    TEST_ASSERT_EQUAL(14, arikkei_utf8_strnlen_chars(utf8, 20));
    TEST_ASSERT_EQUAL(0, arikkei_utf8_strnlen_chars(utf8 + 12, 256));
}

