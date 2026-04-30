#define __DICT_C__

#include <stdlib.h>

#include <arikkei/arikkei-dict.h>
#include <arikkei/arikkei-utils.h>

#include "unity/unity.h"

static unsigned int
remove_odd (const void *lhs_ptr, const void *rhs_ptr, void *data)
{
    void **lhs = (void **) lhs_ptr;
    void **rhs = (void **) rhs_ptr;
    int key = ARIKKEI_POINTER_TO_INT(*lhs);
    int val = ARIKKEI_POINTER_TO_INT(*rhs);
    return key & 1;
}

unsigned int n_keys = 0;

static void
copy_key (void *dst, const void *src)
{
    int *d = (int *) dst;
    int *s = (int *) src;
    *d = *s;
    n_keys += 1;
}

static void
free_key (void *key)
{
    int *k = (int *) key;
    n_keys -= 1;
}

void
test_dict()
{
    unsigned int n_entries, n_root, longest_chain, n_free;
    ArikkeiDict dict;
    /* Test adding and removing elements */
    for (unsigned int add = 10; add <= 100000; add *= 10) {
        fprintf(stdout, "Add %u elements\n", add);
        arikkei_dict_setup_int32(&dict, 3);
        dict.copy_key = copy_key;
        dict.free_key = free_key;
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            void *key_ptr = ARIKKEI_INT_TO_POINTER(key);
            void *val_ptr = ARIKKEI_INT_TO_POINTER(val);
            arikkei_dict_insert_pval(&dict, key_ptr, val_ptr);
        }
        arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
        fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
        fprintf(stdout, "Num keys: %u\n", n_keys);
        TEST_ASSERT(n_keys == add);
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            TEST_ASSERT(arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            void *key_ptr = ARIKKEI_INT_TO_POINTER(key);
            void **ptr = (void **) arikkei_dict_lookup(&dict, &key_ptr);
            TEST_ASSERT(ptr);
            TEST_ASSERT(ARIKKEI_POINTER_TO_INT(*ptr) == val);
        }
        /* Remove odd elements*/
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            if (i & 1) {
                TEST_ASSERT(arikkei_dict_remove_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
                TEST_ASSERT(!arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            } else {
                TEST_ASSERT(arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            }
        }
        arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
        fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
        fprintf(stdout, "Num keys: %u\n", n_keys);
        TEST_ASSERT(n_keys == add / 2);
        /* Remove even elements*/
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            if (i & 1) {
                TEST_ASSERT(!arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            } else {
                TEST_ASSERT(arikkei_dict_remove_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
                TEST_ASSERT(!arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            }
        }
        arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
        fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
        fprintf(stdout, "Num keys: %u\n", n_keys);
        TEST_ASSERT(n_keys == 0);
        /* Add again */
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            void *key_ptr = ARIKKEI_INT_TO_POINTER(key);
            void *val_ptr = ARIKKEI_INT_TO_POINTER(val);
            arikkei_dict_insert_pval(&dict, key_ptr, val_ptr);
        }
        arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
        fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
        fprintf(stdout, "Num keys: %u\n", n_keys);
        TEST_ASSERT(n_keys == add);
        /* Remove odd elements*/
        unsigned int n_removed = arikkei_dict_remove_all(&dict, remove_odd, NULL);
        fprintf(stdout, "Removed %u\n", n_removed);
        arikkei_dict_get_stats(&dict, &n_entries, &n_root, &longest_chain, &n_free);
        fprintf(stdout, "Entries %u roots %u max chain %u free %u\n", n_entries, n_root, longest_chain, n_free);
        fprintf(stdout, "Num keys: %u\n", n_keys);
        /* Remove even elements*/
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            //fprintf(stdout, "Key %d value %d exists %u\n", key, val, arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            if (key & 1) {
                TEST_ASSERT(!arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            } else {
                TEST_ASSERT(arikkei_dict_remove_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
                TEST_ASSERT(!arikkei_dict_exists_pval(&dict, ARIKKEI_INT_TO_POINTER(key)));
            }
        }
        fprintf(stdout, "Num keys: %u\n", n_keys);
        TEST_ASSERT(n_keys == 0);

        arikkei_dict_release(&dict);
    }
}
