#include <stdlib.h>   
#include <stdio.h>   
//#include <sys/types.h>   
//#include <sys/socket.h>   
//#include <netinet/in.h>   
#include <errno.h>   
//#include <arpa/inet.h>  
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h> 
#include <time.h>
#include <fcntl.h>
//#include <dirent.h>
//#include <sys/stat.h>

#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <pthread.h>


//#define ConsoleDev	"/dev/ttyO1"	/**/
static char *ConsoleDev;

#define MAX(x,y)  ((x)>(y)?(x):(y))
int  SerialFd;


void setTermios(struct termios * pNewtio, int uBaudRate)
{
	bzero(pNewtio, sizeof(struct termios)); /* clear struct for new port settings */

	pNewtio->c_cflag = uBaudRate | CS8 | CREAD | CLOCAL;
	pNewtio->c_lflag &= ~(ICANON | ECHO | ECHONL | ECHOE | ISIG);
   	pNewtio->c_iflag |= (IGNBRK | IGNPAR);
    	pNewtio->c_iflag &= ~(BRKINT | INPCK | ISTRIP | INLCR | IGNCR |
                           ICRNL | IXON | IXOFF | IXANY);
    	pNewtio->c_oflag &= ~OPOST;
	/*

	initialize all control characters

	default values can be found in /usr/include/termios.h, and

	are given in the comments, but we don't need them here

	*/

	pNewtio->c_cc[VINTR] = 0; /* Ctrl-c */
	pNewtio->c_cc[VQUIT] = 0; /* Ctrl-\ */
	pNewtio->c_cc[VERASE] = 0; /* del */
	pNewtio->c_cc[VKILL] = 0; /* @ */
	pNewtio->c_cc[VEOF] = 4; /* Ctrl-d */
	pNewtio->c_cc[VTIME] = 2; /* inter-character timer, timeout VTIME*0.1 */
	pNewtio->c_cc[VMIN] = 0; /* blocking read until VMIN character arrives */
	pNewtio->c_cc[VSWTC] = 0; /* '\0' */
	pNewtio->c_cc[VSTART] = 0; /* Ctrl-q */
	pNewtio->c_cc[VSTOP] = 0; /* Ctrl-s */
	pNewtio->c_cc[VSUSP] = 0; /* Ctrl-z */
	pNewtio->c_cc[VEOL] = 0; /* '\0' */
	pNewtio->c_cc[VREPRINT] = 0; /* Ctrl-r */
	pNewtio->c_cc[VDISCARD] = 0; /* Ctrl-u */
	pNewtio->c_cc[VWERASE] = 0; /* Ctrl-w */
	pNewtio->c_cc[VLNEXT] = 0; /* Ctrl-v */
	pNewtio->c_cc[VEOL2] = 0; /* '\0' */
	
}

int  WriteCmdToSerial(char *Cmd,int len)
{
	int result = 0;
	
	if(SerialFd <= 0) 
	{
	    printf("==SerialFd q111==\n");
	    return -1;
	}
	
	if(Cmd==NULL)
	{
	      printf("warn: write null data to serial\n");
   	      return -1;
	}
	printf(">>Write %d Bytes to Serial:%s\n",len,Cmd);	
	result = write(SerialFd,Cmd,len);

	if(result <= 0)
	{
		printf("error: write data to serial(%d)\n",result);
		return -1;
	}

	return 0;
}

/*发送数据到串口*/
void SendToModule()
{
    //unsigned char cmd[4]={0};
    unsigned char cmd[128];
	int cmdlen;
	//cmd[0]=0x01;
	//cmd[1]=0x02;
	//cmd[2]=0x03;
	//cmd[3]=0x04;
	//cmdlen=1;
	while(1){
		fgets(cmd,128,stdin);
		cmdlen = strlen(cmd);
		//printf(">>Write %d Bytes to Serial:%s\n",cmdlen,cmd);	
		WriteCmdToSerial(cmd,cmdlen-1);
		//sleep(1);
	}
	return ;
}

void *WaitForPlayout()
{
	int maxfd, ret,i,j;
	fd_set dtamR,dtamR1;      /* set of fifo read filedescriptors for select() */
	int readlen;
	char buffer[128]={0};
	FD_ZERO(&dtamR);
	maxfd = 0;
	memset(buffer,0,64);
	FD_SET(SerialFd, &dtamR);
	maxfd = MAX(maxfd, SerialFd);
	printf("\nwaiting for playout...\n");
	while(1)
	{                
		memcpy( &dtamR1, &dtamR, sizeof(dtamR));	
		memset(buffer,0,128);
		ret = select((maxfd+1), &dtamR1, NULL, NULL, NULL);
	       if(ret < 0)  {
	    		continue;
		}
		if(FD_ISSET(SerialFd, &dtamR1)) {
			if((readlen = read(SerialFd, buffer, 128)) < 0)
			{
				return (void *)-1 ;				
			}

			#if 1
			printf("rcv:len==%d ,daet:%s\n",readlen,buffer);
			#else
			for(i=0;i<readlen;i++)
			{
			   printf("rcv:0x%02x\n",buffer[i]);
			}
			printf("\n");
			#endif
			
			continue;

		}
	}

}
static pthread_t mRingThread;

int main(int argc, char *argv[])
{
    if(argc !=2)
	{
		printf("%s <tty*>\n",argv[0]);
		return -1;
	}
	ConsoleDev = argv[1];
	
	struct termios oldios;
	struct termios newios; 


	SerialFd = open(ConsoleDev,O_RDWR | O_NOCTTY);
	if(SerialFd <0)
	{
		printf("/r/nopen failed.\n");
		return -1;
	}
	tcgetattr(SerialFd,&oldios);	/*获取当前串口设置*/
	bzero(&newios,sizeof(newios));	/*清空当前串口设置*/ 
	setTermios(&newios,B115200);	/*串口比特率设置*/
	tcflush(SerialFd,TCIFLUSH);		/*端口复位,清空原有数据*/
	tcsetattr(SerialFd,TCSANOW,&newios); /*使端口属性设置生效*/ 
	
	int ret = pthread_create(&mRingThread, NULL, WaitForPlayout, NULL);/*接收串口线程*/
	
	SendToModule();/*发送数据到串口*/
	
	pthread_join(mRingThread,NULL);

}

