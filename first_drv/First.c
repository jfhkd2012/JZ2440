#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<stdio.h>

int main(void)
{
	int fd;
	int val = 1;
	fd = open("/dev/xyz", O_RDWR);
	if(fd < 0)
	{
		printf("open device /dev/xyz failure.\n");
		return -1;
	}
	write(fd, &val, 4);
	
}
