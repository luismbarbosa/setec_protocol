#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void error(char *msg) {
  perror(msg);
  exit(1);
}


int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  const char *gui_commands[3];
  char *buf_split;
  char *command_parameters[10];
  int i = 0;

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  gui_commands[0] = "direction";
  gui_commands[1] = "ofdm";
  gui_commands[2] = "qam";


  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR receiving packet");

    printf("Message received: %s\n", buf);
    buf_split = strtok(buf, ":");
    
    /* loop to iterate over the message received; split message by the delimiter ":" */
    i = 0;
    while (buf_split != NULL)
    {
	command_parameters[i++] = buf_split;
	printf("Message[%d] received: %s\n", i, buf_split);
	buf_split = strtok(NULL, ":");
    }
    
    /* is a change direction command */
    if(strcmp(command_parameters[0], gui_commands[0]) == 0){	
    	printf("Please turn the direction to: %s\n", command_parameters[1]);
    }
    /* is a ofdm command */
    else if(strcmp(command_parameters[0], gui_commands[1]) == 0){  	
    	printf("Please use now ofdm\n");
    	printf("Number of carriers: %s\n", command_parameters[1]);
    	printf("Guard time: %s\n", command_parameters[2]);
    	printf("# pilot frequencies: %s\n", command_parameters[3]);
    	printf("Error correction? %s\n", command_parameters[4]);
    }    	    	    	    	

    /* is a qam command */
    else if(strcmp(command_parameters[0], gui_commands[2]) == 0){  	
    	printf("Please use now qam\n");
    	printf("Number of symbols: %s\n", command_parameters[1]);
    	printf("Magnitude: %s\n", command_parameters[2]);
    }
    
    /* not known command received; maybe send a message to GUI team to resend previous command */  	
    else{
    	printf("Sorry but I don't recognize this command: %s\nPlease resend!\n", command_parameters[0]);  	
    }
  }
}
