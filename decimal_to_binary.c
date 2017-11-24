/* 
* C program to convert decimal numbers to binary numbers
*/
 
#include <stdio.h>
#include <conio.h>
 
long decimalToBinary(long n);
int main() {
    long decimal;
    printf("Enter a decimal number\n");
    scanf("%ld", &decimal);
    printf("Binary number of %ld is %ld", decimal, decimalToBinary(decimal));
     
 getch();
    return 0;
}
 
/* Function to convert a decinal number to binary number */
long decimalToBinary(long n) {
    int remainder; 
 long binary = 0, i = 1;
  
    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}
