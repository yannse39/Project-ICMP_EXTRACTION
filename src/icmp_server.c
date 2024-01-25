#include "icmp_server.h"

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
// permet d obtenir le timestamp actuel
void getTimestamp(char *timestamp, size_t size)
{
    time_t temps_lecture;
    struct tm *temps;

    time(&temps_lecture);
    temps = localtime(&temps_lecture);

    strftime(timestamp, size, "%Y-%m-%dT%H:%M:%S", temps);
}

int icmp_server()
{
    // socket brut ICMP
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0)
    {
        perror("socket creation failed");
        return 1;
    }

    char paquet[PACKET_SIZE];
    struct sockaddr_in client;
    socklen_t clientAddrLen = sizeof(client);

    unsigned char *Chunks = NULL;
    int *chunkRecu = NULL;
    int totalChunksRecu = 0;
    int totalChunkssave = -1;
    FILE *output = NULL;

    // boucle infinie pour parcourir les paquet ICMP
    while (1)
    {
        memset(paquet, 0, sizeof(paquet));
        // recoit un paquet
        ssize_t bytesRead =
            recvfrom(sock, paquet, sizeof(paquet), 0,
                     (struct sockaddr *)&client, &clientAddrLen);
        if (bytesRead < 0)
        {
            perror("recvfrom failed");
            close(sock);
            if (output != NULL)
            {
                fclose(output);
            }
            free(Chunks);
            free(chunkRecu);
            return 1;
        }
        // convertit ICMP en ECHO REPLYS au client
        ipHeader = (struct IP *)paquet;
        icmpHeader = (struct ICMP *)(paquet + (ipHeader->ip_hl << 2));

        if (icmpHeader->icmp_type == ICMP_ECHO)
        {
            icmpHeader->icmp_type = ICMP_ECHOREPLY;
            icmpHeader->icmp_cksum = 0;

            if (sendto(sock, paquet, bytesRead, 0, (struct sockaddr *)&client,
                       sizeof(client))
                <= 0)
            {
                perror("sendto failed");
                close(sock);
                return 1;
            }

            printf("ICMP echo reply sent to %s\n", inet_ntoa(client.sin_addr));

            unsigned int *cid = (unsigned int *)(paquet + (ipHeader->ip_hl << 2)
                                                 + ICMP_HEADER_SIZE);
            unsigned int *tid =
                (unsigned int *)(paquet + (ipHeader->ip_hl << 2)
                                 + ICMP_HEADER_SIZE + sizeof(unsigned int));
            unsigned int *num = (unsigned int *)(paquet + (ipHeader->ip_hl << 2)
                                                 + ICMP_HEADER_SIZE
                                                 + (2 * sizeof(unsigned int)));
            unsigned int *total =
                (unsigned int *)(paquet + (ipHeader->ip_hl << 2)
                                 + ICMP_HEADER_SIZE
                                 + (3 * sizeof(unsigned int)));
            // verifie si le num du fragment est ok
            if (*num >= *total)
            {
                printf("Invalid fragment number: CID=%u, TID=%u, Fragment=%u, "
                       "Total=%u\n",
                       *cid, *tid, *num, *total);
                continue;
            }
            // calcule taille charge utile
            unsigned int payloadSize =
                bytesRead - (ICMP_HEADER_SIZE + (4 * sizeof(unsigned int)));

            unsigned char *payload = (unsigned char *)paquet
                + (ipHeader->ip_hl << 2) + ICMP_HEADER_SIZE
                + (4 * sizeof(unsigned int));

            // initialise les chunks si y a besoin
            if (totalChunkssave == -1)
            {
                totalChunkssave = *total;
                Chunks = malloc(totalChunkssave * PAYLOAD_SIZE);
                chunkRecu = calloc(totalChunkssave, sizeof(int));
                if (Chunks == NULL || chunkRecu == NULL)
                {
                    perror("Memore non allouée :)");
                    close(sock);
                    if (output != NULL)
                    {
                        fclose(output);
                    }
                    free(Chunks);
                    free(chunkRecu);
                    return 1;
                }
            }
            // Si le chunk n a pas ete recu celle ci copie la charge utile dans
            // Chunks
            if (chunkRecu[*num] == 0)
                if (chunkRecu[*num] == 0)
                {
                    memcpy(Chunks + (*num * PAYLOAD_SIZE), payload,
                           payloadSize);
                    chunkRecu[*num] = 1;
                    totalChunksRecu++;
                    printf("Received chunk %d of %d\n", *num + 1,
                           totalChunkssave);
                }

            if (totalChunksRecu == totalChunkssave)
            {
                char timestamp[20];
                getTimestamp(timestamp, sizeof(timestamp));

                char dossier[50];
                snprintf(dossier, sizeof(dossier), "%u", *cid);

                if (mkdir(dossier, 0777) == -1)
                {
                    if (errno != EEXIST)
                    {
                        perror("Folder creation failed");
                        close(sock);
                        free(Chunks);
                        free(chunkRecu);
                        return 1;
                    }
                }

                char filename[100];
                snprintf(filename, sizeof(filename), "%s/output_file_%s_%u_%u",
                         dossier, timestamp, *cid, *tid);
                output = fopen(filename, "wb");
                if (output == NULL)
                {
                    perror("File open failed");
                    close(sock);
                    free(Chunks);
                    free(chunkRecu);
                    return 1;
                }

                fwrite(Chunks, 1, totalChunkssave * PAYLOAD_SIZE, output);

                printf("File received successfully\n");
                fclose(output);

                close(sock);
                free(Chunks);
                free(chunkRecu);
                return 0;
            }
        }
    }
}
