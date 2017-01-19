#include "cache.h"
#define KBLU  "\x1B[34m"
#define KNRM  "\x1B[0m"
/*
** 	cvs-log command display all the commits history with their 
** 	user and commiter information. It reads all the SHA of commits 
** 	from file .commit_hash.txt in in .dircache and reads the 
** 	contents one by one and display it with arrangement from recent
** 	to oldest.
*/


int main( int argc , char **argv )
{
	int fd;
	void *buf;
	char type[20];
	unsigned long size;
	fd=open(".dircache/.commit_hash.txt",O_RDONLY);
	if(fd<0)
		printf("No Log to Show");
	char sha[41];
	char sha1[20];
	int offset = lseek(fd,0,SEEK_END);
	while( offset != 0)
	{
		offset=lseek(fd,-40,SEEK_CUR);
		read(fd,sha,40);
		sha[40]='\0';
		if (get_sha1_hex(sha, sha1))
			usage("invalid sha");
		buf = read_sha1_file(sha1, type, &size);
		if (!buf)
			continue;
		printf("\n%s%s %s%s\n",KBLU,type,sha,KNRM);
		if (write(1, buf, size) != size)
			strcpy(type, "bad");
		offset = lseek(fd,-40,SEEK_CUR);
	}
	return 0;
}
