#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<stdio.h>

int main(int argc, char **argv)
{
	int fd;
	int val = 1;
	fd = open("/dev/led", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/led fail\n");
		return -1;
	}
	if(argc != 2)
	{
		printf("Usage :  %s <on | off>\n", argv[0]);
		return -2;
	}
	if(strcmp(argv[1], "on") == 0)
		val = 1;
	else
		val = 0;
	write(fd, &val, 4);

	return 0;
}


