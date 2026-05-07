#define __DICT_C__

#include <stdlib.h>
#include <string.h>

#include <arikkei/arikkei-dict.h>
#include <arikkei/arikkei-utils.h>

#include "unity/unity.h"

static unsigned int key_size = 8;
static unsigned int val_size = 8;

unsigned int n_keys = 0;

static void
copy_key (void *dst, const void *src)
{
    memcpy(dst, src, key_size);
    n_keys += 1;
}

static void
free_key (void *key)
{
    int *k = (int *) key;
    n_keys -= 1;
}

static void
copy_val (void *dst, const void *src)
{
    memcpy(dst, src, val_size);
}

static void
generate_key(uint8_t key[], unsigned int size, unsigned int idx)
{
	srand(idx);
    for (unsigned int i = 0; i < size; i++) {
        key[i] = (uint8_t) rand();
    }
}

static void
generate_val(uint8_t val[], unsigned int size, unsigned int idx)
{
	srand(idx ^ 0x12345678);
    for (unsigned int i = 0; i < size; i++) {
        val[i] = (uint8_t) rand();
    }
}

static unsigned int
val_hash(const void *data)
{
    return arikkei_memory_hash(data, val_size);
}

static unsigned int
val_equal(const void *lhs, const void *rhs)
{
    return arikkei_memory_equal(lhs, rhs, val_size);
}

static unsigned int
remove_odd (const void *key, const void *val, void *data)
{
    unsigned int hval = arikkei_memory_hash(key, key_size);
    return hval & 1;
}

static void
test_dict_n_elements(unsigned int add)
{
    uint8_t key[32], val[32];
    unsigned int n_entries, n_root, longest_chain, n_free;
    ArikkeiDict dict;
    arikkei_dict_setup_sizes(&dict, 3, key_size, val_size, val_hash, val_equal);
    dict.copy_key = copy_key;
    dict.free_key = free_key;
    /* Add random elements */
    for (unsigned int i = 0; i < add; i++) {
        generate_key(key, key_size, i);
        if (arikkei_dict_exists(&dict, key)) {
            fprintf(stderr, "Key %u already exists\n", i);
            arikkei_dict_release(&dict);
            return;
        }
        generate_val(val, val_size, i);
        arikkei_dict_insert(&dict, key, val);
    }
    arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
    fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
    fprintf(stdout, "Num keys: %u\n", n_keys);
    TEST_ASSERT(n_keys == add);
    /* Check that all elements exist */
    for (unsigned int i = 0; i < add; i++) {
        generate_key(key, key_size, i);
        generate_val(val, val_size, i);
        TEST_ASSERT(arikkei_dict_exists(&dict, key));
        uint8_t *ptr = (uint8_t *) arikkei_dict_lookup(&dict, key);
        TEST_ASSERT(ptr);
        TEST_ASSERT(!memcmp(ptr, val, val_size));
    }
    /* Remove odd elements*/
    for (unsigned int i = 0; i < add; i++) {
        generate_key(key, key_size, i);
        if (i & 1) {
            TEST_ASSERT(arikkei_dict_remove(&dict, key));
            TEST_ASSERT(!arikkei_dict_exists(&dict, key));
        } else {
            TEST_ASSERT(arikkei_dict_exists(&dict, key));
        }
    }
    arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
    fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
    fprintf(stdout, "Num keys: %u\n", n_keys);
    TEST_ASSERT(n_keys == (add - (add / 2)));
    /* Remove even elements*/
    for (unsigned int i = 0; i < add; i++) {
        generate_key(key, key_size, i);
        if (i & 1) {
            TEST_ASSERT(!arikkei_dict_exists(&dict, key));
        } else {
            TEST_ASSERT(arikkei_dict_remove(&dict, key));
            TEST_ASSERT(!arikkei_dict_exists(&dict, key));
        }
    }
    arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
    fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
    fprintf(stdout, "Num keys: %u\n", n_keys);
    TEST_ASSERT(n_keys == 0);

    /* Add again */
    srand(0);
    for (unsigned int i = 0; i < add; i++) {
        generate_key(key, key_size, i);
        generate_val(val, val_size, i);
        arikkei_dict_insert(&dict, key, val);
    }
    arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
    fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
    fprintf(stdout, "Num keys: %u\n", n_keys);
    TEST_ASSERT(n_keys == add);

    /* Remove odd valued elements*/
    unsigned int n_removed = arikkei_dict_remove_all(&dict, remove_odd, NULL);
    fprintf(stdout, "Removed %u\n", n_removed);
    arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
    fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
    fprintf(stdout, "Num keys: %u\n", n_keys);

    /* Remove even valued elements*/
    srand(0);
    for (unsigned int i = 0; i < add; i++) {
        generate_key(key, key_size, i);
        unsigned int hval = arikkei_memory_hash(key, key_size);
        if (hval & 1) {
            TEST_ASSERT(!arikkei_dict_exists(&dict, key));
        } else {
            TEST_ASSERT(arikkei_dict_remove(&dict, key));
            TEST_ASSERT(!arikkei_dict_exists(&dict, key));
        }
    }
    fprintf(stdout, "Num keys: %u\n", n_keys);
    TEST_ASSERT(n_keys == 0);

    arikkei_dict_release(&dict);
}

void
test_dict()
{
    /* Test adding and removing elements */
    for (unsigned int size = 1; size <= 32; size *= 3) {
        fprintf(stdout, "Test with key/val size %u\n", size);
        for (unsigned int add = 10; add <= 1000000; add *= 10) {
            fprintf(stdout, "Test with %u elements\n", add);
            key_size = size;
            val_size = size;
            test_dict_n_elements(add);
        }
    }
}
