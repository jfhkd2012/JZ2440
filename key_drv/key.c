#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd;
	int cnt = 0;
	unsigned char key_val[4];
	
	fd = open("/dev/key", O_RDWR);
	if(fd < 0)
	{
		printf("can't open /dev/key\n");
		return -1;
	}
	while(1)
	{
		read(fd, key_val, 4);
		if(!key_val[0] || !key_val[1] || !key_val[2] || !key_val[3] )
		{
			printf("%04d key pressed : %d, %d, %d, %d\n", cnt++, key_val[0], key_val[1], key_val[2], key_val[3]);
		}
	}
	return 0;
}


