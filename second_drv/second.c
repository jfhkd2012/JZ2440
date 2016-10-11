#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<stdio.h>

int main(int argc, char **argv)
{
	int fd;
	int val = 1;

	if(argc != 2)
	{
		printf("Usage : %s <on | off> \n", argv[0]);
		return -1;
	}
	fd = open("/dev/myled", O_RDWR);
	if(fd < 0)
	{
		printf("open device /dev/myled failure.\n");
		return -1;
	}

	if(strcmp(argv[1], "on") == 0)
		val = 1;
	else if(strcmp(argv[1], "off") == 0)
		val = 0;
	printf("val = %d\n", val);
	write(fd, &val, 4);

	return 0;
	
}
