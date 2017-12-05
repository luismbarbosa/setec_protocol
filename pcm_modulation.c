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
  double step = (double)1/(size*r*2);
  //printf("N= %f\n", r/(2*step));

  double x = 0;
  int count = 0;
  double argum_up = 0, argum_down = 0;
  for(double i=0; i <= r/2; i=i+step){
    x = count * step;
    //printf("for cycle: %f \t contador= %d\n", i, count);
    argum_up = (2*(double)PI/r)*(x-((double)r/2));
    argum_down = (2*(double)PI/r)*(x-1+((double)r/2));
    //printf("argum up= %f\t argum down= %f\n", argum_up, argum_down);
    data_up[count] = (double)1/2*(1+cos(argum_up));
    data_down[count] = (double)1/2*(1+cos(argum_down));
    //printf("UP= %f\t DOWN= %f\n", data_up[count], data_down[count]);
    count++;
  }
}

int main(int argc, char const *argv[]) {
  int size_i = 10; //size of the message
  int bit[sizeof(int)*size_i]; //array with the message
  fill_array(bit, size_i); //fill the array with 0 and 1

  //define the first state as the first position
  int state = bit[0];
  //numero de vezes em que o mesmo sinal aparece seguido
  //ex: 001 vemos que o 0 aparece 2 vezes logo seq=2
  int seq = 1, i;

  int n_amostra = 49; //numero de amostras para representação do 0/1
  double r = 0.66;

  //numero de amostras para a subida/descida
  int size_t = r/(2*(double)1/(n_amostra*r*2));

  printf("size tukeywin = %d\n", size_t);
  double data_u[sizeof(double)*size_t];
  double data_d[sizeof(double)*size_t];
  /*No pior caso possivel teremos apenas mudanças de 0 para 1
  //teremos de alocar 49 vezes a representação do 0 e/ou 11
  //mais a representação da subida = size_t e descida (daí ser x2)*/
  double *data_sent = (double *) malloc(sizeof(double)*(n_amostra*size_i+size_t*2));
  tukeywin(data_u, data_d, n_amostra, r);

  /*/######## JUST A TEST ###############################
  for (i=0; i < size; i++){
    printf("This is data: %f \t %f\n", data_u[i], data_d[i]);
  }
  //####################################################/*/

  //ciclo que corre o comprimento da sequencia de bits
  //a ser enviada
  int index = 0;
  printf("First state = %d, \t Second state = %d\n", state, bit[1]);
  for (int k=0; k < size_i; k++) {
    //se houver dois estados seguidos:
    //ex: 11 ou 00
    if(state == bit[k+1]){
      seq++;
      //printf("TRUE\n");
    }
    //se houver uma mudança ex: 01 ou 10
    else{
      //int c;
      //printf("FALSE\n");
      //printf("In the switch %d, state = %d\n", k, state);
      switch (state) {
        case 1: //estamos num caso de descida
          for(i=0;i<=seq*n_amostra;i++){
              data_sent[index] = 1;
              index++;
          }
          for(i=0;i<=size_t;i++){
            data_sent[index] = data_d[i];
            index++;
          }
          printf("INDEX em 1: %d\n", index);
          state = 0;
        break;

        case 0: //estamos num caso de subida
          for(i=0;i<=seq*n_amostra;i++){
              data_sent[index] = 0;
              index++;
          }
          for(i=0;i<=size_t;i++){
            data_sent[index] = data_u[i];
            index++;
          }
          state = 1;
          printf("INDEX em 0: %d\n", index);
        break;

        default:
        break;
      }
      seq = 1;
    }
  }
  printf("Calc= %d\n", size_t);
  for(i=0;i<=index;i++){
    printf("This is the final singal %d: %f\n", i, data_sent[i]);
  }
  return 0;
}
