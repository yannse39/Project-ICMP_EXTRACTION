#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "icmp_client.h"

#define PACKET_SIZE 1520
#define ICMP_HEADER_SIZE 8
#define PAYLOAD_SIZE 512

// Ici la fonction permet de calculer le Checksum ICMP
uint16_t calculate_checksum(unsigned char* buffer, int bytes)
{
    uint32_t checksum = 0;
    unsigned char* end = buffer + bytes;

    // odd bytes add last byte and reset end
    if (bytes % 2 == 1) {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }

    // add words of two bytes, one by one
    while (buffer < end) {
        checksum += buffer[0] << 8;
        checksum += buffer[1];
        buffer += 2;
    }

    // add carry if any
    uint32_t carray = checksum >> 16;
    while (carray) {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }

    // negate it
    checksum = ~checksum;

    return checksum & 0xffff;
}
//principale fonction du client ICMP
int getRandomNumber(int min, int max)
{
  return rand() % (max - min + 1) + min;
}
// ouvre le fichier en binaire 
int icmp_client(const char *serverIP, const char *fileName)
{
  FILE *file = fopen(fileName, "rb");
  if (file == NULL)
  {
    perror("File open failed");
    return 1;
  }
  //determine la taille du fichier 
  fseek(file, 0, SEEK_END);
  long taille_fichier = ftell(file);
  fseek(file, 0, SEEK_SET);
   
   // nombre total de chunk a envoyer 
  int totalChunks = (taille_fichier + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;

  // socket icmp
  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sock < 0)
  {
    perror("socket creation failed");
    fclose(file);
    return 1;
  }
  // creer une structure avec l ip du serveur 
  struct sockaddr_in destAddr;
  memset(&destAddr, 0, sizeof(destAddr));
  destAddr.sin_family = AF_INET;
  if (inet_pton(AF_INET, serverIP, &(destAddr.sin_addr)) <= 0)
  {
    perror("inet_pton failed");
    close(sock);
    fclose(file);
    return 1;
  }
  //buffer pour paquet ICMP 
  char packet[PACKET_SIZE];
  struct icmphdr *icmpHeader = (struct icmphdr *)packet;

  int nombre_chunk = 0;
  int byte_lu;
  //genere des nombres aleatoire 
  srand(time(NULL));
  unsigned int aid = getRandomNumber(1000, 9999);
  unsigned int bid = getRandomNumber(1000, 9999);

  char payload[PAYLOAD_SIZE];

  while ((byte_lu = fread(payload, 1, PAYLOAD_SIZE, file)) > 0)
  {
    memset(packet, 0, sizeof(packet));

    icmpHeader->type = ICMP_ECHO;
    icmpHeader->code = 0;
    icmpHeader->checksum = 0;
    icmpHeader->un.echo.id = getpid();
    icmpHeader->un.echo.sequence = 0;
    icmpHeader->checksum = calculate_checksum((unsigned char *)icmpHeader, ICMP_HEADER_SIZE);
     //donnee perso pour le chunk 
    unsigned int nombre = nombre_chunk; 
    unsigned int total = totalChunks; 

   //copie donne dans le paquet ICMP 
    memcpy(packet + ICMP_HEADER_SIZE, &aid, sizeof(aid));
    memcpy(packet + ICMP_HEADER_SIZE + sizeof(aid), &bid, sizeof(bid));
    memcpy(packet + ICMP_HEADER_SIZE + sizeof(aid) + sizeof(bid), &nombre,
           sizeof(nombre));
    memcpy(packet + ICMP_HEADER_SIZE + sizeof(aid) + sizeof(bid) + sizeof(nombre),
           &total, sizeof(total));

   
    memcpy(packet + ICMP_HEADER_SIZE + (4 * sizeof(unsigned int)), payload,
           byte_lu);

    //Taille totale du paquet 
    unsigned int packetSize =
        ICMP_HEADER_SIZE + (4 * sizeof(unsigned int)) + byte_lu;

  
    icmpHeader->checksum =
        calculate_checksum((unsigned char *)icmpHeader, packetSize);

   
    if (sendto(sock, packet, packetSize, 0, (struct sockaddr *)&destAddr,
               sizeof(destAddr))
        <= 0)
    {
      perror("sendto failed");
      close(sock);
      fclose(file);
      return 1;
    }

    printf("ICMP echo request sent!\n");
    printf("Sent chunk %d of %d\n", nombre_chunk + 1, totalChunks);

    nombre_chunk++;
  }

  fclose(file);
  close(sock);
  return 0;
}
