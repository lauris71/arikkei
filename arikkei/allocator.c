#define __ARIKKEI_ALLOCATOR_C__

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

#include <stdlib.h>

#include "arikkei-utils.h"

#include "allocator.h"

struct _ArikkeiPoolChunk {
	ArikkeiPoolChunk *next;
	/* Element data follows, padded to 16-byte boundary */
};

/* The offset of element data from the chunk start */
#define ARIKKEI_POOL_DATA_OFFSET ((sizeof (ArikkeiPoolChunk) + 15) & ~(size_t) 15)

void
arikkei_pool_setup (ArikkeiPool *pool, unsigned int element_size, unsigned int pool_size)
{
	arikkei_return_if_fail (pool != NULL);
	arikkei_return_if_fail (element_size > 0);
	arikkei_return_if_fail (pool_size > 0);
	/* Element has to hold the free list pointer and preserve 8-byte alignment */
	if (element_size < sizeof (void *)) element_size = sizeof (void *);
	pool->element_size = (element_size + 7) & ~7u;
	pool->pool_size = pool_size;
	pool->n_elements = 0;
	pool->free_list = NULL;
	pool->chunks = NULL;
}

void
arikkei_pool_release (ArikkeiPool *pool)
{
	arikkei_return_if_fail (pool != NULL);
	while (pool->chunks) {
		ArikkeiPoolChunk *next = pool->chunks->next;
		free (pool->chunks);
		pool->chunks = next;
	}
	pool->element_size = 0;
	pool->pool_size = 0;
	pool->n_elements = 0;
	pool->free_list = NULL;
}

static unsigned int
arikkei_pool_grow (ArikkeiPool *pool)
{
	size_t size = ARIKKEI_POOL_DATA_OFFSET + (size_t) pool->element_size * pool->pool_size;
	ArikkeiPoolChunk *chunk = (ArikkeiPoolChunk *) malloc (size);
	uint8_t *data;
	unsigned int i;
	if (!chunk) return 0;
	chunk->next = pool->chunks;
	pool->chunks = chunk;
	/* Link all new elements into the free list */
	data = (uint8_t *) chunk + ARIKKEI_POOL_DATA_OFFSET;
	for (i = 0; i < pool->pool_size; i++) {
		void *elem = data + (size_t) i * pool->element_size;
		*(void **) elem = pool->free_list;
		pool->free_list = elem;
	}
	return 1;
}

void *
arikkei_pool_alloc (ArikkeiPool *pool)
{
	void *elem;
	arikkei_return_val_if_fail (pool != NULL, NULL);
	if (!pool->free_list && !arikkei_pool_grow (pool)) return NULL;
	elem = pool->free_list;
	pool->free_list = *(void **) elem;
	pool->n_elements += 1;
	return elem;
}

void
arikkei_pool_free (ArikkeiPool *pool, void *elem)
{
	arikkei_return_if_fail (pool != NULL);
	if (!elem) return;
	*(void **) elem = pool->free_list;
	pool->free_list = elem;
	pool->n_elements -= 1;
}
