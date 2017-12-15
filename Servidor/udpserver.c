#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>

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
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  const char *gui_commands[5];
  char *buf_split;
  char *command_parameters[10] = { NULL };
  int i = 0;
  fd_set readfds; 
  int rv;
  struct timeval tv;
  char *messages_received[3] = { NULL };
  int messages_received_count = 0;
  int c = 0;



  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);
  

  gui_commands[0] = "x";
  gui_commands[1] = "y";
  gui_commands[2] = "ofdm";
  gui_commands[3] = "qam";
  gui_commands[4] = "zoom";


  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sockfd < 0)
    error("ERROR opening socket");

  fcntl(sockfd, F_SETFL, O_NONBLOCK); 

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

	setbuf(stdout, NULL);
  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");
    
   for(i=0; i<3; i++){
   	messages_received[i] = (char *) malloc(sizeof(char) * BUFSIZE);
   }
   
   for(i=0; i<10; i++){
   	command_parameters[i] = (char *) malloc(sizeof(char) * BUFSIZE);
   }

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {
  
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    rv = select(sockfd + 1, &readfds, NULL, NULL, &tv); 
    
    if(rv == 1){
	    n = recvfrom(sockfd, buf, BUFSIZE, 0,
			 (struct sockaddr *) &clientaddr, (socklen_t*)&clientlen);
	    if (n < 0)
	      error("ERROR receiving packet");
	      
	    printf("Buf status: %s \n", buf);
	    
	    /* append the message received to the array messages_received */
		memcpy(messages_received[messages_received_count] , buf, n);
		*(messages_received[messages_received_count]+n) = '\0';
		printf("Counter: %d\n", messages_received_count);
		messages_received_count++;
		printf("mensagem guardada %s\n", messages_received[messages_received_count - 1]);

	    
	    if(messages_received_count == 3) {
	    	for(i=0; i<3; i++){
	    	    printf("Message: %s\n", messages_received[i]);
		    buf_split = strtok(messages_received[i], ":");
		    /* loop to iterate over the message received; split message by the delimiter ":" */
		    c = 0;
		    while (buf_split != NULL)
		    {
			memcpy(command_parameters[c] , buf_split, BUFSIZE);
			
			printf("Message[%d] received: %s\n", c, buf_split);
			buf_split = strtok(NULL, ":");
			c++;
		    }
		    
    		    if(command_parameters[0][0] == 'z')
    		    	command_parameters[0][4] = '\0';
    		    
		    /* is a change direction command in x axis */
		    if(strcmp(command_parameters[0], gui_commands[0]) == 0){	
		    	printf("Please turn the direction (left, right) to: %s\n", command_parameters[1]);
		    }
		    
		    else if(strcmp(command_parameters[0], gui_commands[1]) == 0){  	
		    	printf("Please turn the direction (front, back) to: %s\n", command_parameters[1]);
		    }
		    /* is a ofdm command */
		    else if(strcmp(command_parameters[0], gui_commands[2]) == 0){  	
		    	printf("Please use now ofdm\n");
		    	printf("Number of carriers: %s\n", command_parameters[1]);
		    	printf("Guard time: %s\n", command_parameters[2]);
		    	printf("# pilot frequencies: %s\n", command_parameters[3]);
		    	printf("Error correction? %s\n", command_parameters[4]);
		    }    	    	    	    	

		    /* is a qam command */
		    else if(strcmp(command_parameters[0], gui_commands[3]) == 0){  	
		    	printf("Please use now qam\n");
		    	printf("Number of symbols: %s\n", command_parameters[1]);
		    	printf("Magnitude: %s\n", command_parameters[2]);
		    }
		    
		    else if(strcmp(command_parameters[0], gui_commands[4]) == 0){  	
		    	printf("Please use change camera zoom\n");
		    	printf("Zoom range to use: %s\n", command_parameters[1]);
		    }
		    
		    /* not known command received; maybe send a message to GUI team to resend previous command */
		    else{
		    	printf("Sorry but I don't recognize this command: %s\nPlease resend!\n", command_parameters[0]);  	
		    }
		}
		messages_received_count = 0;
	  }
	  else {
	  	continue;
	  }
    }

  }
}
