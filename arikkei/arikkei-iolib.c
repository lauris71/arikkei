#define __ARIKKEI_IOLIB_C__

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

// Disable VS2005 nagging
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#endif

#include <sys/timeb.h>
#ifndef _WIN32
#include <sys/time.h>
#include <time.h>
#endif

#include "arikkei-strlib.h"
#include "arikkei-dict.h"

#include "arikkei-iolib.h"

#ifdef _DEBUG
//#define PRINT_MAPSIZE 1
#endif

#ifdef PRINT_MAPSIZE
static size_t total = 0;
#endif

#ifdef _WIN32

const unsigned char *
arikkei_mmap (const unsigned char *filename, uint64_t *size)
{
	unsigned short *ucs2filename;
	unsigned char *cdata;
	struct _stat st;
	HANDLE fh, mh;

	if (!filename || !*filename) return NULL;

	ucs2filename = arikkei_utf8_ucs2_strdup (filename);

	if (_wstat (ucs2filename, &st)) {
		/* No such file */
		/* fprintf (stderr, "arikkei_mmap: File %s not found or not regular file\n", filename); */
		free (ucs2filename);
		return NULL;
	}

	fh = CreateFile (ucs2filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fh == INVALID_HANDLE_VALUE) {
		/* Cannot open */
		/* fprintf (stderr, "arikkei_mmap: File %s cannot be opened for reading\n", filename); */
		free (ucs2filename);
		return NULL;
	}

	mh = CreateFileMapping (fh, NULL, PAGE_READONLY, 0, 0, NULL);
	if (mh == NULL) {
		/* Mapping failed */
		/* fprintf (stderr, "arikkei_mmap: File %s cannot be mapped as %s\n", filename, mapname); */
		DWORD ecode = GetLastError ();
		fprintf (stderr, "arikkei_mmap: File %s cannot be mapped as %s (Error %d)\n", filename, mapname, ecode);
		CloseHandle (fh);
		free (ucs2filename);
		return NULL;
	}
	/* Get a pointer to the file-mapped shared memory. */
	cdata = (unsigned char *) MapViewOfFile (mh, FILE_MAP_READ, 0, 0, 0);

#ifdef PRINT_MAPSIZE
	if (!cdata) {
		DWORD ecode = GetLastError ();
		fprintf (stderr, "arikkei_mmap: Error %d\n", ecode);
	} else {
		total += st.st_size;
		fprintf (stderr, "MMap size+: %x\n", (unsigned int) total);
	}
#endif
	CloseHandle (mh);
	CloseHandle (fh);

	free (ucs2filename);

	*size = st.st_size;

	return cdata;
}

void
arikkei_munmap (const unsigned char *cdata, size_t size)
{
	/* Release data */
	UnmapViewOfFile (cdata);

#ifdef PRINT_MAPSIZE
	total -= size;
	fprintf (stderr, "MMap size-: %x\n", (unsigned int) total);
#endif
}

#else

const uint8_t *
arikkei_mmap (const uint8_t *file_name, uint64_t *map_size)
{
	unsigned char *cdata;
	struct stat st;
	cdata = NULL;
	if (!stat ((const char *) file_name, &st) && S_ISREG (st.st_mode) && (st.st_size > 8)) {
		int fd;
		fd = open ((const char *) file_name, O_RDONLY);
		if (fd < 0) return NULL;
		cdata = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
		close (fd);
		if ((!cdata) || (cdata == (unsigned char *) -1)) return NULL;
	}

	*map_size = st.st_size;

	return cdata;
}

void
arikkei_munmap (const uint8_t *map, uint64_t map_size)
{
	munmap ((void *) map, map_size);

}

#endif

FILE *
arikkei_fopen (const uint8_t *file_name, const uint8_t *mode)
{
#ifdef _WIN32
	unsigned short *ucs2filename, *ucs2mode;
	FILE *fs;
	if (!file_name || !*file_name) return NULL;
	ucs2filename = arikkei_utf8_ucs2_strdup (file_name);
	ucs2mode = arikkei_utf8_ucs2_strdup (mode);
	fs = _wfopen (ucs2filename, ucs2mode);
	free (ucs2filename);
	free (ucs2mode);
	return fs;
#else
	return fopen ((const char *) file_name, (const char *) mode);
#endif
}

#ifdef _WIN32
double
arikkei_get_time (void)
{
	struct _timeb t;
	_ftime (&t);
	double dtval = (t.time + t.millitm / 1000.0);
	return dtval;
}
#elif defined (_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
double
arikkei_get_time (void)
{
	struct timespec tspec;
	clock_gettime(CLOCK_REALTIME, &tspec);
	double dtval = (tspec.tv_sec + tspec.tv_nsec / 1000000000.0);
	return dtval;
}
#else
double
arikkei_get_time (void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	double dtval = (tv.tv_sec + tv.tv_usec / 1000000.0);
	return dtval;
}
#endif
