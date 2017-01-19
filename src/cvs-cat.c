#include "cache.h"

/*
* cvs-cat command takes one command line argument 
* That argument is SHA. And it converts this SHA to hex 
* After converting it to hex it reads the file containing SHA's
* and tell you type of SHA i.e., type commit and blob are dealt here.
* It shows you type of sha and then show content of that sha
*/


int main( int argc , char **argv )
{
	unsigned char sha1[20];
	char type[20];
	void *buf;
	unsigned long size;

	if (argc != 2 || get_sha1_hex(argv[1], sha1))
		usage("cat-file: cat-file <sha1>");
	
		buf = read_sha1_file(sha1, type, &size);
	
	if (!buf)
		exit(1);
	
	printf("%s\n", type);

	if (write(1, buf, size) != size)
		strcpy(type, "bad");

}
