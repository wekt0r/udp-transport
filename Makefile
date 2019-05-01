CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

all: main.o writer.o receiver.o sender.o
	$(CC) -o transport $^ $(CFLAGS)

%.o: %.c utils.h 
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
distclean:
	rm -rf *.o transport
