
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
void *copyfile(void *arguments);
int isdirectory(char *path);
void *copy_dir(void *arguments);
struct arg_struct {
    char arg1[100];
    char arg2[100];
};


int main (int argc, char *argv[]) {

if(argc!=3)
{
	fprintf(stderr, "Usage: %s filename \n", argv[0]);
		return 1;
}
if (!isdirectory(argv[2]))
	mkdir(argv[2],0777);
if (isdirectory(argv[1]))
{ 

	fprintf(stderr, "%s is a directory \n", argv[1]);
	struct arg_struct args;
	sprintf(args.arg1, "%s", argv[1]);
	sprintf(args.arg2, "%s", argv[2]);

	copy_dir(&args);
}
else
fprintf(stderr, "%s is not a directory \n", argv[1]);
return 0;
}
void *copyfile(void *arguments)
{
	int myfd, myfd1;
	char c = '!';
	struct arg_struct* args=arguments; 
	if( (myfd = open (args->arg1,O_RDONLY)) == -1 ) {
			perror("1Failed to open file");
				return 1;
			}
			
			if( (myfd1= open (args->arg2, O_CREAT | O_WRONLY)) == -1 ) {
			perror("Failed to open file");
				return 1;
			}
			while (read(myfd, &c,1) > 0)
				write(myfd1, &c, 1);

}
void *copy_dir(void *arguments)
{
	struct dirent *direntp;
	struct dirent *direntp1;
	DIR *dirp;
	DIR *dirp1;
	pthread_t tid[100];
	char filename[100000];
	char filename1[100000];
	int myfd, myfd1;
	char c = '!';
	int count=-1;
	int error;
	char a[100];
	char b[100];
	struct arg_struct* args=arguments; 
	sprintf(a, "%s", args->arg1);
	sprintf(b, "%s", args->arg2);
	dirp = opendir (a);
	dirp1= opendir (b);

	while ( (direntp = readdir (dirp) ) != NULL )
	{	//printf("inode number is %d %s \n", (int)direntp->d_ino, direntp->d_name);
	
		count=count+1;

		if (direntp->d_type == DT_DIR)
		{		
			args=(struct arg_struct*)malloc(sizeof(struct arg_struct));
			if (strcmp (direntp->d_name, "..") != 0 && strcmp (direntp->d_name, ".") != 0){
				sprintf(filename, "%s/%s", a, direntp->d_name);
				sprintf(filename1, "%s/%s", b, direntp->d_name);
				mkdir(filename1,0777);
				sprintf(args->arg1, "%s", filename);
				sprintf(args->arg2, "%s", filename1);
			
				if (count < 100) {

				if (error = pthread_create((&tid[count]), NULL, copy_dir, (void *)args))

				fprintf(stderr, "Failed to create thread: %s\n", strerror(error));

				} else {

				printf("Out of thread limit.\n");

				}
				if ((error = pthread_join(tid[count], NULL))!=0) 
        				fprintf(stderr, "Failed to join thread \n");
		}

		}
		else
		{
			sprintf(filename, "%s/%s", a, direntp->d_name);
			sprintf(filename1, "%s/%s", b, direntp->d_name);
			args=(struct arg_struct*)malloc(sizeof(struct arg_struct));
			sprintf(args->arg1, "%s", filename);
			sprintf(args->arg2, "%s", filename1);

				if (count < 100) {

				if (error = pthread_create((&tid[count]), NULL, copyfile, (void *)args))

				fprintf(stderr, "Failed to create thread: %s\n", strerror(error));

				} else {

				printf("Out of thread limit.\n");

				}
				if ((error = pthread_join(tid[count], NULL))!=0) 
        				fprintf(stderr, "Failed to join thread \n");


			}
		
		}
	
	closedir(dirp);
	closedir(dirp1);
	

}

int isdirectory(char *path){
struct stat statbuf;

if(stat(path, &statbuf) == -1)
return 0;
else
return S_ISDIR(statbuf.st_mode);
}
