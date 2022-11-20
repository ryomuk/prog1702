/*
 * prog1702.c
 * A simple programmer for Intel 1702A EPROM using Raspberry Pi Zero
 *
 * Copyright (c) 2022 Ryo Mukai
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h> // for clock_gettime()

// for disable and enable interrupt
#define BLOCK_SIZE (4 * 1024)
#define PI0_PERI_BASE 0x20000000
#define PERI_BASE PI0_PERI_BASE

typedef unsigned char byte;
#define bit(x) (1<<(x))

#define LOOPS 32
#define MEMSIZE 0x100

#define A0 14
#define A1 15
#define A2 18
#define A3 23
#define A4 24
#define A5 25
#define A6  8
#define A7  7
#define D0 10
#define D1  9
#define D2 11
#define D3  5
#define D4  6
#define D5 13
#define D6 19 
#define D7 26  
#define VDD_VGG 20
#define PROGRAM 21

#define ON  1
#define OFF 0

#define delay_usec(x) delayNanoseconds((x)*1000L)
void delayNanoseconds(unsigned int howLong)
{
  struct timespec tNow, tEnd;

  clock_gettime(CLOCK_MONOTONIC, &tNow);
  tEnd.tv_sec = tNow.tv_sec;
  tEnd.tv_nsec = tNow.tv_nsec + howLong;
  if(tEnd.tv_nsec >= 1000000000L){
    tEnd.tv_sec++;
    tEnd.tv_nsec -= 1000000000L;
  }
  do{
    clock_gettime(CLOCK_MONOTONIC, &tNow);
  } while ( (tNow.tv_sec == tEnd.tv_sec) ?
	    (tNow.tv_nsec < tEnd.tv_nsec)
	    : (tNow.tv_sec < tEnd.tv_sec));
}

volatile unsigned int *g_irqen1;
volatile unsigned int *g_irqen2;
volatile unsigned int *g_irqen3;
volatile unsigned int *g_irqdi1;
volatile unsigned int *g_irqdi2;
volatile unsigned int *g_irqdi3;
unsigned int g_irq1, g_irq2, g_irq3;

int initInterrupt(){
  int mem_fd;
  char *map;
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0){
    return(-1);
  }
  map = (char*) mmap(NULL,
		     BLOCK_SIZE,
		     PROT_READ | PROT_WRITE,
		     MAP_SHARED,
		     mem_fd,
		     PERI_BASE + 0xb000
		     );
  if (map == MAP_FAILED){
    return(-1);
  }
  g_irqen1 = (volatile unsigned int *) (map + 0x210);
  g_irqen2 = (volatile unsigned int *) (map + 0x214);
  g_irqen3 = (volatile unsigned int *) (map + 0x218);
  g_irqdi1 = (volatile unsigned int *) (map + 0x21c);
  g_irqdi2 = (volatile unsigned int *) (map + 0x220);
  g_irqdi3 = (volatile unsigned int *) (map + 0x224);
  return(0);
}

void disableInterrupt(){
  g_irq1 = *g_irqen1;
  g_irq2 = *g_irqen2;
  g_irq3 = *g_irqen3;

  *g_irqdi1 = 0xffffffff;
  *g_irqdi2 = 0xffffffff;
  *g_irqdi3 = 0xffffffff;
}

void enableInterrupt()
{
  *g_irqen1 = g_irq1;
  *g_irqen2 = g_irq2;
  *g_irqen3 = g_irq3;
}

void initGPIO(){
  // Initialize WiringPi
  wiringPiSetupGpio();

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);

  pinMode(VDD_VGG, OUTPUT);
  digitalWrite(VDD_VGG, OFF);

  pinMode(PROGRAM, OUTPUT);
  digitalWrite(PROGRAM, OFF);
}

void setAddress(unsigned int address){
  // Address levels are approximately -40 volts for a logic "0"
  // and approximately zero volts for a logic "1".
  // ("The Intel Memory Design Handbook, Aug. 1973", p.2-4)

  // flip the bits for TBD62083("H" input Sink Driver down to -47V)
  address = ~address; 

  digitalWrite(A0, address & bit(0));
  digitalWrite(A1, address & bit(1));
  digitalWrite(A2, address & bit(2));
  digitalWrite(A3, address & bit(3));
  digitalWrite(A4, address & bit(4));
  digitalWrite(A5, address & bit(5));
  digitalWrite(A6, address & bit(6));
  digitalWrite(A7, address & bit(7));
}

void setData(unsigned int data){
  // A data level of approximately zero volts will result in the location
  // remaining unchanged, while a level of -47(\pm 1)V will program
  // a logic "1" (high output in read mode).
  // ("The Intel Memory Design Handbook, Aug. 1973", p.2-4)

  // Driver is TBD62083("H" input Sink Driver down to -47V)
  digitalWrite(D0, data & bit(0));
  digitalWrite(D1, data & bit(1));
  digitalWrite(D2, data & bit(2));
  digitalWrite(D3, data & bit(3));
  digitalWrite(D4, data & bit(4));
  digitalWrite(D5, data & bit(5));
  digitalWrite(D6, data & bit(6));
  digitalWrite(D7, data & bit(7));
}

void usage(){
  fprintf(stderr, "Usage: prog1702 [OPTION]... FILE\n");
  fprintf(stderr, "  -l[oops] (default=%d)\n", LOOPS);
  exit(1);
}

int main(int argc, char *argv[]){
  int i;
  unsigned int a, d;
  FILE *fp = stdin;
  char *filename = NULL;
  byte *buf = NULL;
  int bufsize = MEMSIZE;
  int loops = LOOPS;
  unsigned long t_start_total, t_total;
  unsigned long t_start_cycle, t_cycle;
  unsigned long t_start_vdd, t_vdd;
  double vdd_duty;

  initGPIO();
  
  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      /*  */ if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "-loops") == 0){
	i++;
	if(i >= argc) usage();
	loops = atoi(argv[i]);
      } else {
	usage();
      }
    } else {
      if(filename == NULL){
	filename = argv[i];
      } else {
	usage();
      }
    }
  }

  if( filename == NULL){
    usage();  // input file should be given by command line
    // fp = stdin;
  } else {
    if((fp = fopen(filename, "r")) == NULL){
      fprintf(stderr, "%s: Cannot open file '%s'\n", argv[0], filename);
      exit(1);
    }
  }
  if((buf = (byte *)calloc(bufsize, sizeof(byte))) == NULL){
    fprintf(stderr, "calloc() failed.\n");
    exit(1);
  }

  fread(buf, sizeof(byte), bufsize, fp);
  
  if(initInterrupt() !=0){
    fprintf(stderr, "initInterrupt() failed. Try sudo ./prog1702\n");
    exit(1);
  }

  // For best results, the 1602A/1702A should be programmed
  // by scanning through the addresses in binary sequence some
  // 32 times. Each pass repeats the same series of programming
  // pulses. The duty cycle for applied power must not exceed
  // 20%. As result, each pass takes about 4 seconds, with the
  // 32 passes taking just over 2 minutes.
  // ("The Intel Memory Design Handbook, Aug. 1973", p.2-4)
  
  t_start_total = micros();
  for(i = 0; i < loops; i++){
    for(a = 0; a < MEMSIZE; a++){
      d = buf[a];
      
      t_start_cycle = micros();
      
      disableInterrupt();
      //**************************************************************
      // Critical Region
      //**************************************************************
      // Disable interrupts so that this process does not stop
      // while power(Vdd, Vgg) or program pulses are asserted.
      
      setAddress(~a); // set binary complement address
      setData(d);
      delay_usec(100);  // t_ACW(>=25us)

      t_start_vdd = micros();
      digitalWrite(VDD_VGG, ON);
      delay_usec(50);  // t_ACH(>=25us)

      setAddress(a);
      delay_usec(20);  // t_ATW(>=10us)
      delay_usec(150); // t_VW(>=100us from VDDandVGG ON)

      digitalWrite(PROGRAM, ON);
      delay_usec(2500); // t_PW(<=3ms)

      digitalWrite(PROGRAM, OFF);
      delay_usec(50);   // t_VD(>=10us, <=100us)

      digitalWrite(VDD_VGG, OFF);
      t_vdd = micros() - t_start_vdd;

      //**************************************************************
      enableInterrupt();

      delay_usec(15*1000); // (>=12ms), t_DH(>=10us), t_ATH(>=10us)

      t_total = micros() - t_start_total;
      t_cycle = micros() - t_start_cycle;
      vdd_duty = (double) t_vdd / t_cycle * 100;

      fprintf(stderr, "\rLoop=%d/%d, %.2lfsec, a[%02x]<=%02x, %.1lfms/byte, DutyCycle(Vdd,Vgg)=%.1lf%%",
	      i+1, loops,
	      (double)t_total/1000/1000,
	      a, d,
	      (double)t_cycle/1000, vdd_duty
	      );
      /* Check Vdd,Vgg Duty Cycle */
      if(vdd_duty > 20.0){
	fprintf(stderr, "\nDuty Cycle(Vdd,Vgg) exceeded the limit(20%%)\n");
	exit(1);
      }
    }
  }
  fprintf(stderr, "\ndone.\n");
}
