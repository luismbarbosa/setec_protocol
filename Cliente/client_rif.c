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

void sendmessage(int table_size, int n_plot, double * b_time, double * b_imag, double * b_freq, int sockfd){
    //int sockfd,
    int portno, n=1;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    /*
    char buf_total[(sizeof(double)*n_plot+sizeof(char)*n_plot)*3+1];
    char buf_int1[sizeof(double)*n_plot+sizeof(char)*n_plot+1];
    char buf_int2[sizeof(double)*n_plot+sizeof(char)*n_plot+1];
    char int_buf_time[sizeof(double)+1];
    char int_buf_imag[sizeof(double)+1];
    char int_buf_freq[sizeof(double)+1];
    */
    char * buf_total = (char *) malloc((sizeof(double)*n_plot+sizeof(char)*n_plot)*3+1);
    char * buf_int1 = (char *) malloc(sizeof(double)*n_plot+sizeof(char)*n_plot+1);
    char * buf_int2 = (char *) malloc(sizeof(double)*n_plot+sizeof(char)*n_plot+1);

    char int_buf_time[sizeof(double)+1];
    char int_buf_imag[sizeof(double)+1];
    char int_buf_freq[sizeof(double)+1];

   	int n_times = table_size / n_plot;
	  int resto = table_size % n_plot;
    int total = n_times + resto;

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
    int i=0, k=0, arg;
    //printf("Total number of cicles = %d\n", n_times);
    printf("ultima posicao = %d\n", n_times*n_plot+resto);
  for(i=0; i < n_times+1; i++){
    printf("In cicle i= %d/%d\n", i, n_times);
    arg = n_plot*(i+1);
    if(i == n_times) arg = n_times*n_plot+resto;
    memset(buf_total, 0, sizeof(buf_total));
    memset(buf_int1, 0, sizeof(buf_int1));
    memset(buf_int2, 0, sizeof(buf_int2));
    for(k=n_plot*i; k < arg; k++){
      memset(int_buf_time, 0, sizeof(int_buf_time));
      memset(int_buf_imag, 0, sizeof(int_buf_imag));
      memset(int_buf_freq, 0, sizeof(int_buf_freq));
      sprintf(int_buf_time, "%f", b_time[k]);
      sprintf(int_buf_imag, "%f", b_imag[k]);
      sprintf(int_buf_freq, "%f", b_freq[k]);
      strcat(buf_total, int_buf_time);
      strcat(buf_int1, int_buf_imag);
      strcat(buf_int2, int_buf_freq);
      if(k != n_plot*(i+1)-1 && k != n_times*n_plot+resto-1){
        strcat(buf_total,":");
        strcat(buf_int1,":");
        strcat(buf_int2, ":");
      }
    }
    strcat(buf_total,"|");
    strcat(buf_total, buf_int1);
    strcat(buf_total,"|");
    strcat(buf_total, buf_int2);
    n = sendto(sockfd, buf_total, strlen(buf_total), 0,(struct sockaddr *) &serveraddr, serverlen);
    if (n < 0) error("ERROR in sendto");
  }
}


int main () {
  int n_plot = 7, f_time = 10;

  double * btime = (double *) malloc(sizeof(double)*TABLE_SIZE);
  double * bimag = (double *) malloc(sizeof(double)*TABLE_SIZE);
  double * bfreq = (double *) malloc(sizeof(double)*TABLE_SIZE);

  //fill buff
  for(int i=0; i<TABLE_SIZE; i++){
  	btime[i] = sin(2* PI * f_time *(double) i/ (double) TABLE_SIZE);
    bimag[i] = sin(2* PI * 8 *(double) i/ (double) TABLE_SIZE);
 		bfreq[i] = 2* PI * (double) f_time *(double) i/ (double) TABLE_SIZE;
	}

  int sockfd; 
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  sendmessage(TABLE_SIZE, n_plot, btime, bimag, bfreq, sockfd);
  close(sockfd);
  printf("----THE END----\n");
  return(0);
}
