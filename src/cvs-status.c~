#include "cache.h"


int main()
{

	unsigned char sha1[41];
	int fd = open(".dircache/HEADS/master",O_RDONLY);
	if(fd<0)
		perror("ERROR: ");
	int k=read(fd,sha1,40);
	sha1[40]='\0';
	printf("We are at branch master\nThe Most recent commit SHA is \n\t%s\n",sha1);
	return 0;
}

