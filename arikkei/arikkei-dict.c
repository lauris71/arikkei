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

#include "arikkei-dict.h"
#include "arikkei-utils.h"

#define EMPTY 0
#define END 1

struct _ArikkeiDictEntry {
	// 0 - empty entry
	// 1 - end of chain
	uint32_t next;
	uint32_t filler;
	uint8_t key[8];
	uint8_t val[8];
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

#define ROOT_ENTRY(d,i) ((ArikkeiDictEntry *) ((char *) (d)->entries + (d)->root_size * (d)->entry_size + (i) * (d)->entry_size))

#define ENTRY(d,i) ((ArikkeiDictEntry *) ((char *) (d)->entries + (i) * (d)->entry_size))
#define ENTRY_IS_EMPTY(e) ((e)->next == EMPTY)
#define ENTRY_IS_LAST(e) ((e)->next == END)

static void
set_entry(ArikkeiDict *dict, unsigned int pos, unsigned int next, const void *key, const void *val)
{
	ArikkeiDictEntry *entry = ENTRY(dict, pos);
	entry->next = next;
	void *key_ptr = (char *) entry + dict->key_offset;
	if (dict->copy_key) {
		dict->copy_key (key_ptr, key);
	} else {
		memcpy(key_ptr, key, dict->key_size);
	}
	void *val_ptr = (char *) entry + dict->val_offset;
	if (dict->copy_value) {
		dict->copy_value (val_ptr, val);
	} else {
		memcpy(val_ptr, val, dict->val_size);
	}
}

static void *
dict_key_ptr(ArikkeiDict *dict, unsigned int pos)
{
	ArikkeiDictEntry *entry = ENTRY(dict, pos);
	return (char *) entry + dict->key_offset;
}

static void *
dict_val_ptr(ArikkeiDict *dict, unsigned int pos)
{
	ArikkeiDictEntry *entry = ENTRY(dict, pos);
	return (char *) entry + dict->val_offset;
}

static ArikkeiDictEntry *
allocate_entries(unsigned int size, unsigned int root_size, unsigned int entry_size)
{
	ArikkeiDictEntry *entries = (ArikkeiDictEntry *) malloc (size * entry_size);
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
	unsigned int i;
	if (hashsize < 3) hashsize = 3;

	memset(dict, 0, sizeof(ArikkeiDict));

	dict->root_size = hashsize;
	dict->size = 3 * hashsize;
	dict->entry_size = sizeof(ArikkeiDictEntry);
	dict->key_offset = ARIKKEI_OFFSET(ArikkeiDictEntry, key);
	dict->key_size = 8;
	dict->val_offset = ARIKKEI_OFFSET(ArikkeiDictEntry, val);
	dict->val_size = 8;
	dict->entries = allocate_entries(dict->size, dict->root_size, dict->entry_size);

	dict->free = dict->root_size;
	dict->_hash = hash;
	dict->_equal = equal;
}

static void
dict_reallocate (ArikkeiDict *dict, unsigned int hash_size)
{
	ArikkeiDictEntry *entries;
	unsigned int size, next_free;
	size = 3 * hash_size;

	entries =  allocate_entries(size, hash_size, dict->entry_size);

	next_free = hash_size;
	for (unsigned int i = 0; i < dict->root_size; i++) {
		// Skip if root is empty
		if (ENTRY(dict, i)->next == EMPTY) continue;
		unsigned int pos = i;
		do {
			unsigned int hval = dict->_hash(dict_key_ptr(dict, pos)) % hash_size;
			if (entries[hval].next == EMPTY) {
				// Add new root
				memcpy(&entries[hval], &dict->entries[pos], dict->entry_size);
				entries[hval].next = END;
			} else {
				unsigned int next = next_free;
				next_free = entries[next_free].next;
				memcpy(&entries[next], &dict->entries[pos], dict->entry_size);
				entries[next].next = entries[hval].next;
				entries[hval].next = next;
			}
			pos = dict->entries[pos].next;
		} while (pos != END);
	}
	free (dict->entries);
	dict->entries = entries;
	dict->root_size = hash_size;
	dict->size = size;
	dict->free = next_free;
}

void
arikkei_dict_setup_string (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_string_hash, arikkei_string_equal);
}

void
arikkei_dict_setup_pointer (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_pointer_hash, arikkei_pointer_equal);
}

void
arikkei_dict_setup_int32 (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_int32_hash, arikkei_int32_equal);
}

void
arikkei_dict_setup_int64 (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_int64_hash, arikkei_int64_equal);
}

void
arikkei_dict_release (ArikkeiDict *dict)
{
	free (dict->entries);
}

void
arikkei_dict_insert(ArikkeiDict *dict, const void *key, const void *val)
{
	unsigned int pos = dict->_hash(&key) % dict->root_size;
	ArikkeiDictEntry *entry = ENTRY(dict, pos);
	if(ENTRY_IS_EMPTY(entry)) {
		set_entry(dict, pos, END, key, val);
	} else {
	}
}

void
arikkei_dict_insert_pval (ArikkeiDict *dict, const void *key, const void *val)
{
	unsigned int root = dict->_hash(&key) % dict->root_size;
	if (dict->entries[root].next == EMPTY) {
		/* Add root element */
		set_entry(dict, root, END, &key, &val);
		return;
	}
	unsigned int pos = root;
	do {
		if (dict->_equal (&key, dict_key_ptr(dict, pos))) {
			/* Replace element */
			set_entry(dict, pos, dict->entries[pos].next, &key, &val);
			return;
		}
		pos = dict->entries[pos].next;
	} while (pos != END);
	/* Prepend new element */
	if (dict->free != END) {
		pos = dict->free;
		dict->free = dict->entries[pos].next;
		set_entry(dict, pos, dict->entries[root].next, &key, &val);
		dict->entries[root].next = pos;
		return;
	}
	dict_reallocate (dict, dict->root_size << 1);
	arikkei_dict_insert_pval(dict, key, val);
}

void
arikkei_dict_remove (ArikkeiDict *dict, const void *key)
{
	unsigned int root;
	if (!key) return;
	root = dict->_hash(&key) % dict->root_size;
	if (dict->entries[root].next == EMPTY) return;
	if (dict->_equal (&key, dict_key_ptr(dict, root))) {
		/* Have to remove root key */
		if (dict->entries[root].next != END) {
			int pos = dict->entries[root].next;
			dict->entries[root] = dict->entries[pos];
			dict->entries[pos].next = dict->free;
			dict->free = pos;
		} else {
			dict->entries[root].next = EMPTY;
		}
	} else {
		int pos, prev;
		prev = root;
		for (pos = dict->entries[root].next; pos != END; pos = dict->entries[pos].next) {
			if (dict->_equal(&key, dict_key_ptr(dict, pos))) {
				dict->entries[prev].next = dict->entries[pos].next;
				dict->entries[pos].next = dict->free;
				dict->free = pos;
				return;
			}
			prev = pos;
		}
	}
}

void
arikkei_dict_clear (ArikkeiDict *dict)
{
	unsigned int i;
	for (i = 0; i < dict->root_size; i++) dict->entries[i].next = EMPTY;
	for (i = dict->root_size; i < dict->size - 1; i++) dict->entries[i].next = i + 1;
	dict->entries[dict->size - 1].next = END;
	dict->free = dict->root_size;
}

unsigned int
arikkei_dict_exists (ArikkeiDict *dict, const void *key)
{
	unsigned int pos = dict->_hash(&key) % dict->root_size;
	if (dict->entries[pos].next == EMPTY) return 0;
	do {
		if (dict->_equal (&key, dict_key_ptr(dict, pos))) return 1;
		pos = dict->entries[pos].next;
	} while (pos != END);
	return 0;
}

const void *
arikkei_dict_lookup (ArikkeiDict *dict, const void *key)
{
	unsigned int pos = dict->_hash(&key) % dict->root_size;
	if (dict->entries[pos].next == EMPTY) return NULL;
	do {
		if (dict->_equal (&key, dict_key_ptr(dict, pos))) return &dict->entries[pos].val;
		pos = dict->entries[pos].next;
	} while (pos != END);
	return NULL;
}

const void *
arikkei_dict_lookup_pval (ArikkeiDict *dict, const void *key)
{
	unsigned int pos = dict->_hash(&key) % dict->root_size;
	if (dict->entries[pos].next == EMPTY) return NULL;
	do {
		if (dict->_equal (&key, dict_key_ptr(dict, pos))) return dict->entries[pos].val;
		pos = dict->entries[pos].next;
	} while (pos != END);
	return NULL;
}

const void *
arikkei_dict_lookup_foreign (ArikkeiDict *dict, const void *key, unsigned int (*hash) (const void *key), unsigned int (*equal) (const void *lhs, const void *rhs))
{
	unsigned int hval, pos;
	if (!key) return NULL;
	hval = hash(key) % dict->root_size;
	pos = hval;
	if (dict->entries[pos].next == EMPTY) return NULL;
	do {
		const void *k, *v;
		memcpy(&k, dict_key_ptr(dict, pos), 8);
		memcpy(&v, dict_val_ptr(dict, pos), 8);
		if (equal (key, k)) return v;
		pos = dict->entries[pos].next;
	} while (pos != END);
	return NULL;
}

unsigned int
arikkei_dict_forall (ArikkeiDict *dict, unsigned int (* forall) (const void *, const void *, void *), void *data)
{
	unsigned int i, pos;
	for (i = 0; i < dict->root_size; i++) {
		if (ENTRY_IS_EMPTY(&dict->entries[i])) continue;
		pos = i;
		do {
			if (!forall (dict->entries[pos].key, dict->entries[pos].val, data)) return 0;
			pos = dict->entries[pos].next;
		} while (pos != END);
	}
	return 1;
}

unsigned int
arikkei_dict_remove_all (ArikkeiDict *dict, unsigned int (*remove) (const void *, const void *, void *), void *data)
{
	unsigned int hash, n_removed;
	n_removed = 0;
	for (hash = 0; hash < dict->root_size; hash++) {
		if (ENTRY_IS_EMPTY(&dict->entries[hash])) continue;
		if (remove (dict->entries[hash].key, dict->entries[hash].val, data)) {
			/* Remove root entry */
			if (dict->entries[hash].next != END) {
				int pos;
				pos = dict->entries[hash].next;
				dict->entries[hash] = dict->entries[pos];
				dict->entries[pos].next = dict->free;
				dict->free = pos;
			} else {
				dict->entries[hash].next = EMPTY;
			}
			n_removed += 1;
		} else {
			int pos, prev;
			prev = hash;
			pos = dict->entries[hash].next;
			while (pos) {
				if (remove (dict->entries[pos].key, dict->entries[pos].val, data)) {
					dict->entries[prev].next = dict->entries[pos].next;
					dict->entries[pos].next = dict->free;
					dict->free = pos;
					n_removed += 1;
				} else {
					prev = pos;
					pos = dict->entries[pos].next;
				}
			}

		}
	}
	return n_removed;
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

unsigned int
arikkei_memory_hash (const void *data, unsigned int size)
{
	const uint8_t *p = *((const uint8_t **) data);
	unsigned int hval = *p;
	for (unsigned int i = 1; i < size; i++) {
		hval = (hval << 5) - hval + p[i];
	}
	return hval;
}

unsigned int
arikkei_memory_equal (const void *l, const void *r, unsigned int size)
{
	return !memcmp (l, r, size);
}


