TARGET	= makedata makedata00ff

CC	= g++ $(CFLAGS) $(LFLAGS)

all: $(OBJ) $(TARGET)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ)

.PHONY: clean
