#include <stdio.h>      // Default System Calls
#include <stdlib.h>     // Needed for OS X
#include <string.h>     // Needed for Strlen
#include <sys/socket.h> // Needed for socket creating and binding
#include <netinet/in.h> // Needed to use struct sockaddr_in
#include <time.h>       // To control the timeout mechanism

#define EXPR_SIZE   1024
#define BUFLEN      512
#define TRUE        1
#define FALSE       0
#define SERVERLEN   1024

int main(int argc, char **argv){

    long portNum;           // Since it's possible to input a value bigger
                            // than 65535 we'll be using long to
                            // avoid overflows
    char expr[EXPR_SIZE];
    char server[SERVERLEN];
    int fd;                 // file descriptor for the connected socket
    int buf[512];
    struct hostent *h;           // information of the host
    unsigned int addrLen;        // address length after getting the port number
    struct sockaddr_in myaddr;   // address of the client
    struct sockaddr_in servaddr; // server's address
    unsigned int exprLen;
    socklen_t slen = sizeof(servaddr);

    printf("Enter server name or IP address:");
    scanf("%s",server);
    printf("Enter port:");
    scanf("%ld",&portNum);
    if ((portNum < 0) || (portNum > 65535)) {
        printf("Invalid port number. Terminating.");
        return 0;
    }
    printf("Enter expression:");
    scanf("%s",expr);

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("cannot create socket");
        return 0;
    }

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(0);

    if(bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0){
        perror("cannot bind");
        return 0;
    }

    /*
     // Discovering the port number the OS allocated
     addrLen = sizeof(myaddr);
     if(getsockname(fd, (struct sockaddr *)&myaddr, &addrLen) < 0){
     perror("cannot getsockname");
     return 0;
     }
     printf("local port number = %d\n", ntohs(myaddr.sin_port));
     */

    memset((char*)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htonl(portNum);

    exprLen = sizeof(expr);


    while(TRUE){
        printf("Sending message to %s port %ld\n",server, portNum);
        if (sendto(fd, expr, strlen(expr), 0, (struct sockaddr *)&servaddr, slen) < 0) {
            perror("cannot sendto()");
        }
        printf("Success\n");

    }


    return 0;
}
