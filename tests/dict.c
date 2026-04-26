#define __DICT_C__

#include <stdlib.h>

#include <arikkei/arikkei-dict.h>
#include <arikkei/arikkei-utils.h>

#include "unity/unity.h"

void
test_dict()
{
    ArikkeiDict dict;
    /* Test adding and removing elements */
    for (unsigned int add = 10; add <= 100000; add *= 10) {
        fprintf(stdout, "Add %u elements\n", add);
        arikkei_dict_setup_int32(&dict, 3);
        srand(0);
        for (unsigned int i = 0; i < add; i++) {
            int key = rand();
            int val = rand();
            void *key_ptr = ARIKKEI_INT_TO_POINTER(key);
            void *val_ptr = ARIKKEI_INT_TO_POINTER(val);
            arikkei_dict_insert_pval(&dict, key_ptr, val_ptr);
        }
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
        arikkei_dict_release(&dict);
    }
}
