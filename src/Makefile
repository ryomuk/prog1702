TARGET	= prog1702
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
