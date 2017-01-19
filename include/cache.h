#ifndef CACHE_H
#define CACHE_H

/*	this file include the some declarations of functions which are there
**	on the cvs-read.o file and there are some structure declared in this 
**	file which are used in all commands corresponding c files. there are 
**	variables declared here which are used in different files.
*/
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/mman.h>

#include <openssl/sha.h>
#include <zlib.h>

/*	this is a default signature of the cache header structure this is placed 
**	in cache header for checking the corruption.
*/
#define CACHE_SIGNATURE 0x44495243

/*	this is a cache header structure which is placed at the starting 
**	of index file to have some meta data of entries in index file 
*/
struct cache_header {
	unsigned int signature;
	unsigned int version;
	//	count of entries
	unsigned int entries;
	//	contains SHA of Entries
	unsigned char sha1[20];
};

/*	this structure saves the time stamp of file 
*/
struct cache_time {
	unsigned int sec;
	unsigned int nsec;
};

/*	this structure is used for maintaining file meta data in index file 
**	which is stagging area to contain files.
*/
struct cache_entry {
	struct cache_time ctime;
	struct cache_time mtime;
	unsigned int st_dev;
	unsigned int st_ino;
	unsigned int st_mode;
	unsigned int st_uid;
	unsigned int st_gid;
	unsigned int st_size;
	unsigned char sha1[20];
	unsigned short namelen;

	unsigned char name[0];
};

/*	this variable is used for containing path for our objects database
*/
const char *sha1_file_directory;
int write_commit;

/*	this is global variable. it is double pointer to cache entry for
**	maintaining the memory representation of staging area.
*/
struct cache_entry **active_cache;
//	these variables are used to keep track of actice cache total entries 
//	and space allocated to actice cache
unsigned int active_nr, active_alloc;

//	here we can get DB_ENVIRONEMT which path to our objects database
//	we can give it by adding a environment variable with name
//	SHA1_FILE_DIRECTORY and give path on it 
#define DB_ENVIRONMENT "SHA1_FILE_DIRECTORY"
//	by default our DB_ENVIRONMENT is .dircache/objects directory
#define DEFAULT_DB_ENVIRONMENT ".dircache/objects"
#define DEFAULT_HEADS_DIR ".dircache/HEADS"
//	cache_entry_size(len) is a macro which return the size of cache entry 
//	cache entry size is used to find size of cache entry given cache entry object
#define cache_entry_size(len) ((offsetof(struct cache_entry,name) + (len) + 8) & ~7)
#define ce_size(ce) cache_entry_size((ce)->namelen)

/*	alloc_nr returns the updated entries size as a estimate
*/
#define alloc_nr(x) (((x)+16)*3/2)

//	these functions are defined in cvs-read.c which is used to read cache which is 
//	our stagging area and used in different commands

extern int read_cache(void);

extern char *sha1_file_name(unsigned char *sha1);

extern int write_sha1_buffer(unsigned char *sha1, void *buf, unsigned int size);

extern void * read_sha1_file(unsigned char *sha1, char *type, unsigned long *size);
extern int write_sha1_file(char *buf, unsigned len);

extern int get_sha1_hex(char *hex, unsigned char *sha1);
extern char *sha1_to_hex(unsigned char *sha1);	/* static buffer! */

extern void usage(const char *err);

#endif /* CACHE_H */
