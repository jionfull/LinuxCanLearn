#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 

#ifndef AF_CAN
#define AF_CAN 29
#endif

void test_can_rw(int fd)
{
	int ret,i=0;
	int packageCount=0;
	int totalCount=0;
	struct can_frame framedump[1000];
	struct can_frame frame;
	fd_set rset;
	while(1)
	{
		FD_ZERO(&rset);
		FD_SET(fd,&rset);
        	ret=select(fd+1,&rset,NULL,NULL,NULL);
		if(ret==0)
		{
			printf("select time out");
		}
		while(1)
		{
	         	ret = recv(fd,&framedump,sizeof(frame)*1000,0);
		       if(ret<sizeof(frame)) 
		        {
		           printf("To Short\n");
		 //  return;
		           break;
	           	}
		        else
		        {
		          packageCount=ret/sizeof(frame);
			 
		          totalCount+=ret/sizeof(frame);

		        }
			
                        if(totalCount%1000==0)
			{
			     printf("Can_Id %d\n",totalCount);
			}

	           	{
        	          usleep(1000);
		        }

		}

		
		
	        
		
	}

}

int main(int argc,char *argv[])
{
	 int s;
	 int ret;
	
	 struct sockaddr_can addr;
	 struct ifreq interfaceRequest;
	 printf("start\r\n");
	 
	 int buflen=sizeof(struct can_frame)* 10000;

	 s=socket(AF_CAN,SOCK_RAW,CAN_RAW);
	 if(s<0)
	 {
	    printf("new socket err\r\n");
	 }
	 
	 strcpy(interfaceRequest.ifr_name,"can1");
	 ret = ioctl(s,SIOCGIFINDEX,&interfaceRequest);
	 if(ret<0)
	 {
		 printf("ioctl Err\n");
		 return -1;
	 }
         if(0!=setsockopt(s,SOL_SOCKET,SO_RCVBUF,&buflen,4))
         {
	   printf("Set ReicveBuf Len err\n");
	   return -2;
         }
	 addr.can_family=AF_CAN;
	 addr.can_ifindex=interfaceRequest.ifr_ifindex;
	 
	 ret =bind(s,(struct sockaddr *)&addr,sizeof(addr));

	 if(ret<0)
	 {
		 perror("bind failed");
		 return 1;

	 } 


	 test_can_rw(s);
	 

}
