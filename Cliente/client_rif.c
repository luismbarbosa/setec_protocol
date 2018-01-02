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
#include <complex.h>
//#include <time.h>

#define PI 3.14159265
#define TABLE_SIZE 64

/*
 * error - wrapper for perror
 */
void myerror(char *msg) {
    perror(msg);
    exit(0);
}

void sendmessage(int table_size, int n_plot, float complex *timeWave, float complex *freqWave, int sockfd1){
    //int sockfd1,
    int portno, n=1;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;

    char * buf_total = (char *) malloc(((sizeof(double)+1)*n_plot+sizeof(char)*n_plot)*4);
    char * buf_int1 = (char *) malloc((sizeof(double)+1)*n_plot+sizeof(char)*n_plot);
    char * buf_int2 = (char *) malloc((sizeof(double)+1)*n_plot+sizeof(char)*n_plot);
    char * buf_int3 = (char *) malloc((sizeof(double)+1)*n_plot+sizeof(char)*n_plot);
    //printf("Tamanho buf_total: %lu\n", ((sizeof(double)+1)*n_plot+sizeof(char)*n_plot)*4);
    //printf("Tamanho buf_outros: %lu\n", (sizeof(double)+1)*n_plot+sizeof(char)*n_plot);

    char int_buf_timer[sizeof(double)+1];
    char int_buf_timef[sizeof(double)+1];
    char int_buf_freqr[sizeof(double)+1];
    char int_buf_freqf[sizeof(double)+1];

    //char * buf_total = (char *) malloc( 4000 );
    //char * buf_int1 = (char *) malloc(2000);
    //char * buf_int2 = (char *) malloc(2000*sizeof(float));
    //char * buf_int3 = (char *) malloc(sizeof(float)*n_plot+sizeof(char)*n_plot+1);
/*
    char buf_total[4000];
    char buf_int1[2000];
    char buf_int2[2000];
    char buf_int3[2000];

    char int_buf_timer[32];
    char int_buf_timef[32];
    char int_buf_freqr[32];
    char int_buf_freqf[32];
*/
   	int n_times = table_size / n_plot;
	  int resto = table_size % n_plot;
    if(resto != 0) n_times++;
    int total = n_times + resto;

    //hostname = "127.0.0.1";
    hostname = "172.30.11.233";
    portno = 5005;

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
    //printf("ultima posicao = %d\n", n_times*n_plot+resto);
  for(i=0; i < n_times; i++){
    //printf("In cicle i= %d/%d\n", i, n_times);
    arg = n_plot*(i+1);
    if(i == n_times) arg = n_times*n_plot+resto;

    memset(buf_total, 0, sizeof(buf_total));
    memset(buf_int1, 0, sizeof(buf_int1));
    memset(buf_int2, 0, sizeof(buf_int2));
    memset(buf_int3, 0, sizeof(buf_int3));

/*
    memset(buf_total, 0, 4000);
    memset(buf_int1, 0, 2000);
    memset(buf_int2, 0, 2000);
    memset(buf_int3, 0, 2000);
*/
    //printf("arg: %d\n",arg);
    //printf("Tamanho total: %lu\n",sizeof(char)*((sizeof(float)*n_plot+sizeof(char)*n_plot)*4+1));

    for(k=n_plot*i; k < arg; k++){
     // printf("t\n");
      memset(int_buf_timer, 0, sizeof(int_buf_timer));
      memset(int_buf_timef, 0, sizeof(int_buf_timef));
      memset(int_buf_freqr, 0, sizeof(int_buf_freqr));
      memset(int_buf_freqf, 0, sizeof(int_buf_freqf));
      sprintf(int_buf_timer, "%f", creal(timeWave[k]));
      sprintf(int_buf_timef, "%f", cimag(timeWave[k]));
      sprintf(int_buf_freqr, "%f", creal(freqWave[k]));
      sprintf(int_buf_freqf, "%f", cimag(freqWave[k]));
      //printf("%s\n",int_buf_freqf);

      strcat(buf_total, int_buf_timer);
      strcat(buf_int1, int_buf_timef);
      strcat(buf_int2, int_buf_freqr);
      strcat(buf_int3, int_buf_freqf);
      if(k != n_plot*(i+1)-1 && k != n_times*n_plot+resto-1){
        strcat(buf_total, ":");
        strcat(buf_int1, ":");
        strcat(buf_int2, ":");
        strcat(buf_int3, ":");
      }

    }
    strcat(buf_total,"|");
    strcat(buf_total, buf_int1);
    strcat(buf_total,"|");
    strcat(buf_total, buf_int2);
    strcat(buf_total,"|");
    //printf("tamanho1: %lu, tamanho2: %lu , tamanho3: %lu \n",strlen(buf_int1),strlen(buf_int2),strlen(buf_int3));
    strcat(buf_total, buf_int3);
   // strcat(buf_total,"\r\n\0");
    //printf("tamanho total: %lu\n",strlen(buf_total));
    //printf("%s\n",buf_total);

    n = sendto(sockfd1, buf_total, strlen(buf_total), 0,(struct sockaddr *) &serveraddr, serverlen);
    if (n < 0) myerror("ERROR in sendto");
  }
  //free(buf_total);
  //close(sockfd1);
}

/*
int main () {
  int n_plot = 64, f_time1, f_time2;

  srand(time(NULL));

  float * timeWave = (float *) malloc(sizeof(float)*TABLE_SIZE);
  float * freqWave = (float *) malloc(sizeof(float)*TABLE_SIZE);


  //######### SOCKET ##########
  int sockfd1;
  sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd1 < 0) myerror("ERROR opening socket");
  //##############################
  printf("Pretared to send !\n");

  while (1) {
    f_time1 = rand()/(RAND_MAX/20);
    f_time2 = rand()/(RAND_MAX/20);
    printf("Freq 1: %d\n", f_time1);
    printf("Freq 2: %d\n", f_time2);
    for(int i=0; i<TABLE_SIZE; i++){
      timeWave[i] = sin(2* PI * f_time1 *(float) i/ (float) TABLE_SIZE);
      freqWave[i] = sin(2* PI * f_time2 *(float) i/ (float) TABLE_SIZE);
    }
    sendmessage(TABLE_SIZE, n_plot, timeWave, freqWave, sockfd1);
    printf("Sending...\n");
  }
  close(sockfd1);
  printf("----THE END----\n");
  return(0);
}*/
