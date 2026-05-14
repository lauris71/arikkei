#ifndef __ARIKKEI_DICT_H__
#define __ARIKKEI_DICT_H__

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

 #include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ArikkeiDict ArikkeiDict;
typedef struct _ArikkeiDictEntry ArikkeiDictEntry;

struct _ArikkeiDict {
	unsigned int root_size;
	unsigned int size;
	unsigned int entry_size;
	uint16_t key_offset;
	uint16_t key_size;
	uint16_t val_offset;
	uint16_t val_size;
	ArikkeiDictEntry *entries;
	unsigned int free;
	unsigned int (*hash) (const void *key);
	/**
	 * @brief Compare two keys for equality
	 * 
	 * @param lhs left hand side key
	 * @param rhs right hand side key
	 * @return 1 if keys are equal, 0 otherwise
	 */
	unsigned int (*equal) (const void *lhs, const void *rhs);
	void (*copy_key) (void *dst, const void *src);
	void (*free_key) (void *key);
	void (*copy_value) (void *dst, const void *src);
	void (*free_value) (void *value);
};

void arikkei_dict_setup_full (ArikkeiDict *dict, unsigned int hashsize,
			      unsigned int (*hash) (const void *key),
			      unsigned int (*equal) (const void *lhs, const void *rhs));
void arikkei_dict_setup_sizes (ArikkeiDict *dict, unsigned int hashsize,
			       unsigned int key_size, unsigned int val_size,
			       unsigned int (*hash) (const void *key),
			       unsigned int (*equal) (const void *lhs, const void *rhs));
void arikkei_dict_release (ArikkeiDict *dict);

/**
 * @brief Insert a value into hash table
 * 
 * Keep in mind that it is the the dereferenced key that is copied into the hash table. Thus, if the
 * hash table should associate const char * with a value, the key should be a pointer to a const char *
 * (i.e. const char **).
 * A similar consideration applies to the value. It is the dereferenced value that is copied into the hash table.
 * The copy_key and copy_value functions are called (if present), otherwise memcpy is used to transfer key_size
 * and value_size bytes.
 * 
 * @param dict The hash table
 * @param key a pointer to the key
 * @param val a pointer to the value
 */
void arikkei_dict_insert(ArikkeiDict *dict, const void *key, const void *val);
/**
 * @brief Remove a key/value pair from the hash table
 * 
 * @param dict The hash table
 * @param key a pointer to the key
 * @return 1 if pair was removed, 0 if there was no such key
 */
unsigned int arikkei_dict_remove(ArikkeiDict *dict, const void *key);

static inline void
arikkei_dict_insert_pval (ArikkeiDict *dict, const void *key, const void *val)
{
	arikkei_dict_insert(dict, &key, &val);
}

static inline unsigned int
arikkei_dict_remove_pval (ArikkeiDict *dict, const void *key)
{
	return arikkei_dict_remove(dict, &key);
}

void arikkei_dict_clear(ArikkeiDict *dict);
unsigned int arikkei_dict_exists(ArikkeiDict *dict, const void *key);
unsigned int arikkei_dict_exists_pval(ArikkeiDict *dict, const void *key);
const void *arikkei_dict_lookup(ArikkeiDict *dict, const void *key);
const void *arikkei_dict_lookup_pval(ArikkeiDict *dict, const void *key);
/*
 * Lookup using foreign key with hash and equal provided by caller (lhs is foreign)
 * This is, e.g. needed to look up null-terminated string hasb by buffer/len pair or
 * if key is based on several fields of value
 */
const void *arikkei_dict_lookup_foreign (ArikkeiDict *dict, const void *foreign_key, unsigned int hash, unsigned int (*equal) (const void *foreign_key, const void *dict_key));
/* Stop if forall returns 0 */
unsigned int arikkei_dict_forall (ArikkeiDict *dict, unsigned int (* forall) (const void *, const void *, void *), void *data);
/* Remove entry if remove returns 1, return number of entries removed */
unsigned int arikkei_dict_remove_all (ArikkeiDict *dict, unsigned int (*remove) (const void *, const void *, void *), void *data);

void arikkei_dict_get_stats (ArikkeiDict *dict, unsigned int *n_entries, unsigned int *n_root, unsigned int *longest_chain, unsigned int *n_free);

/* Utility methods */
uint32_t arikkei_string_hash (const void *data);
unsigned int arikkei_string_equal (const void *l, const void *r);
unsigned int arikkei_pointer_hash (const void *data);
unsigned int arikkei_pointer_equal (const void *l, const void *r);
unsigned int arikkei_int32_hash (const void *data);
unsigned int arikkei_int32_equal (const void *l, const void *r);
unsigned int arikkei_int64_hash (const void *data);
unsigned int arikkei_int64_equal (const void *l, const void *r);

uint32_t arikkei_memory_hash (const void *data, unsigned int size);
unsigned int arikkei_memory_equal (const void *l, const void *r, unsigned int size);

static inline void
arikkei_dict_setup_string (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_string_hash, arikkei_string_equal);
}

static inline void
arikkei_dict_setup_pointer (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_pointer_hash, arikkei_pointer_equal);
}

static inline void
arikkei_dict_setup_int32 (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_int32_hash, arikkei_int32_equal);
}

static inline void
arikkei_dict_setup_int64 (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_int64_hash, arikkei_int64_equal);
}

#ifdef __cplusplus
}
#endif

#endif
