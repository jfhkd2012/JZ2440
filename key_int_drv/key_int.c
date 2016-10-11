#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned char key_val;

	fd = open("/dev/key", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/key failed\n");
		return -1;
	}
	while(1)
	{
		read(fd, &key_val, 1);
		printf("key_val = %d\n", key_val);
	}
}
