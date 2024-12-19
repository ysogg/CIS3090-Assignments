CC = gcc
CFLAGS = -g -Wall -std=c99

all: a2encrypt a2decrypt_serial a2decrypt

clean:
	rm -f *.o a2encryp a2decrypt_serial a2decrypt

decrypt.o: decrypt.c
	mpicc $(CFLAGS) -c decrypt.c -o decrypt.o	

decrypt: decrypt.o
	mpicc decrypt.o -o decrypt

a2decrypt.o: a2decrypt.c
	mpicc $(CFLAGS) -c a2decrypt.c -o a2decrypt.o	

a2decrypt: a2decrypt.o
	mpicc a2decrypt.o -o a2decrypt

a2decrypt_serial.o: a2decrypt_serial.c
	$(CC) $(CFLAGS) -c a2decrypt_serial.c -o a2decrypt_serial.o

a2decrypt_serial: a2decrypt_serial.o
	$(CC) a2decrypt_serial.o -o a2decrypt_serial

a2encrypt.o: a2encrypt.c
	$(CC) $(CFLAGS) -c a2encrypt.c -o a2encrypt.o

a2encrypt: a2encrypt.o
	$(CC) a2encrypt.o -o a2encrypt