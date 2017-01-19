#include "cache.h"


/*
 * This function just takes four arguments and compare their 
 * name lengths to check if they can be same enteries or not. 
 */

static int cache_name_compare(const char *name1, int len1, const char *name2, int len2)
{
	int len = len1 < len2 ? len1 : len2;
	int cmp;

	cmp = memcmp(name1, name2, len);
	if (cmp)
		return cmp;
	if (len1 < len2)
		return -1;
	if (len1 > len2)
		return 1;
	return 0;
}

/*
 * This function just take the name of the file and its name length.
 * 
 * We declare some variables and of integer type name "first" and "last"
 * and we initialize the "last" variable with the total number of entries 
 * present in our cache. "active_nr" is a global variable created in "include.h" 
 * it contains total number of entries present in our cache/index file. 
 * 
 * Then we compare the name of each entry to check if such entry already exist 
 * in our cache or not, if it does we will update it and will not add a new entry,
 * if there is none then we will add a new entry in our index/cache. For this purpose
 * we compare the name of our given file with all the files in our cache. We select 
 * the files from our cache heuristicly. 
 *
 * If file is found we return its location in our cache and if not then we return  
 * our last location of cache where we can entertain a new entry.
 */


static int cache_name_pos(const char *name, int namelen)
{
	int first, last;

	first = 0;
	last = active_nr;
	while (last > first) {
		int next = (last + first) >> 1;
		struct cache_entry *ce = active_cache[next];
		int cmp = cache_name_compare(name, namelen, ce->name, ce->namelen);
		if (!cmp)
			return -next-1;
		if (cmp < 0) {
			last = next;
			continue;
		}
		first = next+1;
	}
	return first;
}

/*
 * If a file has been deleted from our working copy then we do not need its entry 
 * in our cache if there is any.
 *
 * This function solely works for this purpose. 
 *
 * First of all we find the position of that entry which is to be removed
 * and if it exist we just call the "memmove" (builtin function) to overwrite
 * the data present next to our desired cache entry onto it. 
 */

static int remove_file_from_cache(char *path)
{
	int pos = cache_name_pos(path, strlen(path));
	if (pos < 0) {
		pos = -pos-1;
		active_nr--;
		if (pos < active_nr)
			memmove(active_cache + pos, active_cache + pos + 1, (active_nr - pos - 1) * sizeof(struct cache_entry *));
	}
}

/*
 * This function takes the populated struct of cache entry. 
 * We know that each file have its own cache entry struct 
 * populated according to its own meta data and data.  
 *
 * This function checks if there is already and entry of the 
 * desired file into cache, if yes we simply replace it with 
 * our newly populated information. 
 * 
 * If it does not exist then we realloc our dynamic 2D array
 * of "active_cache" which is declared in "include.h" and make
 * the entry on the position returned by "cache_name_pos" function.
 *
 * We know that "cache_name_pos" calculate the position heuristicly 
 * so if active_nr (total number of entries) is greater then our returned 
 * position then we move some data back and forth to make room for our 
 * new data on the returned position. 
 */ 


static int add_cache_entry(struct cache_entry *ce)
{
	int pos;

	pos = cache_name_pos(ce->name, ce->namelen);

	/* existing match? Just replace it */
	if (pos < 0) {
		active_cache[-pos-1] = ce;
		return 0;
	}

	/* Make sure the array is big enough .. */
	if (active_nr == active_alloc) {
		active_alloc = alloc_nr(active_alloc);
		active_cache = realloc(active_cache, active_alloc * sizeof(struct cache_entry *));
	}

	/* Add it in.. */
	active_nr++;
	if (active_nr > pos)
		memmove(active_cache + pos + 1, active_cache + pos, (active_nr - pos - 1) * sizeof(ce));
	active_cache[pos] = ce;
	return 0;
}

/*
 * The main purpose of this function is just to write compressed 
 * data into SHA named file. 
 * 
 * In order to understand this function it is better to man "mmap"
 * and SHA related functions. In this function we just created 
 * some pointers and declares some dynamic memory. 
 *
 * Then we read all the data from file and write it into memory
 * using "mmap" then we use "deflateInit" function to initiate our 
 * stream then we write our compressed meta data on stream and after
 * that the content of our file. 
 *
 * Now we have meta data, data of file and everything on our stream so
 * now we can create SHA for our file and using SHA functions. 
 *
 * Then we call "write_sha1_buffer" function whose decleration is given 
 * in "read.c". This function creates a file with our SHA name is the 
 * respective directory of object database and write the compressed 
 * content of the original file into our newly created SHA file. 
 */



static int index_fd(const char *path, int namelen, struct cache_entry *ce, int fd, struct stat *st)
{
	z_stream stream;
	int max_out_bytes = namelen + st->st_size + 200;
	void *out = malloc(max_out_bytes);
	void *metadata = malloc(namelen + 200);
	void *in = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	SHA_CTX c;

	close(fd);
	if (!out || (int)(long)in == -1)
		return -1;

	memset(&stream, 0, sizeof(stream));
	deflateInit(&stream, Z_BEST_COMPRESSION);

	/*
	 * ASCII size + nul byte
	 */	
	stream.next_in = metadata;
	stream.avail_in = 1+sprintf(metadata, "blob %lu", (unsigned long) st->st_size);
	stream.next_out = out;
	stream.avail_out = max_out_bytes;
	while (deflate(&stream, 0) == Z_OK)
		/* nothing */;

	/*
	 * File content
	 */
	stream.next_in = in;
	stream.avail_in = st->st_size;
	while (deflate(&stream, Z_FINISH) == Z_OK)
		/*nothing */;

	deflateEnd(&stream);
	
	SHA1_Init(&c);
	SHA1_Update(&c, out, stream.total_out);
	SHA1_Final(ce->sha1, &c);

	return write_sha1_buffer(ce->sha1, out, stream.total_out);
}


/*
 * This function just take the path/name of the file 
 * which it to be added in the cache as an argument.
 * 
 * It declares a variable of "cache_entry" struct which 
 * is given in our "include.h" file. Another struct of 
 * stat type is declared to get the meta-data of file. 
 *
 * First of all there is some error checking to check if
 * the file is present or not, if file is not present then
 * it means it have been removed form our working copy so 
 * we call "remove_file_from_cache()" so that we can remove
 * that file from our cache if it already exists in it 
 * and then the stat struct variable is populated.
 * 
 * Then we calculate the name length of the given file
 * and calculate the size for cache entry on the basis
 * of name length, that entry is to be added in our cache.
 *
 * Then we declare some dynamic memory so that we can add some
 * data into our original memory. 
 *
 * All the struct's variables are populated and then we compress
 * our data and for this purpose we use "index_fd" function.
 * 
 * Now we have created an entry and we can add it to cache so we 
 * call "add_cache_entry" so that entry gets added into cache 
 */


static int add_file_to_cache(char *path)
{
	int size, namelen;
	struct cache_entry *ce;
	struct stat st;
	int fd;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT)
			return remove_file_from_cache(path);
		return -1;
	}
	if (fstat(fd, &st) < 0) {
		close(fd);
		return -1;
	}
	namelen = strlen(path);
	size = cache_entry_size(namelen);
	ce = malloc(size);
	memset(ce, 0, size);
	memcpy(ce->name, path, namelen);
	ce->ctime.sec = st.st_ctime;
	ce->ctime.nsec = st.st_ctim.tv_nsec;
	ce->mtime.sec = st.st_mtime;
	ce->mtime.nsec = st.st_mtim.tv_nsec;
	ce->st_dev = st.st_dev;
	ce->st_ino = st.st_ino;
	ce->st_mode = st.st_mode;
	ce->st_uid = st.st_uid;
	ce->st_gid = st.st_gid;
	ce->st_size = st.st_size;
	ce->namelen = namelen;

	if (index_fd(path, namelen, ce, fd, &st) < 0)
		return -1;

	return add_cache_entry(ce);
}


/*
 * The main purpose of this function is to write the new entry into cache.
 *
 * It declares "cache_header" struct (given in include.h) variable and  
 * populate it and after that it recalculate the SHA of all entries SHAs
 * and then it writes all the entries into our cache file.
 */

static int write_cache(int newfd, struct cache_entry **cache, int entries)
{
	SHA_CTX c;
	struct cache_header hdr;
	int i;

	hdr.signature = CACHE_SIGNATURE;
	hdr.version = 1;
	hdr.entries = entries;

	SHA1_Init(&c);
	SHA1_Update(&c, &hdr, offsetof(struct cache_header, sha1));
	for (i = 0; i < entries; i++) {
		struct cache_entry *ce = cache[i];
		int size = ce_size(ce);
		SHA1_Update(&c, ce, size);
	}
	SHA1_Final(hdr.sha1, &c);

	if (write(newfd, &hdr, sizeof(hdr)) != sizeof(hdr))
		return -1;

	for (i = 0; i < entries; i++) {
		struct cache_entry *ce = cache[i];
		int size = ce_size(ce);
		if (write(newfd, ce, size) != size)
			return -1;
	}
	return 0;
}		

/*
 * We fundamentally don't like some paths: we don't want
 * dot or dot-dot anywhere, and in fact, we don't even want
 * any other dot-files (.dircache or anything else). They
 * are hidden.
 *
 * Also, we don't want double slashes or slashes at the
 * end that can make pathnames ambiguous. 
 */
static int verify_path(char *path)
{
	char c;

	goto inside;
	for (;;) {
		if (!c)
			return 1;
		if (c == '/') {
inside:
			c = *path++;
			if (c != '/' && c != '.' && c != '\0')
				continue;
			return 0;
		}
		c = *path++;
	}
}

/*
 * Takes names of the file in the command line arguments 
 * which are to be added the in cache/stagging area.  
 *
 * First of all it tries to read the cache to check if there 
 * are some enteries in it so that it can enter the
 * entry of the new file after the exsisting one.
 *
 * There are some conditions to check the validity of cache 
 * and if it is not corrupted then we open that file 
 * change its name from "index" to "index.lock" so that no other 
 * function (if any) can try to access it while an entry is being 
 * made into it and then make the entry of our new file. 
 *
 * But first we verfiy the name of our file to be added to 
 * check if there is any illegal character.  
 *
 * Then we create an entry to be added into our cache by 
 * "add_file_to_cache()" function. 
 *
 * The files/newly created entries are then added into the staging 
 * area through "write-cache" function.
 */

int main(int argc, char **argv)
{
	int i, newfd, entries;

	entries = read_cache();
	if (entries < 0) {
		perror("cache corrupted");
		return -1;
	}

	newfd = open(".dircache/index.lock", O_RDWR | O_CREAT | O_EXCL, 0600);
	if (newfd < 0) {
		perror("unable to create new cachefile");
		return -1;
	}
	for (i = 1 ; i < argc; i++) {
		char *path = argv[i];
		if (!verify_path(path)) {
			fprintf(stderr, "Ignoring path %s\n", argv[i]);
			continue;
		}
		if (add_file_to_cache(path)) {
			fprintf(stderr, "Unable to add %s to database\n", path);
			goto out;
		}
	}
	if (!write_cache(newfd, active_cache, active_nr) && !rename(".dircache/index.lock", ".dircache/index"))
		return 0;
out:
	unlink(".dircache/index.lock");
}
