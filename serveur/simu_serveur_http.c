#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Pour projet d'application integrateur L3 Miage - 2014/2015               */
/* Application "infirmiere"                                                 */

/* Rappel - CONTEXTE de l'application :                                     */
/*                                                                          */
/*    client (navigateur)  <-->  proxy  <--> serveur HTTP (NodeJS)          */
/*                                   \                                      */
/*                                    \__> GoogleMaps API                   */

/* Ceci est un minuscule simulateur de serveur HTTP (NodeJS), pour pouvoir  */
/* tester le proxy. A utiliser comme suit :                                 */
/*     ./simu_serveur_http 5555                                             */
/* car le proxy suppose qu'il lui parle sur le port 5555.                   */
/* Il repond systematiquement la suite d'adresses suivante (que le proxy    */
/* utilise pour interroger GoogleMaps API) :                                */
/* origins=La+Tronche+38700+Rond-Point+de+la+Croix+de+Vie&destinations=Grenoble+38031+46+avenue+Félix+Viallet
   origins=Grenoble+38031+46+avenue+Félix+Viallet&destinations=Grenoble+38041+60+rue+de+la+Chimie
   origins=Grenoble+38041+60+rue+de+la+Chimie&destinations=Montbonnot-Saint-Martin+38330+655+Avenue+de+l+Europe
*/

#define VAL_INTERDITE -1

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

int mysock;
char buffer[4096];

// interception ctrl-c
void interceptionKill(int sig){
  close(mysock);
  exit(0);
}

/* Deconnecte un client du sereur en fermant la socket du serveur*/
void clientDeconnecter(int sock_client){
  close(sock_client);
  exit(0);
}

// Reinitialisation du buffer de lecture
void init_buffer(){
  int i; 
  for (i=0; i<4096; i++)
    buffer[i] = '\0';
}

/* void send_information(int id, int fd){  */
/*   char answer1[128]="origins=La+Tronche+38700+Rond-Point+de+la+Croix+de+Vie&destinations=Grenoble+38031+46+avenue+Félix+Viallet#"; */
/*   char answer2[128]="origins=Grenoble+38031+46+avenue+Félix+Viallet&destinations=Grenoble+38041+60+rue+de+la+Chimie#"; */
/*   char answer3[128]="origins=Grenoble+38041+60+rue+de+la+Chimie&destinations=Montbonnot-Saint-Martin+38330+655+Avenue+de+l+Europe#$"; */
/*   printf("Requete recue de l'infirmiere %d, je reponds...\n", id); */
/*   write(fd,answer1,strlen(answer1)); */
/*   write(fd,answer2,strlen(answer2)); */
/*   write(fd,answer3,strlen(answer3)); */
/* } */

// Fonction reprise : elle envoie maintenant le fichier exemple de Celine
void send_information(int id, int fd){ 
  int fic, n;
  char tampon[512];
  printf("**On envoie le fichier de nom %s... \n", "XML_process/data/cabinet_output.xml");
  if ((fic=open("XML_process/data/cabinet_output.xml", O_RDONLY)) != -1) {  
    while ( (n=read(fic, tampon, 512)) != 0) {
      write(fd, tampon, n);
    }
    close(fic);
  }
  else 
    fprintf(stderr, "Fichier inexistant !! \n");
}

// Fonction pour la lecture d'une ligne entiere dans la requete (placee 
// dans la variable globale "buffer"):
int get_next_line(int current_index, char line[120]){
  int i=current_index, j=0;
  while (buffer[i]!='\r' && buffer[i]!='\n'){
    line[j++]=(char)toupper((int)buffer[i++]);
  } 
  line[j]='\0';
  return i+1;
}

// Attend une requete d'infirmiere, recupere son numero, et renvoie une reponse (adresses)
int fdserve(int fd){
  char line[120], action[15], cmd[16], str_id[10];
  int n;
  int i,j,current_index,id;
  int nurse=0;

  init_buffer();
  // lecture de la requete, placee dans buffer:
  n=read(fd,buffer,sizeof(buffer));
  if(n<1) {
    if(n<0) perror("read");
    printf("Nothing read.\n");
    return -1;
  }
  // recuperation du nom de la commande:
  for(i=0;i<15;i++)
    if(buffer[i] && (buffer[i]!=' ')) {
      cmd[i]=(char)toupper((int)buffer[i]);
    } else break;
  cmd[i]='\0';
  // Si commande POST, on gere le cas de "POST /INFIRMIERE HTTP/1.1":
  if (!strcmp(cmd,"POST")) {
    // lecture de l'action derriere "POST":
    for(j=0,i++;i<30;i++)
      if(buffer[i] && (buffer[i]!=' ')) {
	action[j++]=(char)toupper((int)buffer[i]);
      } else break;
    action[j]='\0';
    if (!strcmp(action, "/INFIRMIERE")){
      nurse=1;
      // lecture de l'id de l'infirmiere (recherche de la ligne qui 
      // commence par id=..) et recuperation de la valeur :
      while (buffer[i]!='\r' && buffer[i++]!='\n');
      current_index=get_next_line(i, line);
      while (strstr(line, "ID=")!=line && current_index<n){	
	current_index=get_next_line(current_index, line);
      }
      if (strstr(line,"ID=")==line){
	strncpy(str_id, line+3, 10);  // id sous forme de chaine de car
	id = atoi(str_id);            // id converti en entier
	send_information(id, fd);
      }
      else 
	fprintf(stderr,"Error: No nurse id in your request.");
    }
  }
  return 0;
}

void init_socket(int port){
  /* Creation de la socket serveur*/
  mysock = socket(AF_INET, SOCK_STREAM,0);
  if(mysock != VAL_INTERDITE){
    int yes=1; 
    /*creation de la structure de la socket serveur*/
    SOCKADDR_IN sock_struct = { 0 };
    sock_struct.sin_family = AF_INET;
    sock_struct.sin_port = htons(port);
    sock_struct.sin_addr.s_addr = htonl(INADDR_ANY);
    // lose the pesky "Address already in use" error message
    if(setsockopt(mysock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    /*Attachement de la socket serveur*/
    if(bind(mysock, (SOCKADDR *) &sock_struct, sizeof sock_struct) != VAL_INTERDITE){		
      /*Attente de connection*/
      if(listen(mysock,2) != VAL_INTERDITE){
	/*Definition de la socket client*/
	int sock_client;
	SOCKADDR_IN sock_client_struct = { 0 };
	socklen_t len = sizeof sock_client_struct;
	while(1){
	  /*Acceptation de la connexion avec creation de la socket client*/
	  sock_client = accept(mysock, (SOCKADDR *) &sock_client_struct, &len);
	  if(sock_client != VAL_INTERDITE){
	    /*fork avec comportement du pere: boucle sur l'acceptation de connexion
	     * comportement du fils: affiche le menu général*/						
	    switch (fork()){
	    case -1:
	      perror("fork");
	      exit(-2);
	    case 0:
	      fdserve(sock_client);
	      exit(-1);
	    }						
	  }
	  else {
	    perror("socket_client()");
	    exit(errno);
	  }
	  close(sock_client);
	}
      }
      else {
	perror("listen()");
	exit(errno);
      }
    }
    else {
      perror("bind()");
      exit(errno);
    }
  }
  else {
    perror("socket ()");
    exit(errno);
  }
}

int main(int argc, char *argv[], char *arge[]) {
  signal(SIGINT,interceptionKill);
  if(argc != 2){
    printf("Parametre: numero de port.\n");
    exit(-1);
  }
  char* name = malloc(sizeof(char)*30);
  gethostname(name,30);
  printf("Le nom du serveur est : %s\n",name);
 
  init_socket(atoi(argv[1]));
  return 0;
}
