#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>

/* Pour projet d'application integrateur L3 Miage - 2014/2015               */
/* Application "infirmiere"                                                 */

/* Rappel - CONTEXTE de l'application :                                     */
/*                                                                          */
/*    client (navigateur)  <-->  proxy  <--> serveur HTTP (NodeJS)          */
/*                                   \                                      */
/*                                    \__> GoogleMaps API                   */

/* Ceci est un minuscule client (pour pouvoir tester le proxy) qui envoie   */
/* une requete de la forme de celle qui sera recue par le proxy dans le cas */
/* d'une interaction avec l'infirmiere (tache 6 sur le dessin).             */
/* La requete commence par "POST /INFIRMIERE HTTP/1.1" et finit par un      */
/* contenu de la forme id=<un entier> qui donne l'id de l'infirmiere.       */
/* Puis il recoit et affiche que ce lui renvoie le proxy (a terme, ce sera  */
/* le resultat de l'algo d'optimisation)                                    */

/* A utiliser comme suit :                                                  */
/*     ./simu_client_req6 localhost 8000 <numero>                           */
/* si on le fait tourner sur la meme machine que le proxy, et si le proxy   */
/* a ete lance avec -p 8000 pour indiquer qu'il attend les requetes client  */
/* sur le port 8000; <numero> est l'id de l'infirmiere.                     */

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define MAX_LIGNE 5000

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
SOCKET sock;

void interceptionKill(int sig){
  close(sock);
  exit(0);
}

// Fabrication et envoi de la requete (pour une infirmiere de numero id)
int envoi_requete_infirmiere(int sock, int id){
  char req[MAX_LIGNE];
  char reponse[MAX_LIGNE];
  int i;
  for (i=0; i<MAX_LIGNE; i++)
    reponse[i] = '\0';
  sprintf(req, "%s\r\n", "POST /INFIRMIERE HTTP/1.1");
  sprintf(req, "%s%s\r\n", req, "Host: localhost:8080");
  sprintf(req, "%s%s\r\n", req, "Connection: keep-alive");
  sprintf(req, "%s%s\r\n", req, "BLA BLA BLA MON BEAU MESSAGE");
  sprintf(req, "%s%s%d\r\n", req, "id=", id);
  if (write(sock, req, strlen(req))==-1)  
    fprintf(stderr, "Erreur de write\n");
  printf("Lecture reponse...\n\n");
  while (read(sock, reponse, MAX_LIGNE-1)>0) {
    printf("%s\n", reponse);
    for (i=0; i<MAX_LIGNE; i++)
      reponse[i] = '\0';
  }
  return 0;
}

// Passer en parametres a la commande le nom du serveur (localhost), le 
// numero de port (8000, si le proxy a ete lance avec -p 8000), et l'id infirmiere
int main(int argc, char** argv, char** arge){
  if(argc != 4){
    printf("Parametres: nom du serveur, numero de port, et id infirmiere.\n");
    exit(-1);
  }
  sock = socket(AF_INET, SOCK_STREAM, 0);  // creation socket
  if(sock == INVALID_SOCKET){
    fprintf (stderr, "Erreur Socket\n");
    exit(EXIT_FAILURE);
  }
  struct hostent *hostinfo = NULL;
  SOCKADDR_IN sin = { 0 }; 
  const char *hostname = argv[1];
  hostinfo = gethostbyname(hostname); 
  if (hostinfo == NULL)
    {
      fprintf (stderr, "Serveur non trouvé\n");
      exit(EXIT_FAILURE);
    }
  sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
  sin.sin_port = htons( atoi(argv[2]) );
  sin.sin_family = AF_INET;
  // connexion au serveur et envoi de la requete : 
  if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
      fprintf (stderr, "Erreur connection\n");
      exit(EXIT_FAILURE);
    }
  signal(SIGINT,interceptionKill);
  envoi_requete_infirmiere(sock, atoi(argv[3]));
  return 0;
}

