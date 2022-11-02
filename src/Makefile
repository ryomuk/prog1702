TARGET	= prog1702 makesample
OBJ	= prog1702.o makesample.o
LFLAGS	= -lwiringPi

#CFLAGS	= -Wall -g3
CFLAGS	= -Wall

CC	= g++ $(CFLAGS) $(LFLAGS)

all: $(OBJ) $(TARGET)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ)

.PHONY: clean
