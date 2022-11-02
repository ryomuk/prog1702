#include<stdio.h>

int
main(){
  int i;
  for(i = 0; i < 0x100; i++){
    putchar(i);
  }
}
