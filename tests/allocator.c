#define __ALLOCATOR_C__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arikkei/allocator.h>

#include "unity/unity.h"

static void
fill_element (uint8_t *elem, unsigned int size, unsigned int tag)
{
    for (unsigned int i = 0; i < size; i++) {
        elem[i] = (uint8_t) (tag ^ (i * 31));
    }
}

static unsigned int
check_element (const uint8_t *elem, unsigned int size, unsigned int tag)
{
    for (unsigned int i = 0; i < size; i++) {
        if (elem[i] != (uint8_t) (tag ^ (i * 31))) return 0;
    }
    return 1;
}

static void
test_pool_basic (unsigned int element_size, unsigned int pool_size, unsigned int n_elems)
{
    ArikkeiPool pool;
    uint8_t **elems = (uint8_t **) malloc (n_elems * sizeof (uint8_t *));
    fprintf (stdout, "Test pool with element size %u, pool size %u, %u elements\n", element_size, pool_size, n_elems);
    arikkei_pool_setup (&pool, element_size, pool_size);
    TEST_ASSERT (pool.element_size >= element_size);
    TEST_ASSERT (pool.element_size >= sizeof (void *));
    TEST_ASSERT_EQUAL_UINT (0, pool.element_size & 7);
    TEST_ASSERT_EQUAL_UINT (0, pool.n_elements);
    /* Allocate more elements than fit into a single chunk */
    for (unsigned int i = 0; i < n_elems; i++) {
        elems[i] = (uint8_t *) arikkei_pool_alloc (&pool);
        TEST_ASSERT_NOT_NULL (elems[i]);
        TEST_ASSERT_EQUAL_UINT64 (0, (uintptr_t) elems[i] & 7);
        if (!(pool.element_size & 15)) {
            TEST_ASSERT_EQUAL_UINT64 (0, (uintptr_t) elems[i] & 15);
        }
        fill_element (elems[i], element_size, i + 1);
        TEST_ASSERT_EQUAL_UINT (i + 1, pool.n_elements);
    }
    /* Elements have to be distinct and keep their contents */
    for (unsigned int i = 0; i < n_elems; i++) {
        for (unsigned int j = i + 1; j < n_elems; j++) {
            TEST_ASSERT (elems[i] != elems[j]);
        }
        TEST_ASSERT (check_element (elems[i], element_size, i + 1));
    }
    /* Free in reverse order */
    for (unsigned int i = n_elems; i > 0; i--) {
        arikkei_pool_free (&pool, elems[i - 1]);
        TEST_ASSERT_EQUAL_UINT (i - 1, pool.n_elements);
    }
    /* Reallocate everything from the free list (no growth needed) */
    for (unsigned int i = 0; i < n_elems; i++) {
        elems[i] = (uint8_t *) arikkei_pool_alloc (&pool);
        TEST_ASSERT_NOT_NULL (elems[i]);
    }
    TEST_ASSERT_EQUAL_UINT (n_elems, pool.n_elements);
    for (unsigned int i = 0; i < n_elems; i++) {
        arikkei_pool_free (&pool, elems[i]);
    }
    TEST_ASSERT_EQUAL_UINT (0, pool.n_elements);
    /* Freeing NULL is allowed */
    arikkei_pool_free (&pool, NULL);
    arikkei_pool_release (&pool);
    free (elems);
}

#define MAX_LIVE 1000

static void
test_pool_fuzz (unsigned int element_size, unsigned int pool_size, unsigned int n_ops)
{
    ArikkeiPool pool;
    static uint8_t *live[MAX_LIVE];
    unsigned int n_live = 0;
    fprintf (stdout, "Fuzz pool with element size %u, pool size %u, %u ops\n", element_size, pool_size, n_ops);
    arikkei_pool_setup (&pool, element_size, pool_size);
    srand (2718);
    for (unsigned int op = 0; op < n_ops; op++) {
        if ((n_live == 0) || ((n_live < MAX_LIVE) && (rand () & 1))) {
            /* Allocate */
            live[n_live] = (uint8_t *) arikkei_pool_alloc (&pool);
            TEST_ASSERT_NOT_NULL (live[n_live]);
            fill_element (live[n_live], element_size, n_live + 1);
            n_live += 1;
        } else {
            /* Free random element */
            unsigned int idx = (unsigned int) rand () % n_live;
            TEST_ASSERT (check_element (live[idx], element_size, idx + 1));
            arikkei_pool_free (&pool, live[idx]);
            n_live -= 1;
            if (idx != n_live) {
                live[idx] = live[n_live];
                fill_element (live[idx], element_size, idx + 1);
            }
        }
        TEST_ASSERT_EQUAL_UINT (n_live, pool.n_elements);
        /* Periodically verify all live elements */
        if (!(op & 1023)) {
            for (unsigned int i = 0; i < n_live; i++) {
                TEST_ASSERT (check_element (live[i], element_size, i + 1));
            }
        }
    }
    for (unsigned int i = 0; i < n_live; i++) {
        TEST_ASSERT (check_element (live[i], element_size, i + 1));
        arikkei_pool_free (&pool, live[i]);
    }
    TEST_ASSERT_EQUAL_UINT (0, pool.n_elements);
    arikkei_pool_release (&pool);
}

void
test_allocator()
{
    /* The main target usage: 16 and 64 byte blocks, <1000 live, frequent alloc/free */
    test_pool_basic (16, 256, 1000);
    test_pool_basic (64, 128, 1000);
    /* Odd sizes are rounded up to pointer size and 8-byte alignment */
    test_pool_basic (1, 16, 100);
    test_pool_basic (13, 3, 100);
    /* Pool size 1 exercises chunk growth on every allocation */
    test_pool_basic (16, 1, 100);
    test_pool_fuzz (16, 64, 100000);
    test_pool_fuzz (64, 16, 100000);
}
