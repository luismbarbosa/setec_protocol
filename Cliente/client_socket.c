/*
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <math.h>

#define PI 3.14159265
#define TABLE_SIZE 1000
/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

void sendmessage(int table_size, int buf_size, double * b_time, double * b_imag, double * b_freq, int sockfd){
    //int sockfd,
    int portno, n=1;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;

    char buf[(sizeof(double)*buf_size+sizeof(char)*buf_size)*2];
    char int_buf[(sizeof(double)*buf_size+sizeof(char)*buf_size)/2+1];
    char int_bufi[(sizeof(double)*buf_size+sizeof(char)*buf_size)/2+1];
    char int_buf_time[sizeof(double)+1];
    char int_buf_imag[sizeof(double)+1];
    char int_buf_freq[sizeof(double)+1];

   	int n_times = table_size / buf_size;
	  int resto = table_size % buf_size;

    hostname = "127.0.0.1";
    portno = 500;

    /* socket: create the socket */
    //sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    //if (sockfd < 0) error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
      (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
//#################################################################
    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    int i=0, k=0;
  for(i=0; i<n_times; i++){
    memset(buf, 0, sizeof(buf));
    memset(int_buf, 0, sizeof(int_buf));
    memset(int_bufi, 0, sizeof(int_bufi));
    for(k=i*buf_size/2; k<i*buf_size/2+buf_size/2;k++){
      memset(int_buf_time, 0, sizeof(int_buf_time));
      memset(int_buf_imag, 0, sizeof(int_buf_imag));
      memset(int_buf_freq, 0, sizeof(int_buf_freq));
      sprintf(int_buf_time, "%f", b_time[k]);
      sprintf(int_buf_imag, "%f", b_imag[k]);
      sprintf(int_buf_freq, "%f", b_freq[k]);
      strcat(buf,int_buf_time);
      strcat(int_buf,int_buf_imag);
      strcat(int_bufi,int_buf_freq);
      if(k != i*buf_size/2+buf_size/2-1){
        strcat(int_buf,":");
        strcat(int_bufi,":");
        strcat(buf, ":");
      }
    }
    strcat(buf,"|");
    strcat(buf, int_buf);
    strcat(buf,"|");
    strcat(buf, int_bufi);
    n = sendto(sockfd, buf, strlen(buf), 0,(struct sockaddr *) &serveraddr, serverlen);
    if (n < 0) error("ERROR in sendto");
    //fprintf(stderr, "Buffer: %s \t i=%d \t times: %d\n", buf, i, n_times);
  }
}


int main () {
  int buf_size = 16, f_time = 10;

  double btime[1000], bfreq[1000];
  memset(btime, 0, sizeof(btime));
  memset(bfreq, 0, sizeof(bfreq));
  //char buf[1000], int_buf[500], int_buf_time[100], int_buf_freq[100];
  //fill buff
  for(int i=0; i<TABLE_SIZE; i++){
  	btime[i] = sin(2* PI * f_time *(double) i/ (double) TABLE_SIZE);
 		bfreq[i] = 2* PI * (double) f_time *(double) i/ (double) TABLE_SIZE;
	}

  int sockfd; //=  malloc(sizeof(int));
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  sendmessage(TABLE_SIZE, buf_size, btime, btime, bfreq, sockfd);
  close(sockfd);
  return(0);
}
