#include "cache.h" //including the header we defined for macros and data structures,

int main(int argc, char **argv)
{
	char *sha1_dir = getenv(DB_ENVIRONMENT), *path; // getting enviornment of the current directory
	int len, i, fd;					// declarations of variables for having meta dat of directory name and all.

	if (mkdir(".dircache", 0700) < 0) {		// finally making a directory ".dircache" this name is repalce d by ".git"
		perror("unable to create .dircache");	//if the directory is not made. as per named above .
		exit(1);				// terminationof program 
	}

	/*
	 * If you want to, you can share the DB area with any number of branches.
	 * That has advantages: you can save space by sharing all the SHA1 objects.
	 * On the other hand, it might just make lookup slower and messier. You
	 * be the judge.
	 */
	sha1_dir = getenv(DB_ENVIRONMENT); 		//having the enviorment of the .dircache directory
	if (sha1_dir) {					//checking if the the directory is made ...as per created above
		struct stat st;				// using the stat of the directory made.
		if (!stat(sha1_dir, &st) < 0 && S_ISDIR(st.st_mode)) // checking for the prmisisons on the the direcotry
			return;
		fprintf(stderr, "DB_ENVIRONMENT set to bad directory %s: ", sha1_dir); //error statement if the file having problems
	}

	/*
	 * The default case is to have a DB per managed directory. 
	 */
	sha1_dir = DEFAULT_DB_ENVIRONMENT; // getting a new directry path as as per defined in the macro in chache.h
	fprintf(stderr, "defaulting to private storage area\n"); //it sends a message to tell the directory named ".dircache/object"
	len = strlen(sha1_dir); //getting the length of the path ".dircache/object"
	if (mkdir(sha1_dir, 0700) < 0) { // now create a new directory ".dircache/object"
		if (errno != EEXIST) { // tells if the directory is not made and the reason is that it eexist
			perror(sha1_dir);// 
			exit(1);
		}
	}
	path = malloc(len + 40); 	//making variable to hold the lenghth of the path that hols the directories in ".dircache/object"
					//Adding 40 means that there is the hash numbr to be appended of lenght 40 chars. 
	memcpy(path, sha1_dir, len);	//now copying the path's part befre the hashnumber
	for (i = 0; i < 256; i++) {	//Now we are on making 256 directories in it. as per we divided the 40 char length hash number into 2+38
					// which actually means a 2 character directory name which is forst 2 charactrs of hash number.
		sprintf(path+len, "/%02x", i);	//now it actuallt make the file name a 2 character string and append it on the en of the path.
		if (mkdir(path, 0700) < 0) {	//Now we are on creating the directory with the name we calcuated.
			if (errno != EEXIST) {	// tells if the directory is not made and the reason is that it eexist
				perror(path);
				exit(1);
			}
		}
	}
	int k=mkdir(DEFAULT_HEADS_DIR,0700);
	if(k<0)
		perror("ERROR");
	char headfile[250];
	sprintf(headfile,"%s/master",DEFAULT_HEADS_DIR);
	fd = open(headfile,O_CREAT,0666);
	if(fd<0)
		perror("ERROR: ");
	return 0;
}
