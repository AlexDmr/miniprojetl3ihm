#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ctype.h>
#include <netdb.h>
#include <signal.h>

#include "FromXMLToGoogleMapHTTPRequest.h"
#include "FromGoogleMapXMLToDistanceTable.h"
#include "SortVisits.h"
#include <fstream>
#include <curl/curl.h>


/* Pour projet d'application integrateur L3 Miage - 2015/2016                       */
/* Application "infirmiere"                                                         */
/* Serveur metier (recherche et optim. matrice de distances) - L.Pierre, juin 2015  */

/* NB. Reutilisation de code en provenance de www-proxy.c : 
   Copyright Imperial Business Systems 1995 - Proxy http server.
   Written by Steve Shipway (steve@cheshire.demon.co.uk,steve@imperial.co.uk)
   in the space of 2 hours.  It took a further 1 hour to remove the last bug.  
   And 2 hours to persuade myself to stop playing with Netscape after it
   was working.
   Command line args:
     -d : debug mode.  Use port 8000 instead, dont background, status msgs
     -l logfile  : logfile to use
     -p port : specify different port number
The port to use is: specified on command line, or http-proxy in /etc/services, 
or 800. The destination default port is http in /etc/services, or 80.

This is provided as-is, with no warranty, expressed or implied.
This source code remains the property of imperial business systems
(ibs@imperial.co.uk)
Not to be distributed for profit, or indeed at all, without permission.
***********/

/* CONTEXTE de l'application :                                                 */
/*                                                                             */
/* client (navigateur) <--> serveur HTTP (NodeJS) <--> serveur metier          */
/*                                                       \                     */
/*                                                        \__> GoogleMaps API  */


// Numero de port pour le serveur HTTP 
#define PORT_HTTP 8080
// Nb max de requetes a faire au serveur Google Distance Matrix (i.e. nb max 
// de deplacements indiques par le serveur HTTP)
#define MAXREQ 50
// Numero de port pour le serveur Google Distance Matrix
#define PORT_GOOGLE 80
// Taille du buffer de reception du fichier database XML
#define SIZEBUFF 65535


#define NONE (fd_set *) NULL
#define NEVER (struct timeval *) NULL
#define IGNORE (struct sockaddr *) NULL

int debug = 0;
char buffer[SIZEBUFF];
int httpport;
int s;
struct sockaddr_in sc_in;
FILE *lfp = NULL;
char **info;
// Par defaut, le serveur NodeJS est sur la meme machine :
char *nomServeurNodeJS = "localhost";
// Fichier contenant la base venue du serveur NodeJS
char *nomFichierDataBase = "FichierDataBase.xml";
// Fichier contenant la matrice de distances
char *nomFichierMatrice = "FichierMatrice.xml";
// Fichier resultat, a renvoyer au client
char *nomFichierRes = "FichierRes.html";


/**************************  Fonctions annexes *********************************/

// Reinitialisation du buffer de lecture
void init_buffer(){
  int i; 
  for (i=0; i<SIZEBUFF; i++)
    buffer[i] = '\0';
}

// Envoi d'une page web au client (navigateur), pour les messages d'erreur 
void fdsend(int fd,char *title, char *head, char *body){
  sprintf(buffer,"<HEAD><TITLE>HTTP Proxy Server Message</TITLE></HEAD>\n<BODY>\n");
  write(fd,buffer,strlen(buffer));
  if(title) {
    sprintf(buffer,"<H1>%s</H1>\n",title);
    write(fd,buffer,strlen(buffer));
  }
  if(head) {
    sprintf(buffer,"<H2>%s</H2>\n",head);
    write(fd,buffer,strlen(buffer));
  }
  if(body) {
    sprintf(buffer,"%s<P>\n",body);
    write(fd,buffer,strlen(buffer));
  }
  sprintf(buffer,"</BODY>\n");
  write(fd,buffer,strlen(buffer));
}

// Gestion d'erreur lors de l'identification du serveur distant
void host_error(char host[128], char buf2[512], int fd){ 
  if(debug) printf("Host %s not found\n",host);
  if(lfp){ fprintf(lfp,"Host %s was not found.\n",host);
    fflush(lfp); }	
  switch(h_errno) {
  case HOST_NOT_FOUND:
    sprintf(buf2,"Sorry, this host could not be identified by the DNS server.\nMaybe the address is wrong or mistyped.");
    break;
  case TRY_AGAIN:
    sprintf(buf2,"Sorry, there is a temporary problem with the DNS server.  Please try again.");
    break;
  case NO_DATA:
    sprintf(buf2,"Sorry, this is a virtual host without an IP address.");
    break;
  default:
    sprintf(buf2,"Unable to resolve this host name to an IP address.");
  }
  fdsend(fd,"Sorry!",host,buf2);
  close(fd);
  if(lfp) {
    fprintf(lfp,"** Host %s not found.\n",host);
    fflush(lfp); 
  }
}

// Gestion d'erreur lors de la tentative de connexion au serveur distant
void connect_error(char buf2[512]){
  switch(errno) {
  case ECONNREFUSED:
    sprintf(buf2,"Sorry, this host is refusing connections from this machine on the specified port.  This is possibly due to the host not running a Web server, or an incorrect port number being specified.\n");
    break;
  case EHOSTUNREACH:
  case ENETUNREACH:
    sprintf(buf2,"Sorry, this host is currently unreachable from this network.\n");
    break;
  case EHOSTDOWN:
  case ENETDOWN:
    sprintf(buf2,"Sorry, this host is down.  Try again later.\n");
    break;
  case ETIMEDOUT:
    sprintf(buf2,"Sorry, the connection to this host was timed out.  This is most likely due to network overload.  Try again later.\n");
    break;
  default:
    sprintf(buf2,"Sorry, this host is not responding.  This may be due to either the remote host being down, to 'netlag' (slow connections), or to an incorrect port number in the URL.");
  }
}


/************  Fonction pour recuperer les reponses du serveur HTTP *************/ 

// Fonction qui recupere les infos en provenance du serveur HTTP (fichier XML url-encod'e) et
// elle les sauve dans un fichier "FichierDataBase.xml" decod'e (que la fonction parseDocument 
// de Celine parsera)
// index_debut = index (dans le buffer) du caractere du debut du fichier XML (entete de la 
//               requete sautee)
// nbcar = nb de caracteres dans le buffer 
void httpGet(int index_debut, int nbcar, char *nomFichierDataBase){ 
  int dest;  // descripteur fichier destination
  char bufferdest[SIZEBUFF];
  char *tampon;
  int len=0;  // longueur de la chaine decodee
  printf("**On recoit du serveur HTTP et on cree un fichier FichierDataBase.xml ... \n");
  if ((dest=open(nomFichierDataBase, O_WRONLY | O_CREAT, 0644)) != -1) {  
    // 1. recopie du morceau de buffer apres index_debut dans bufferdest
    buffer[nbcar]='\0';  // ajout '\0' en fin de buffer pour avoir un vrai char*
    strcpy(bufferdest,buffer+index_debut+1);
    // 2. decodage du buffer (qui etait url-encode) dans tampon
    tampon=curl_easy_unescape(NULL, bufferdest, nbcar-index_debut, &len);
    // 3. ecriture de tampon (sauf le \0) dans le fichier destination
    write(dest, tampon, len-1);   // on ecrit len-1 caracteres pour ne pas ecrire le \0
    close(dest);
  }
  else 
    fprintf(stderr, "Impossible de creer le fichier XML");
}

/**********  Fonction pour construire la requete a Google Maps distance matrix *********/

// Fonction reprise, avec les primitives de Celine - Elle utilise le resultat produit 
// par getGoogleMapHttpRequest_V2
void set_header_request_distmatrix(int id, char req[1024]){
  char numero[20];
  sprintf(numero, "%d", id);
  std::string s(numero);
  printf("-----> set header \n");
  FromXMLToGoogleMapHTTPRequest dataBaseParser;

  //sprintf(req, "GET /maps/api/distancematrix/xml?sensor=false&mode=driving&units=metric&%s HTTP/1.1\n", 
  //	  (dataBaseParser.getGoogleMapHttpRequest_V2()).c_str());
  sprintf(req, "GET http://maps.googleapis.com/maps/api/distancematrix/xml?sensor=false&mode=driving&units=metric&%s HTTP/1.1\n", 
  	  (dataBaseParser.getGoogleHttpRequest(nomFichierDataBase,id)));
  //sprintf(req, "%s%s\r\n", req, "Host: maps.googleapis.com\n");
  sprintf(req, "%s%s\r\n", req, "Accept: text/html,application/xhtml+xml,application/xml\n");
  sprintf(req, "%s%s\r\n", req, "Accept-Language: fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3-us\n");
  sprintf(req, "%s%s\r\n", req, "Connection: keep-alive");
  printf("<----- set header \n");
}

/************  Fonction pour recuperer la reponse de GoogleMapAPI  *************/ 

// Pour la lecture d'une ligne dans la socket 
// (http://man7.org/tlpi/code/online/dist/sockets/read_line.c.html)
/* Read characters from 'fd' until a newline is encountered. If a newline
  character is not encountered in the first (n - 1) bytes, then the excess
  characters are discarded. The returned string placed in 'buf' is
  null-terminated and includes the newline character if it was read in the
  first (n - 1) bytes. The function return value is the number of bytes
  placed in buffer (which includes the newline character if encountered,
  but excludes the terminating null byte). */
ssize_t readLine(int fd, char *buffer, size_t n) {
  ssize_t numRead;                    /* # of bytes fetched by last read() */
  size_t totRead;                     /* Total bytes read so far */
  char *buf;
  char ch;

  //printf("----> readline \n");   
  if (n <= 0 || buffer == NULL) {
    errno = EINVAL;
    return -1;
  }
  buf = buffer;                   /* No pointer arithmetic on "void *" (????!!!) */
  totRead = 0;
  for (;;) {
    numRead = read(fd, &ch, 1);
    if (numRead == -1) {
      if (errno == EINTR)         /* Interrupted --> restart read() */
	continue;
      else
	return -1;              /* Some other error */
    } else if (numRead == 0) {      /* EOF */
      if (totRead == 0)           /* No bytes read; return 0 */
	return 0;
      else                        /* Some bytes read; add '\0' */
	break;
    } else {                        /* 'numRead' must be 1 if we get here */
      if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
	totRead++;
	*buf++ = ch;
      }
      if (ch == '\n')
	break;
    }
  }
  *buf = '\0';
  //printf("<---- readline %d \n", totRead);   
  return totRead;
}

/**********  Fonctions pour traiter la reponse de Google Maps distance matrix *********/

// Fonction de recuperation de la matrice de distance aupres de GoogleMap 
// qui est placee dans FichierMatrice.xml
int getDistanceMatrix (char *nomFichierMatrice, int mat_fd){ 
  int n, entete, fin;
  //printf(" <<<<<<<< FIN <<<<<<<<<<< \n");  
  int ficmat;
  char tampon_rep[1025];
  init_buffer();
  // lecture de la reponse de Google matrix, et mise dans un fichier "FichierMatrice.xml"
  if ((ficmat=open(nomFichierMatrice, O_WRONLY | O_CREAT, 0644)) != -1) { 
    /*
      while ( (n=read(mat_fd, tampon, 512)) != 0) {
      write(ficmat, tampon, n);
      }
    */
    /* Update 25/11/2014 : */
    entete = 1;
    fin = 0;
    while ( (n=readLine(mat_fd, tampon_rep, 1024)) != 0) {   // lecture d'une ligne
      if (entete) {
	if (strstr(tampon_rep, "<?xml") == tampon_rep){   // on arrive dans le xml, il faut 
	  entete = 0;                                     // maintenant garder les lignes
	  write(ficmat, tampon_rep, n);
	}
      }
      else {      // on n'est plus dans l'entete, on recopie le texte qui est bon
	if (!fin) {
	  write(ficmat, tampon_rep, n);
	  if (strstr(tampon_rep, "</DistanceMatrixResponse>") == tampon_rep) fin = 1;
	}
      }
    }    
    close(ficmat);
    return 1;
  }
  else return 0;
}

// Fonction de traitement resultant du code de Celine (elle place tout dans 
// un fichier, qu'on va faire passer au client) 
void process_matrix (char *nomficres, int id){ 
  SortVisits sorter;
  sorter.processDistanceMatrix(nomFichierDataBase, nomFichierMatrice, id, "data/cabinetToInfirmier.xsl", nomficres);
/*   FromGoogleMapXMLToDistanceTable googleMapParser; */
/*   std::vector<std::string> adresses; */
/*   std::vector< std::vector<int> > distances; */
/*   std::ofstream f;  */

/*   f.open(nomficres); */
/*   adresses = googleMapParser.getAdresses(); */
/*   distances = googleMapParser.getDistances(); */
/*   sorter.modifyFile("char * filename", adresses); */
/*   sorter.saveXHTMLFile("data/cabinetInfirmier.xml", nomficres, id); */
}

// Fonction pour la recuperation de la matrice de distances, et l'exploitation
// de cette information (par fcts Celine et Quentin)
void get_and_process_matrix(int id, int fd){ 
  int n;
  char request_matrix[1024], buf2[512];
  char tampon[512];
  int mat_fd, ficres; 
  //  char host[128]="maps.googleapis.com";
  char host[128] = "www-cache.ujf-grenoble.fr";   // a cause du proxy de l'UFR
  int portnum;
  struct sockaddr_in sin;
  struct hostent *shes2;
 
 /* Preparation of the connection */
  if(debug) printf("gethostbyname(%s)=",host);
  shes2 = gethostbyname(host);
  if(debug) {
    if(!shes2) {
      printf("null\n");
    }else{
      printf("%d.%d.%d.%d\n",
	     (unsigned char)shes2->h_addr[0],
	     (unsigned char)shes2->h_addr[1],
	     (unsigned char)shes2->h_addr[2],
	     (unsigned char)shes2->h_addr[3]
	     );
    }
  }
  if(!shes2) {
    host_error(host, buf2, fd); 
    return;
  }
  //  portnum=PORT_GOOGLE;
  portnum=3128;       // a cause du proxy de l'UFR

  if(debug) printf("Trying %s on port %d...\n",host,portnum);
  sin.sin_family = AF_INET;
  bcopy(shes2->h_addr,&sin.sin_addr,sizeof(struct in_addr)) ;
  sin.sin_port = htons((u_short) portnum) ;

  /* Get answers from Google Distance Matrix and process them */
  set_header_request_distmatrix(id, request_matrix);

  printf("+++++++++++++ ma requete +++++++++++++++ \n");
  printf("%s\n", request_matrix);
  printf("++++++++++++++++++++++++ \n");
  if ((mat_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    if(debug) perror("socket");
    if(lfp) fprintf(lfp,"*** socket() failed in child process. ***\n");
    return;
  }
  if(debug>2) printf("mat_fd is on fd %d\n",mat_fd);    
  if(connect(mat_fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) < 0) {
    if(debug) perror("connect");
    if(lfp) { fprintf(lfp,"Connect failed to host %s [%d]\n",host,errno);
      fflush(lfp); }
    connect_error(buf2);
    fdsend(fd,"Sorry!",host,buf2);
    close(fd);
    return;
  }
  if(debug) printf("* Connected to %s on port %d\n",host,portnum);
  if(lfp) { 
    fprintf(lfp,"Connected to %s:%d\n",host,portnum);	
    fflush(lfp);
  }
  printf(" >>> Je contacte %s et j'envoie la requete %s\n", host, request_matrix);  
  if(write(mat_fd,request_matrix,strlen(request_matrix))<1) {
    perror("write");
    close(fd);
    close(mat_fd);
    return;
  }
  else {
    if (getDistanceMatrix (nomFichierMatrice, mat_fd)){  // recuperation de la matrice
      // traitement 
      process_matrix(nomFichierRes, id);
      // envoi au client
      printf(" >>> Envoi final de la reponse au client...\n\n");
      if ((ficres=open(nomFichierRes, O_RDONLY)) != -1) {
	std::string s = "HTTP/1.0 200 OK\nContent-Type: text/html\n\n";
	write(fd, s.c_str(), s.size());
	while ( (n=read(ficres, tampon, 512)) != 0) {
	  write(fd, tampon, n);
	}
	close(ficres);
      }
      else
	fprintf(stderr, "Fichier de resultat inexistant !! \n");
    }
    else 
      fprintf(stderr, "Impossible de creer le fichier reponse de Google");
  }
}


/***********  Fonction pour le traitement de la requete de l'infirmiere  ************/

// Reception du fichier XML par le serveur HTTP (lecture dans "buffer", des indices
// current_index a nbcar), copie dans nomFichierDataBase, puis envoi de la requete 
// a GoogleMapAPI
void process_nurse_request(int current_index, int nbcar, int fd, int id){
  printf("Traitement d'une requete de l'infirmiere %d...\n", id); 
  /* -- 1 -- Connect to server HTTP and get answers  (c'etait l'ancienne version !) */
  //connect_HTTP(fd, id, name_serv_http);
  /* -- 1 -- Get file from server HTTP */
  printf(" >>> Reception du fichier XML par le serveur HTTP <<< \n"); 
  httpGet(current_index, nbcar, nomFichierDataBase);
  /* -- 2 -- Connect to server Google Distance Matrix */
  get_and_process_matrix(id,fd); 
}

/***************************  Fonctions du serveur ****************************/

// Fonction pour la lecture d'une ligne entiere dans la requete (placee 
// dans la variable globale "buffer"):
int get_next_line(int current_index, char line[120]){
  int i=current_index, j=0;
  while (buffer[i]!='\r' && buffer[i]!='\n' && buffer[i]!='\0' && buffer[i]!='&'){
    line[j++]=(char)toupper((int)buffer[i++]);
  } 
  line[j]='\0';
  return i+1;
}

/* 
La requete en provenance du serveur HTTP a la forme suivante: 
buffer = POST /INFIRMIERE HTTP/1.1
Host: mandelbrot.e.ujf-grenoble.fr:8000
Content-Type: application/x-www-form-urlencoded
Content-Length: 13455
Via: 1.1 www-cache-1 (squid/3.4.12)
X-Forwarded-For: 152.77.81.20
Cache-Control: max-age=259200
Connection: keep-alive

id=002&xml=%3C%3Fxml%20version%3D%221.0%22%20encoding%3D%22UTF-8%22%3F%3E%3Ccabi...

Donc : on lit jusqu'a trouver "id=", on lit alors l'id sur 3 caracteres, puis on 
saute les caracteres "&xml=" et on prend ce qui reste (c'est le fichier XML 
url-encod'e envoy'e par le serveur HTTP)
*/

// Traitement des requetes arrivant du client (sur la socket fd).     //
// Il y a 2 cas : 
//   - la requete est invalide 
//   - la requete commence par "POST /INFIRMIERE HTTP/1.1" et on joue le 
//     role de serveur metier (appel de la fonction process_nurse_request) 

int fdserve(int fd){
  char line[120], action[150];
  int n;
  extern int h_errno;
  char cmd[16], str_id[10];
  int i,j,current_index,id;
  int nurse=0;

  if(debug) printf("* Spawning **************************\n");
  init_buffer();
  // lecture de la requete, placee dans buffer:
  n=read(fd,buffer,sizeof(buffer));
  if(n<1) {
    if(lfp) fprintf(lfp,"*** read() failed in child process.***\n");
    if(debug) {
      if(n<0) perror("read");
      printf("Nothing read.\n");
    }
    return -1;
  }
  // recuperation du nom de la commande:
  for(i=0;i<15;i++)
    if(buffer[i] && (buffer[i]!=' ')) {
      cmd[i]=(char)toupper((int)buffer[i]);
    } else break;
  cmd[i]='\0';
  printf("COMMANDE = %s\n", cmd);

  // Si commande invalide:
  if (strcmp(cmd,"GET")&&strcmp(cmd,"POST"))  {
    if(debug) printf("Not a GET or POST command\n");
    if(lfp) { fprintf(lfp,"Bad command [%s] received.\n---------\n",cmd);
      fprintf(lfp,"%s\n----------\n",buffer);fflush(lfp); }
    fdsend(fd,"Error 405",cmd,"This command is not supported by the proxy server.");
    close(fd);
    return -1;
  }
  else 
    // Si commande POST, on gere le cas de "POST /INFIRMIERE HTTP/1.1":
    if (!strcmp(cmd,"POST")) {
      // lecture de l'action derriere "POST":
      for(j=0,i++;i<150;i++)
	if(buffer[i] && (buffer[i]!=' ')) {
	  action[j++]=(char)toupper((int)buffer[i]);
	} else break;
      action[j]='\0';
      // recherche de la chaine /INFIRMIERE:
      if (strstr(action, "/INFIRMIERE") != NULL){     
	nurse=1;
	// lecture de l'id de l'infirmiere (recherche de la ligne qui 
        // commence par id=..) et recuperation de la valeur :
	while (buffer[i]!='\r' && buffer[i++]!='\n') ;
	current_index=get_next_line(i, line);
	while ((strstr(line, "ID=")!=line || !line) && current_index<n){	
	  current_index=get_next_line(current_index, line);
	}
	if (strstr(line, "ID=")==line){
	  strncpy(str_id, line+3, 10);    // id sous forme de chaine de car
	  id = atoi(str_id);              // id converti en entier
	  // On saute les caracteres "xml=" (ce qui sera derriere est le fichier XML)
	  current_index+=3;   
	  // On traite ce qui reste dans le buffer (des indices current_index a n)
	  process_nurse_request(current_index,n,fd,id);
	}
	else 
	  fdsend(fd,"Error ", "Nurse id", "No nurse id in your request.");
      }
    }
  if (!nurse){   // Dans tous les autres cas, erreur !!
    fprintf(stderr, "Unable to process other requests...\n");
    fdsend(fd,"Error 405",cmd,"This command is not supported by the proxy server.");
    return -1;
  }
  close(fd);
  return 0;
}

void finfils(int sig){
  wait(NULL);
  printf("Child finished !!\n");
}

/********************************  main  ******************************/

int main(int argc, char *argv[]){
  int j;
  int children= 0;
  extern char *optarg;
  int c,port;
  char statbuf[32];
  struct servent *sss;
  char *logfile=(char *)0;
  int last;

  struct sigaction action;
  action.sa_handler = finfils;
  action.sa_flags = SA_RESTART;  
  sigaction(SIGCHLD, &action, NULL);

  info=argv+1;
  port=0;
  while((c=getopt(argc,argv,"dp:l:"))!=-1) 
    switch(c) {
    case 'd': debug++; break;
    case 'p': port=atoi(optarg); break;
    case 'l': logfile = optarg; break;
    default: printf("Usage: %s [-d] [-p port] [-l logfile]\n",argv[0]);
      exit(1);
    }
  if(logfile) {
    lfp=fopen(logfile,"a");
    if(!lfp) perror(logfile);
  }
  if(!port) {
    fprintf(stderr, "Missing port!...\n");
    exit(1);
  }

  // Recherche infos sur HTTP dans /etc/services :
  sss = getservbyname("http","tcp");
  httpport=(sss?(u_short)htons(sss->s_port):80);     // Verifier ce que ca fait exactement !!!!!!!
  if(debug) printf("Using http port [%-10.10s] %d\n",sss->s_name,httpport);

  /* open port to listen to */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    if(debug) perror("socket");
    return 0;
  }
  sc_in.sin_family = AF_INET;
  sc_in.sin_addr.s_addr = htonl(INADDR_ANY);
  sc_in.sin_port = htons((u_short) port);

  /* now try and set the socket option to allow reuse of address */
  {
    int b;
    b=-1;
    if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&b,4)) {
      perror("setsockopt");
      if(lfp) fprintf(lfp,"setsockopt failed.\n");
      exit(1);
    }
  }

  if (bind(s, (struct sockaddr *) &sc_in, sizeof(sc_in)) < 0) {
    perror("bind: proxy");
    if(lfp) fprintf(lfp,"bind failed.\n");
    return 0;
  }
  if (listen(s, 25) < 0) {
    perror("listen");
    return 0;
  }

  if(lfp) { fprintf(lfp,"*** Program starting.\n"); fflush(lfp); }
  /* SELECT LOOP */
  /* we loop until something happens! */
  children=0;
  last = s;

  for( ;; ) {
    sprintf(statbuf,"port %d, %d procs, last=%d" ,port,children,last);
    if(debug) printf("** Waiting..\n");
    if(debug) printf("** Selecting..\n");

    if ((j = accept(s, IGNORE, NULL)) < 0) {
      if(debug) perror("accept");
      if(lfp) { fprintf(lfp,"* accept() failed\n"); fflush(lfp);}
    } else {
      last=j;
      children++;
      printf("Accept succeeded !!\n");
      if(!fork()) {
	close(s);
	fdserve(j);
	if(debug) printf("* Child exiting.\n");
	exit(0);
      }
      close(j);
    }
  }
  if(debug) printf("** Bye.\n");
  if(lfp) fprintf(lfp,"***** EXITING PROGRAM *****\n");
  exit(0);
}

