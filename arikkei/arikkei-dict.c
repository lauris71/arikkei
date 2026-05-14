#define __ARIKKEI_DICT_C__

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
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "arikkei-dict.h"
#include "arikkei-utils.h"

#ifdef _WIN32
#define aligned_alloc(a,s) _aligned_malloc(s,a)
#define aligned_free(p) _aligned_free(p)
#endif

#define EMPTY 0
#define END 1

struct _ArikkeiDictEntry {
	// 0 - empty entry
	// 1 - end of chain
	uint32_t next;
	uint32_t filler;
	uint8_t key[8];
	uint8_t val[8];
	uint64_t filler2;
};

static ArikkeiDictEntry *
entry_ptr(ArikkeiDict *dict, ArikkeiDictEntry *entries, unsigned int pos)
{
	return (ArikkeiDictEntry *) ((char *) entries + pos * dict->entry_size);
}

static ArikkeiDictEntry *
dict_entry_ptr(ArikkeiDict *dict, unsigned int pos)
{
	return entry_ptr(dict, dict->entries, pos);
}

static void *
key_ptr(ArikkeiDict *dict, ArikkeiDictEntry *entry)
{
	return (char *) entry + dict->key_offset;
}

static void *
val_ptr(ArikkeiDict *dict, ArikkeiDictEntry *entry)
{
	return (char *) entry + dict->val_offset;
}

static void *
dict_key_ptr(ArikkeiDict *dict, unsigned int pos)
{
	ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
	return (char *) entry + dict->key_offset;
}

static void *
dict_val_ptr(ArikkeiDict *dict, unsigned int pos)
{
	ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
	return (char *) entry + dict->val_offset;
}

#define ROOT_ENTRY(d,i) ((ArikkeiDictEntry *) ((char *) (d)->entries + (d)->root_size * (d)->entry_size + (i) * (d)->entry_size))

#define ENTRY(d,i) ((ArikkeiDictEntry *) ((char *) (d)->entries + (i) * (d)->entry_size))
#define ENTRY_IS_EMPTY(e) ((e)->next == EMPTY)
#define ENTRY_IS_LAST(e) ((e)->next == END)

static void
set_entry(ArikkeiDict *dict, unsigned int pos, unsigned int next, const void *key, const void *val, unsigned int replace)
{
	ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
	entry->next = next;
	if (key) {
		void *key_ptr = (char *) entry + dict->key_offset;
		if (replace && dict->free_key) dict->free_key(key_ptr);
		if (dict->copy_key) {
			dict->copy_key(key_ptr, key);
		} else {
			memcpy(key_ptr, key, dict->key_size);
		}
	}
	void *val_ptr = (char *) entry + dict->val_offset;
	if (replace && dict->free_value) dict->free_value(val_ptr);
	if (dict->copy_value) {
		dict->copy_value(val_ptr, val);
	} else {
		memcpy(val_ptr, val, dict->val_size);
	}
}

static inline void
clear_entry(ArikkeiDict *dict, ArikkeiDictEntry *entry)
{
		if (dict->free_key) {
			dict->free_key(key_ptr(dict, entry));
		}
		if (dict->free_value) {
			dict->free_value(val_ptr(dict, entry));
		}
}

static ArikkeiDictEntry *
allocate_entries(unsigned int size, unsigned int root_size, unsigned int entry_size)
{
	ArikkeiDictEntry *entries = (ArikkeiDictEntry *) aligned_alloc (16, size * entry_size);
	memset (entries, 0, size * entry_size);
	for (unsigned int i = root_size; i < size - 1; i++) ((ArikkeiDictEntry *) ((char *) entries + i * entry_size))->next = i + 1;
	((ArikkeiDictEntry *) ((char *) entries + (size - 1) * entry_size))->next = END;
	return entries;
}

void
arikkei_dict_setup_full (ArikkeiDict *dict, unsigned int hashsize,
			 unsigned int (* hash) (const void *),
			 unsigned int (* equal) (const void *, const void *))
{
	arikkei_dict_setup_sizes(dict, hashsize, 8, 8, hash, equal);
}

static unsigned int
align16 (unsigned int v)
{
	return (v + 15) & ~(unsigned int) 15;
}

void
arikkei_dict_setup_sizes (ArikkeiDict *dict, unsigned int hashsize,
			  unsigned int key_size, unsigned int val_size,
			  unsigned int (* hash) (const void *),
			  unsigned int (* equal) (const void *, const void *))
{
	if (hashsize < 3) hashsize = 3;

	memset(dict, 0, sizeof(ArikkeiDict));

	dict->root_size = hashsize;
	dict->size = 3 * hashsize;
	dict->key_offset = 16;
	dict->key_size = key_size;
	dict->val_offset = align16(dict->key_offset + key_size);
	dict->val_size = val_size;
	dict->entry_size = align16(dict->val_offset + val_size);
	dict->entries = allocate_entries(dict->size, dict->root_size, dict->entry_size);

	dict->free = dict->root_size;
	dict->hash = hash;
	dict->equal = equal;
}

static void
dict_reallocate (ArikkeiDict *dict, unsigned int new_root_size)
{
	unsigned int new_size = 3 * new_root_size;
	ArikkeiDictEntry *new_entries =  allocate_entries(new_size, new_root_size, dict->entry_size);
	unsigned int new_free = new_root_size;
	for (unsigned int hval = 0; hval < dict->root_size; hval++) {
		// Skip if root is empty
		ArikkeiDictEntry *root_entry = dict_entry_ptr(dict, hval);
		if(root_entry->next == EMPTY) continue;
		unsigned int pos = hval;
		do {
			ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
			unsigned int new_hval = dict->hash(key_ptr(dict, entry)) % new_root_size;
			ArikkeiDictEntry *new_root_entry = entry_ptr(dict, new_entries, new_hval);
			if (new_root_entry->next == EMPTY) {
				// Add new root
				memcpy(new_root_entry, entry, dict->entry_size);
				new_root_entry->next = END;
			} else {
				unsigned int new_next = new_root_entry->next;
				new_root_entry->next = new_free;
				ArikkeiDictEntry *new_entry = entry_ptr(dict, new_entries, new_free);
				new_free = new_entry->next;
				memcpy(new_entry, entry, dict->entry_size);
				new_entry->next = new_next;
			}
			pos = entry->next;
		} while (pos != END);
	}
	aligned_free (dict->entries);
	dict->entries = new_entries;
	dict->root_size = new_root_size;
	dict->size = new_size;
	dict->free = new_free;
}

void
arikkei_dict_release (ArikkeiDict *dict)
{
	for (unsigned int i = 0; i < dict->root_size; i++) {
		ArikkeiDictEntry *root_entry = dict_entry_ptr(dict, i);
		if (root_entry->next == EMPTY) continue;
		clear_entry(dict, root_entry);
		unsigned int pos = root_entry->next;
		while (pos != END) {
			ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
			clear_entry(dict, entry);
			pos = entry->next;
		}
	}
	aligned_free (dict->entries);
}

void
arikkei_dict_insert(ArikkeiDict *dict, const void *key, const void *val)
{
	unsigned int pos = dict->hash(key) % dict->root_size;
	ArikkeiDictEntry *root_entry = dict_entry_ptr(dict, pos);
	if(root_entry->next == EMPTY) {
		set_entry(dict, pos, END, key, val, 0);
		return;
	}
	if (dict->equal(key, key_ptr(dict, root_entry))) {
		set_entry(dict, pos, root_entry->next, NULL, val, 1);
		return;
	}
	pos = root_entry->next;
	while (pos != END) {
		ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
		if (dict->equal(key, key_ptr(dict, entry))) {
			set_entry(dict, pos, entry->next, NULL, val, 1);
			return;
		}
		pos = entry->next;
	}
	if (dict->free != END) {
		pos = dict->free;
		ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
		dict->free = entry->next;
		set_entry(dict, pos, root_entry->next, key, val, 0);
		root_entry->next = pos;
		return;
	}
	dict_reallocate (dict, dict->root_size << 1);
	arikkei_dict_insert(dict, key, val);
}

unsigned int
arikkei_dict_remove(ArikkeiDict *dict, const void *key)
{
	unsigned int root = dict->hash(key) % dict->root_size;
	ArikkeiDictEntry *root_entry = dict_entry_ptr(dict, root);
	if (root_entry->next == EMPTY) return 0;
	if (dict->equal (key, key_ptr(dict, root_entry))) {
		/* Have to remove root key */
		clear_entry(dict, root_entry);
		/* Move next key to root position */
		if (root_entry->next != END) {
			int pos = root_entry->next;
			ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
			memcpy(root_entry, entry, dict->entry_size);
			/* Add old entry to the free list */
			entry->next = dict->free;
			dict->free = pos;
		} else {
			root_entry->next= EMPTY;
		}
		return 1;
	}
	ArikkeiDictEntry *prev_entry = root_entry;
	int pos = root_entry->next;
	while (pos != END) {
		ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
		if (dict->equal (key, key_ptr(dict, entry))) {
			clear_entry(dict, entry);
			prev_entry->next = entry->next;
			entry->next = dict->free;
			dict->free = pos;
			return 1;
		}
		prev_entry = entry;
		pos = entry->next;
	}
	return 0;
}

void
arikkei_dict_clear (ArikkeiDict *dict)
{
	for (unsigned int i = 0; i < dict->root_size; i++) {
		ArikkeiDictEntry *root_entry = dict_entry_ptr(dict, i);
		if (root_entry->next == EMPTY) continue;
		clear_entry(dict, root_entry);
		unsigned int pos = root_entry->next;
		root_entry->next = EMPTY;
		while (pos != END) {
			ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
			clear_entry(dict, entry);
			pos = entry->next;
		}
	}
	for (unsigned int i = dict->root_size; i < dict->size - 1; i++) {
		dict_entry_ptr(dict, i)->next = i + 1;
	}
	dict_entry_ptr(dict, dict->size - 1)->next = END;
	dict->free = dict->root_size;
}

unsigned int
arikkei_dict_exists(ArikkeiDict *dict, const void *key)
{
	return arikkei_dict_lookup(dict, key) != NULL;
}

unsigned int
arikkei_dict_exists_pval (ArikkeiDict *dict, const void *key)
{
	return arikkei_dict_lookup(dict, &key) != NULL;
}

const void *
arikkei_dict_lookup (ArikkeiDict *dict, const void *key)
{
	unsigned int pos = dict->hash(key) % dict->root_size;
	ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
	if (entry->next == EMPTY) return NULL;
	if (dict->equal (key, key_ptr(dict, entry))) return val_ptr(dict, entry);
	for (pos = entry->next; pos != END; pos = entry->next) {
		entry = dict_entry_ptr(dict, pos);
		if (dict->equal (key, key_ptr(dict, entry))) return val_ptr(dict, entry);
	}
	return NULL;
}

const void *
arikkei_dict_lookup_pval (ArikkeiDict *dict, const void *key)
{
	void **val = (void **) arikkei_dict_lookup(dict, &key);
	return (val) ? *val : NULL;
}

const void *
arikkei_dict_lookup_foreign (ArikkeiDict *dict, const void *foreign_key, unsigned int hash, unsigned int (*equal) (const void *lhs, const void *rhs))
{
	if (!foreign_key) return NULL;
	unsigned int pos = hash % dict->root_size;
	if (dict_entry_ptr(dict, pos)->next == EMPTY) return NULL;
	do {
		ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
		if (equal (foreign_key, key_ptr(dict, entry))) return val_ptr(dict, entry);
		pos = entry->next;
	} while (pos != END);
	return NULL;
}

unsigned int
arikkei_dict_forall (ArikkeiDict *dict, unsigned int (* forall) (const void *, const void *, void *), void *data)
{
	for (unsigned int i = 0; i < dict->root_size; i++) {
		ArikkeiDictEntry *entry = dict_entry_ptr(dict, i);
		if (entry->next == EMPTY) continue;
		unsigned int pos = i;
		do {
			entry = dict_entry_ptr(dict, pos);
			if (!forall (key_ptr(dict, entry), val_ptr(dict, entry), data)) return 0;
			pos = entry->next;
		} while (pos != END);
	}
	return 1;
}

unsigned int
arikkei_dict_remove_all (ArikkeiDict *dict, unsigned int (*remove) (const void *, const void *, void *), void *data)
{
	unsigned int n_removed = 0;
	for (unsigned int hval = 0; hval < dict->root_size; hval++) {
		ArikkeiDictEntry *root_entry = dict_entry_ptr(dict, hval);
		if (root_entry->next == EMPTY) continue;
		unsigned int pos = root_entry->next;
		ArikkeiDictEntry *prev;
		if (remove(key_ptr(dict, root_entry), val_ptr(dict, root_entry), data)) {
			/* Remove root entry */
			clear_entry(dict, root_entry);
			root_entry->next = EMPTY;
			n_removed += 1;
			prev = NULL;
		} else {
			prev = root_entry;
		}
		while (pos != END) {
			ArikkeiDictEntry *entry = dict_entry_ptr(dict, pos);
			unsigned int next = entry->next;
			if (remove(key_ptr(dict, entry), val_ptr(dict, entry), data)) {
				/* Remove this entry */
				clear_entry(dict, entry);
				entry->next = dict->free;
				dict->free = pos;
				if (prev) prev->next = next;
				n_removed += 1;
			} else {
				/* Keep this entry */
				if (!prev) {
					/* Move to root */
					memcpy(root_entry, entry, dict->entry_size);
					prev = root_entry;
					entry->next = dict->free;
					dict->free = pos;
				} else {
					prev = entry;
				}
			}
			pos = next;
		}
	}
	return n_removed;
}

void
arikkei_dict_get_stats (ArikkeiDict *dict, unsigned int *n_entries, unsigned int *n_root, unsigned int *longest_chain, unsigned int *n_free)
{
	*n_entries = *n_root = *longest_chain = 0;
	for (unsigned int i = 0; i < dict->root_size; i++) {
		ArikkeiDictEntry *entry = dict_entry_ptr(dict, i);
		if (entry->next == EMPTY) continue;
		*n_entries += 1;
		*n_root += 1;
		unsigned int chain_length = 0;
		unsigned int pos = entry->next;
		while (pos != END) {
			pos = dict_entry_ptr(dict, pos)->next;
			*n_entries += 1;
			chain_length += 1;
		};
		if (chain_length > *longest_chain) *longest_chain = chain_length;
	}
	unsigned int pos = dict->free;
	while (pos != END) {
		*n_free += 1;
		pos = dict_entry_ptr(dict, pos)->next;
	}
}

unsigned int
arikkei_string_hash (const void *data)
{
	const uint8_t *p = *((const uint8_t **) data);
	unsigned int hval = *p;
	if (hval) {
		for (p = p + 1; *p; p++) hval = (hval << 5) - hval + *p;
	}
	return hval;
}

unsigned int
arikkei_string_equal (const void *l, const void *r)
{
	const char *lhs = *((const char **) l);
	const char *rhs = *((const char **) r);
	return !strcmp (lhs, rhs);
}

unsigned int
arikkei_pointer_hash (const void *data)
{
	return arikkei_memory_hash (data, 8);
}

unsigned int
arikkei_pointer_equal (const void *l, const void *r)
{
	return *((const void **) l) == *((const void **) r);
}

unsigned int
arikkei_int32_hash (const void *data)
{
	uint32_t x = *((const uint32_t *) data);
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return x;
}

unsigned int
arikkei_int32_equal (const void *l, const void *r)
{
	const uint32_t *lhs = (const uint32_t *) l;
	const uint32_t *rhs = (const uint32_t *) r;
	return *lhs == *rhs;
}

unsigned int
arikkei_int64_hash (const void *data)
{
	uint64_t x = *((const uint64_t *) data);
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
	x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
	x = x ^ (x >> 31);
	return (unsigned int) x;
}

unsigned int
arikkei_int64_equal (const void *l, const void *r)
{
	return *((const uint64_t *) l) == *((const uint64_t *) r);
}

uint32_t
arikkei_memory_hash (const void *data, unsigned int size)
{
	const uint8_t *p = (const uint8_t *) data;
	// FNV 1a
	uint32_t hval = 2166136261;
	for (unsigned int i = 0; i < size; i++) {
		hval = (hval ^ p[i]) * 16777619;
	}
	return hval;
}

unsigned int
arikkei_memory_equal (const void *lhs, const void *rhs, unsigned int size)
{
	return !memcmp (lhs, rhs, size);
}


