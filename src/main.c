
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icmp_server.c"
#include "icmp_client.c"


void printHelp()
{
  printf("-m, --service                        run the binary as a admin "
         "(server), it's the default value\n");
  printf("-s, --client <SERVER IP>             run the binary as a client"
         "(client)\n");
  printf("-f, --file <file path>              Only for slave: specify the data "
         "file\n");
  printf("                                    Incompatible with -d, "
         "--directory\n");
  printf("-d, --directory <directory path>    Only for slave: specify a "
         "directory, each file in the specified directory must be extracted "
         "(not recursively)\n");
  printf("                                    Incompatible with -f, --file\n");
}

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    printf("Syntax Error: le script doit avoir au moins un arguemnt ! See help "
           "(--help or -h) for usage.\n");
  }
  else if (argc >= 2)
  {
    // récupère l'argument principal
    const char *arg = argv[1];

    if ((strcmp(arg, "--help") == 0) || (strcmp(arg, "-h") == 0))
    {
      // affiche l'aide
      printHelp();
    }
    else if (strcmp(arg, "-m") == 0 || strcmp(arg, "--master") == 0)
    {
      // exécute le mode MASTER
      int result = icmp_server();
      if (result == 0)
      {
        printf("Le mode maître s'est exécuté avec succès.\n");
      }
      else
      {
        printf("Erreur lors de l'exécution du mode maître.\n");
      }
    }
    else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--slave") == 0)
    {
      // vérifie les arguments pour le mode SLAVE
      if (argc >= 3)
      {
        const char *serverIP = argv[2];
        if (argc >= 4
            && (strcmp(argv[3], "-f") == 0 || strcmp(argv[3], "--file") == 0))
        {
          if (argc != 5)
          {
            printf("Erreur il manque le chemin du fichier\n");
            return -1;
          }
          const char *fileName = argv[4];
          int result = icmp_client(serverIP, fileName);
          if (result == 0)
          {
            printf("Le mode slave s'est exécuté avec succès.\n");
          }
          else
          {
            printf("Erreur lors de l'exécution du mode slave.\n");
          }
        }
        else if (argc >= 4
                 && (strcmp(argv[3], "-d") == 0
                     || strcmp(argv[3], "--directory") == 0))
        {
          if (argc != 5)
          {
            printf("Erreur il manque le chemin du dossier\n");
            return -1;
          }
          const char *fileName = argv[4];
          int result = icmp_client(serverIP, fileName);
          if (result == 0)
          {
            printf("Le mode slave s'est exécuté avec succès.\n");
          }
          else
          {
            printf("Erreur lors de l'exécution du mode slave.\n");
          }
        }
        else
        {
          printf("Erreur lors de l'exécution. Il manque option -f\n");
        }
      }
      else
      {
        printf("Veuillez spécifier l'adresse IP du maître.\n");
      }
    }
    else
    {
      printf("Argument invalide.\n");
    }
  }

  return 0;
}
