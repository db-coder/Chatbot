#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define BACKLOG 1
#define MAX_INPUT_SIZE 256

int main(int argc, char *argv[])
{
	int sockfd,new_fd,port_no,i=0,n;
	struct sockaddr_in my_addr, their_addr;
	char inputbuf[MAX_INPUT_SIZE],buffer[10];
	if (argc!=2)												//Check if no. of arguments are correct. 
	{
		fprintf(stderr,"Wrong number of arguments provided.\n");
		exit(1);
	}
	for (i = 0; i < strlen(argv[1]); ++i)
	{
		if(isdigit(argv[1][i])==0)								//Check if only digits are given as input.
		{
			fprintf(stderr,"Port number should be a number.\n");
			exit(1);
		}
	}
	port_no=atoi(argv[1]);										//Convert port no. to interger.
	if((sockfd=socket(PF_INET,SOCK_STREAM,0))==-1)				//Make a socket.
	{
		perror("socket");
		exit(3);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port_no);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);
	if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)	//Check bind and bid the socket with a sock address.
	{
		perror("bind");
		exit(2);
	}
	listen(sockfd,BACKLOG);										//Listening on a port.
	int sin_size=sizeof(struct sockaddr_in);
	if((new_fd=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size))==-1)
	{
		perror("accept");
	}
	close(sockfd);												//Closing sockfd so that other clients don't interfere.
	bzero(inputbuf,MAX_INPUT_SIZE);
	read(new_fd,inputbuf,(MAX_INPUT_SIZE-1));					//Reading the messages from client through the port.
	while(! (inputbuf[0]=='B' && inputbuf[1]=='y' && inputbuf[2]=='e' && inputbuf[3]=='\n'))	//Server active unless "Bye" message is sent by a client.
	{
		strcpy(inputbuf,"OK ");
		strcat(inputbuf,inet_ntoa(their_addr.sin_addr));
		strcat(inputbuf,":");
		sprintf(buffer,"%d",ntohs(their_addr.sin_port));
		strcat(inputbuf,buffer);
		write(new_fd,inputbuf,strlen(inputbuf));				//Writing the message to the socket.
		read(new_fd,inputbuf,(MAX_INPUT_SIZE-1));				//Reading further messages.
	}
	strcpy(inputbuf,"Goodbye ");								
	strcat(inputbuf,inet_ntoa(their_addr.sin_addr));
	strcat(inputbuf,":");
	sprintf(buffer,"%d",ntohs(their_addr.sin_port));
	strcat(inputbuf,buffer);	
	write(new_fd,inputbuf,strlen(inputbuf));					//At the end "Goodbye" is sent.
	close(new_fd);												//The socket is closed finally.
}
