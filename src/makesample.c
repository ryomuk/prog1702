#include<stdio.h>

int
main(){
  int i, j;

  puts("Test Data 1702A");

  j = 1;
  for(i = 0; i < 8; i++){
    putchar(j);
    j <<=1;
  }
  j = 1;
  for(i = 0; i < 8; i++){
    putchar(j);
    j = (j<<1)+1;
  }
  for(i = 0; i < 0x10; i++){
    putchar(i*0x10);
  }
  for(i = 0; i < 0xd0; i++){
    putchar(i);
  }
}
