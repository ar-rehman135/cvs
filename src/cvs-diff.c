#include "cache.h"

#define MTIME_CHANGED	0x0001
#define CTIME_CHANGED	0x0002
#define OWNER_CHANGED	0x0004
#define MODE_CHANGED    0x0008
#define INODE_CHANGED   0x0010
#define DATA_CHANGED    0x0020


/*
 * We provide two files to this function, one which is in our 
 * cache and one in our current working directory and checks if 
 * any changes occured in them. 
 *
 * We have declared some macros and if there are some changes we 
 * populate our "changed" variable accordingly and return it. 
 */



static int match_stat(struct cache_entry *ce, struct stat *st)
{
	unsigned int changed = 0;

	if (ce->mtime.sec  != (unsigned int)st->st_mtim.tv_sec ||
	    ce->mtime.nsec != (unsigned int)st->st_mtim.tv_nsec)
		changed |= MTIME_CHANGED;
	if (ce->ctime.sec  != (unsigned int)st->st_ctim.tv_sec ||
	    ce->ctime.nsec != (unsigned int)st->st_ctim.tv_nsec)
		changed |= CTIME_CHANGED;
	if (ce->st_uid != (unsigned int)st->st_uid ||
	    ce->st_gid != (unsigned int)st->st_gid)
		changed |= OWNER_CHANGED;
	if (ce->st_mode != (unsigned int)st->st_mode)
		changed |= MODE_CHANGED;
	if (ce->st_dev != (unsigned int)st->st_dev ||
	    ce->st_ino != (unsigned int)st->st_ino)
		changed |= INODE_CHANGED;
	if (ce->st_size != (unsigned int)st->st_size)
		changed |= DATA_CHANGED;
	return changed;
}


/*
 * The basic functionality of this function is to run diff
 * command and populate the buffer with differences between 
 * the given files.
 */

static void show_differences(struct cache_entry *ce, struct stat *cur,
	void *old_contents, unsigned long long old_size)
{
	static char cmd[1000];
	FILE *f;

	snprintf(cmd, sizeof(cmd), "diff -u - %s", ce->name);
	f = popen(cmd, "w");
	fwrite(old_contents, old_size, 1, f);
	pclose(f);
}


/*
 * In this function we check the difference between the files in 
 * cache and files in our current working directory. 
 * 
 * We read our cache and also reads the stats of our file with the 
 * name in our current working directory. Then we match their meta data
 * to check if anything has changed. 
 *
 * Then we get the SHA of the respective file and read its content and
 * pass the data to the "show difference" function. 
 */




int main(int argc, char **argv)
{
	int entries = read_cache();
	int i;

	if (entries < 0) {
		perror("read_cache");
		exit(1);
	}
	for (i = 0; i < entries; i++) {
		struct stat st;
		struct cache_entry *ce = active_cache[i];
		int n, changed;
		unsigned int mode;
		unsigned long size;
		char type[20];
		void *new;

		if (stat(ce->name, &st) < 0) {
			printf("%s: %s\n", ce->name, strerror(errno));
			continue;
		}
		changed = match_stat(ce, &st);
		if (!changed) {
			printf("%s: ok\n", ce->name);
			continue;
		}
		printf("%.*s:  ", ce->namelen, ce->name);
		for (n = 0; n < 20; n++)
			printf("%02x", ce->sha1[n]);
		printf("\n");
		new = read_sha1_file(ce->sha1, type, &size);
		show_differences(ce, &st, new, size);
		free(new);
	}
	return 0;
}
