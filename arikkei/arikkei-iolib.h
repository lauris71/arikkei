#ifndef __ARIKKEI_IOLIB_H__
#define __ARIKKEI_IOLIB_H__

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
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* All file names are UTF-8 */
const uint8_t *arikkei_mmap (const uint8_t *file_name, uint64_t *map_size);
void arikkei_munmap (const uint8_t *map, uint64_t map_size);
/* Frontend to open file with utf-8 name (only really needed for windows) */
FILE *arikkei_fopen (const uint8_t *file_name, const uint8_t *mode);

/* Get current wall time in seconds as double with maximum supported precision */
double arikkei_get_time (void);

#ifdef __cplusplus
}
#endif

#endif
