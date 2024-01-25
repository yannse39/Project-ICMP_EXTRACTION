CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wvla -Werror -pedantic

all:	src/icmp_server.o	src/icmp_client.o	src/main.o

src/icmp.o:        src/icmp_server.c	src/icmp_client.c
	$(CC)	$(CFLAGS)	-o	src/icmp_server.o	src/icmp_client.o	src/main.o	-c	src/icmp_server.c	src/icmp_client.c	src/main.c
clean:
	rm	src/*.o
   
