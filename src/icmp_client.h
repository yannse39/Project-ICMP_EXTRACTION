#ifndef ICMP_CLIENT_H
#define ICMP_CLIENT_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PACKET_SIZE 1520
#define ICMP_HEADER_SIZE 8
#define PAYLOAD_SIZE 512

uint16_t calculate_checksum(unsigned char *buffer, int bytes);
int getRandomNumber(int min, int max);
int icmp_client(const char *serverIP, const char *fileName);

#endif /* ICMP_CLIENT_H */
