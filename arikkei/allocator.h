#ifndef __ARIKKEI_ALLOCATOR_H__
#define __ARIKKEI_ALLOCATOR_H__

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

/*
 * General purpose pool allocator for small fixed-size memory blocks
 *
 * Memory is acquired from the OS in chunks of pool_size elements and
 * recycled through an intrusive free list (the link is stored inside the
 * free element itself, so there is no per-element overhead). Both
 * allocation and deallocation are O(1) and normally do not touch the
 * system allocator, so the pool is well suited for extremely frequent
 * allocation/deallocation of small blocks (like 16 or 64 byte structs).
 *
 * The allocator is not thread-safe.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ArikkeiPool ArikkeiPool;
typedef struct _ArikkeiPoolChunk ArikkeiPoolChunk;

struct _ArikkeiPool {
	unsigned int element_size; /* Effective element size (rounded up to hold a pointer, multiple of 8) */
	unsigned int pool_size;    /* Number of elements in a single chunk */
	unsigned int n_elements;   /* Number of currently allocated (live) elements */
	void *free_list;
	ArikkeiPoolChunk *chunks;
};

/**
 * @brief Set up a pool allocator
 *
 * The pool does not allocate any memory during setup, chunks are created
 * on demand when the pool runs out of free elements.
 *
 * The effective element size is the requested size rounded up to hold a
 * pointer and to a multiple of 8. Chunk data is 16-byte aligned, so
 * element sizes that are multiples of 16 (like the typical 16 and 64)
 * yield 16-byte aligned elements.
 *
 * @param pool the pool allocator
 * @param element_size the requested size of a single element in bytes (has to be > 0)
 * @param pool_size the number of elements in a single chunk (has to be > 0)
 */
void arikkei_pool_setup (ArikkeiPool *pool, unsigned int element_size, unsigned int pool_size);
/**
 * @brief Release all memory held by a pool allocator
 *
 * All live elements are invalidated, so normally everything should be
 * freed before release (n_elements can be used to track leaks). The pool
 * is returned to the freshly set-up state (except that element_size and
 * pool_size are zeroed) and can be set up again.
 *
 * @param pool the pool allocator
 */
void arikkei_pool_release (ArikkeiPool *pool);
/**
 * @brief Allocate an element from the pool
 *
 * If there are no free elements a new chunk of pool_size elements is
 * allocated from the OS. The returned memory is uninitialized.
 *
 * @param pool the pool allocator
 * @return the element or NULL if out of memory
 */
void *arikkei_pool_alloc (ArikkeiPool *pool);
/**
 * @brief Return an element to the pool
 *
 * Freeing NULL is a no-op. Freeing an element not owned by the pool or
 * freeing the same element twice is an error (as is with free).
 *
 * @param pool the pool allocator
 * @param elem the element (may be NULL)
 */
void arikkei_pool_free (ArikkeiPool *pool, void *elem);

#ifdef __cplusplus
}
#endif

#endif
