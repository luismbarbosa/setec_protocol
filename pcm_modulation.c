#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#define PI 3.14159265
/*##################################

compile like:
gcc pcm_modulation.c -o pcm_modulation -lm

##################################*/
//fill an array
void fill_array(int *data, int size){
  for (int i=0; i<size; i++) {
         if(i % 2 == 0) data[i] = 0;
         else data[i] = 1;
       }
}

void tukeywin(double *data_up, double *data_down, int size, double r){
  double step = (double)1/size;
  printf("step= %f\n", step);

  double x = 0;
  int count = 0;
  double argum_up = 0, argum_down = 0;
  for(double i=0; i < 1; i=i+step){
    x = count * step;
    printf("for cycle: %f\n", i);
    argum_up = (2*(double)PI/r)*(x-((double)r/2));
    argum_down = (2*(double)PI/r)*(x-1+((double)r/2));
    printf("argum up= %f\t argum down= %f\n", argum_up, argum_down);
    data_up[count] = (double)1/2*(1+cos(argum_up));
    data_down[count] = (double)1/2*(1+cos(argum_down));
    count++;
  }
}

int main(int argc, char const *argv[]) {
  int size = 10; //size of the message
  int bit[sizeof(int)*size]; //array with the message
  fill_array(bit, size); //fill the array with 0 and 1

  //define the first state as the first position
  int state = bit[0];

  //numero de vezes em que o mesmo sinal aparece seguido
  //ex: 001 vemos que o 0 aparece 2 vezes logo seq=2
  int seq = 1, i;

  size = 25;
  double data_u[sizeof(double)*size];
  double data_d[sizeof(double)*size];
  double r = 0.66;

  tukeywin(data_u, data_d, size, r);

  for (i=0; i < size; i++){
    printf("This is data: %f\n", data_u[i]);
  }
  //ciclo que corre o comprimento da sequencia de bits
  //a ser enviada
  for (i=0; i < size-1; i++) {
    //se houver dois estados seguidos:
    //ex: 11 ou 00
    if(state == bit[i+1]){
      seq++;
    }
    //se houver uma mudança ex: 01 ou 10
    else{
      int c;
      switch (state) {
        case 1: c=1;
        case 0: c=0;
      }
    }
  }

  return 0;
}
