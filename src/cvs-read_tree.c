/*	This file contins the code for reading database objects of type tree
**	this file make up the command cvs-read_tree which takes a SHA of tree as input
**	and read the object database of that SHA if it tree object it is displayed.
*/


#include "cache.h"
//	This is a helper functon which is used to unpack the tree struct
static int unpack(unsigned char *sha1)
{
	void *buffer;
	unsigned long size;
	char type[20];
	/*	here we read an object file from .dircache/objects/ directory
	**	read_sha1_file() function defined in cvs-read.c is used to read
	**	it returns the pointer to data present in memeory
	*/
	buffer = read_sha1_file(sha1, type, &size);
	// checking file readed or not
	if (!buffer)
		usage("unable to read sha1 file");
	//checking tree type
	if (strcmp(type, "tree"))
		usage("expected a 'tree' node");
	//reading entries of file from buffer
	while (size) {
		int len = strlen(buffer)+1;
		unsigned char *sha1 = buffer + len;
		char *path = strchr(buffer, ' ')+1;
		unsigned int mode;
		if (size < len + 20 || sscanf(buffer, "%o", &mode) != 1)
			usage("corrupt 'tree' file");
		buffer = sha1 + 20;
		size -= len + 20;
		printf("%o %s (%s)\n", mode, path, sha1_to_hex(sha1));
	}
	return 0;
}
//	this is main function which is called when we run command cvs-read_tree <sha>
int main(int argc, char **argv)
{
	int fd;
	unsigned char sha1[20];

	if (argc != 2)
		usage("read-tree <key>");
	if (get_sha1_hex(argv[1], sha1) < 0)
		usage("read-tree <key>");
	sha1_file_directory = getenv(DB_ENVIRONMENT);
	if (!sha1_file_directory)
		sha1_file_directory = DEFAULT_DB_ENVIRONMENT;
	if (unpack(sha1) < 0)
		usage("unpack failed");
	return 0;
}
