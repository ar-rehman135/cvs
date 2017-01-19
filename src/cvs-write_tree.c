/*	This file contains code to write tree structure to the Data base.
**	Tree structure is just like blob structure but it contains information
**	about files currently present in tree as a content.
**	The Database which is our subdirecoty in .dircache directory contains
**	the files with named SHA. The code contain in this file make a command
**	"cvs-write_tree". The function of this command is to take all the 
**	structures of files contained in cache file which is index file in
**	directory .dircache. this simply takes all the structures read their SHA
**	and make a new tree structure whose content is files structures contained
**	in index file.
*/





#include "cache.h"

/*	check for SHA is Valid or not. This is done so 
**	by just looking into DB if the file with given 
**	SHA exist or not.
*/
int commit =0;
static int check_valid_sha1(unsigned char *sha1)
{
	/* sha1_file_name function return the relative path to file with given SHA
	*/
	char *filename = sha1_file_name(sha1);  
	int ret;
	/*  We just see do we access the file to see if it is there or not  */
	ret = access(filename, R_OK);
	if (ret)
		perror(filename);
	return ret;
}

/*	this is helper function which is used to prepend the integer to the given buffer 
**	given the integer val which we want to prepend and integer i the offset.
*/
static int prepend_integer(char *buffer, unsigned val, int i)
{
	buffer[--i] = '\0';
	do {
		buffer[--i] = '0' + (val % 10);
		val /= 10;
	} while (val);
	return i;
}


//the estimated size of tree header
#define ORIG_OFFSET (40)

// this is a main functions calls when we type the cvs-write_tree command
int main(int argc, char **argv)
{
	unsigned long size, offset, val;
	
	/*	rading cache means read all blob entries present in index files 
	**	this done through read_cache() function defined in cvs-read.c
	**	return the integer which shows the number of entries present.
	*/
	int i, entries = read_cache();
	char *buffer;

	if (entries <= 0) {
		fprintf(stderr, "No file-cache to create a tree of\n");
		exit(1);
	}

	
	size = entries * 40 + 400;
	buffer = malloc(size);
	offset = ORIG_OFFSET;

	for (i = 0; i < entries; i++) {
		/*	acrive_cache is a global 2d array populated by read_cache() */
		struct cache_entry *ce = active_cache[i];
		if (check_valid_sha1(ce->sha1) < 0)
			exit(1);
		if (offset + ce->namelen + 60 > size) {
			size = alloc_nr(offset + ce->namelen + 60);
			buffer = realloc(buffer, size);
		}
		/*	populatng contents of tree */
		offset += sprintf(buffer + offset, "%o %s", ce->st_mode, ce->name);
		buffer[offset++] = 0;
		memcpy(buffer + offset, ce->sha1, 20);
		offset += 20;
	}
	/*	here we are prepending the the size of content in the end of first 40 bytes*/
	i = prepend_integer(buffer, offset - ORIG_OFFSET, ORIG_OFFSET);
	i -= 5;
	/*now prepending the "tree" s that we cn know that this file is tree  */
	memcpy(buffer+i, "tree ", 5);

	buffer += i;
	offset -= i;
	
	/*	now writing the tree structure to file 	*/
	write_sha1_file(buffer, offset);
	return 0;
}
/*	At the end the file with tree structure is like	
**	tree SIZE_OF_CONTENT content...
*/


