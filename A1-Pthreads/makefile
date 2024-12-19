CC = gcc
CFLAGS = -g -Wall -std=c99

all: A1

clean:
	rm -f *.o A1 serial

serialTest.o: serialTest.c
	$(CC) $(CFLAGS) -c serialTest.c -o serialTest.o	

serial: serialTest.o
	$(CC) serialTest.o -o serial

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

A1: main.o
	$(CC) main.o -o A1