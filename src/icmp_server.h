#ifndef ICMP_SERVER_H
#define ICMP_SERVER_H

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

struct IP
{
    int ip_hl;
};
struct ICMP
{
    int icmp_type;
    int icmp_cksum;
};
struct IP *ipHeader;
struct ICMP *icmpHeader;

void getTimestamp(char *timestamp, size_t size);
int icmp_server();
#endif /*ICMP_SERVER_H*/
