#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

int fd;


void my_signal_fun(int signum)
{
	unsigned char key_val;

	read(fd, &key_val, 1);
	printf("key_val : 0x%x\n", key_val);
}


int main(int argc, char **argv)
{
	
	unsigned char key_val;

	int ret;
	int flags;

	//signal(SIGIO, my_signal_fun);

	fd = open("/dev/key", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/key failed\n");
		return -1;
	}

	//fcntl(fd, F_SETOWN, getpid());
	//flags = fcntl(fd, F_GETFL);
	//fcntl(fd, F_SETFL, flags | FASYNC);
	while(1)
	{
		ret = read(fd, &key_val, 1);
		printf("key_val : 0x%x,  ret = %d\n", key_val, ret);
		sleep(5);
		
	}

	return 0;
}
