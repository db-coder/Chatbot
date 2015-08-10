#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define MAX_INPUT_SIZE 256

int main(int argc, char *argv[])
{
	fd_set master, read_fds;
	int fdmax,sockfd,newfd,yes=1,addrlen,i,j,size;
	struct sockaddr_in my_addr, their_addr,client_addr;			//Server and Client IP address
	char inputbuf[MAX_INPUT_SIZE],buffer[10];
	size=sizeof(struct sockaddr);
	FD_ZERO(&master);											//Clear master ans temp sets.
	FD_ZERO(&read_fds);
	if(argc!=2)													//Check if no. of arguments are correct. 
	{
		fprintf(stderr,"Wrong number of arguments provided.\n");
		exit(1);
	}
	for(i = 0; i < strlen(argv[1]); ++i)
	{
		if(isdigit(argv[1][i])==0)								//Check if only digits are given as input.
		{
			fprintf(stderr,"Port number should be a number.\n");
			exit(1);
		}
	}
	int port_no=atoi(argv[1]);									//Convert port no. to interger.
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)				//Make a socket.
	{
		perror("socket");
		exit(3);
	}
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
	{
		perror("setsockopt");
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
	if(listen(sockfd,BACKLOG)==-1)								//Listening on a port.
	{
		perror("listen");
		exit(3);
	}
	FD_SET(sockfd,&master);										//Add the socket to the master set.
	fdmax=sockfd;

	while(1)													//The server waits for listening from a client even when all the previous connections are closed.
	{
		read_fds=master;
		if (select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1)
		{
			perror("select");
			end(3);
		}
		for(i=0;i<=fdmax;i++)									//Looking for data to read from existing connections.
		{
			if(FD_ISSET(i,&read_fds))							//Data to read found.
			{
				if(i==sockfd)
				{
					addrlen=sizeof(their_addr);					//New connections.
					if((newfd=accept(sockfd,(struct sockaddr *)&their_addr,&addrlen))==-1)
					{
						perror("accept");
					}
					else
					{
						FD_SET(newfd,&master);					//Adding socket to master set.
						if(newfd>fdmax)							//Update maximum if changed.
						{
							fdmax=newfd;
						}
					}
				}
				else
				{
					recv(i,inputbuf,sizeof(inputbuf),0);
					if(inputbuf[0]=='B' && inputbuf[1]=='y' && inputbuf[2]=='e' && inputbuf[3]=='\n')
					{
						getpeername(i,(struct sockaddr *)&client_addr,&size);
						strcpy(inputbuf,"Goodbye ");								
						strcat(inputbuf,inet_ntoa(client_addr.sin_addr));
						strcat(inputbuf,":");
						sprintf(buffer,"%d",ntohs(client_addr.sin_port));
						strcat(inputbuf,buffer);
						send(i,inputbuf,strlen(inputbuf),0);
						close(i);								//Closing the connection.
						FD_CLR(i,&master);						//Removing socket from master set.
					}
					else if(inputbuf[0]=='L' && inputbuf[1]=='i' && inputbuf[2]=='s' && inputbuf[3]=='t' && inputbuf[4]=='\n')
					{
						strcpy(inputbuf,"OK ");					//Printing the socket which is asking for List.
						getpeername(i,(struct sockaddr *)&client_addr,&size);							
						strcat(inputbuf,inet_ntoa(client_addr.sin_addr));
						strcat(inputbuf,":");
						sprintf(buffer,"%d",ntohs(client_addr.sin_port));
						strcat(inputbuf,buffer);
						for(j=0;j<=fdmax;j++)
						{
							if(FD_ISSET(j,&master))
							{
								if(j!=sockfd && j!=i)			//The other sockets in the master set.
								{
									strcat(inputbuf,", ");
									getpeername(j,(struct sockaddr *)&client_addr,&size);							
									strcat(inputbuf,inet_ntoa(client_addr.sin_addr));
									strcat(inputbuf,":");
									sprintf(buffer,"%d",ntohs(client_addr.sin_port));
									strcat(inputbuf,buffer);
								}
							}
						}
						send(i,inputbuf,strlen(inputbuf),0);	//Printing the whole list of sockets.
					}
					else

					{
						getpeername(i,(struct sockaddr *)&client_addr,&size);
						strcpy(inputbuf,"OK ");								
						strcat(inputbuf,inet_ntoa(client_addr.sin_addr));
						strcat(inputbuf,":");
						sprintf(buffer,"%d",ntohs(client_addr.sin_port));
						strcat(inputbuf,buffer);
						send(i,inputbuf,strlen(inputbuf),0);
					}
				}
			}
		}
	}
	close(sockfd);
	return 0;
}
