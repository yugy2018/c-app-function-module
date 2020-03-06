#include <syslog.h>
#include <pthread.h>

#include "debug.h"

static pthread_t mThread;

void *newthread()
{
	int i=0;
	while(1)
	{
		debug(LOG_ERR, "newthread do nothing\n");
		i++;
		//sleep(2);
	}
}

int main(int argc, char **argv)
{
	char s[]="YUGY test";
	int p1 = 100;

	
	int ret = pthread_create(&mThread, NULL, newthread, NULL);/*接收串口线程*/
	while(1)
	{
		debug(LOG_NOTICE, "Parsing token: %s, " "value: %d\n", s, p1);
		p1++;
		sleep(2);
	}
	return 0;
}

