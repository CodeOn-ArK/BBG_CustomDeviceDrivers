#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV "/dev/pcd"

int main(int argc, char *argv[]){
	char str[10];
        int perm;
	sprintf(str, DEV "%d", atoi(argv[1]));
	perm = atoi(argv[2]);
	printf("perm -> %d\n", perm);
	printf("O_RDONLY -> %d\nO_WRONLY -> %d\nO_RDWR -> %d\n", O_RDONLY, O_WRONLY, O_RDWR);
	printf("opening %s with perm %d\n", str, perm);

	int fd = open(str, perm);
	if(fd < 0){
		perror("unable to open file\n");
		return EXIT_FAILURE;
	}

	printf("opened successfully\n");
	close(fd);
	return 0;
}
