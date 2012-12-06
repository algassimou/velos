#ifndef __CLIENT__
#define __CLIENT__

#include "liste.h"
#include <pthread.h>

extern int get_line_cmd (int , char ** , const char *);
extern int convert_string_int(char *);


#define ADDR_PARK "localhost"
#define PORT 5000 
#define TEMPS 10
#define FIXE 0
#define LINE_OPTION "hp:a:t:f"
#define HELP "client options \n" \
  "-a addr : adresse du park emprunt \n" \
  "-p port : port d'ecoute du park \n" \
  "-t tps : temps de location " \
  " -f  : choisir le park de retour du velo (sinon aleatoire) "

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
extern int port , temps , fixe  ;
extern char * adresse_park ;

extern int creer_socket (char * , int ) ;
extern void * traitement (void *);
extern int creer_connection (char * , int  , Liste * , sem_t *);

/*extern Liste *liste_traitement , *liste_ecriture ;
extern sem_t sem_trt , sem_ep , sem_depart , sem_dwl ;
extern pthread_t lec , ecr , trt , lg , dwl ;
extern int sock_park , sock_srv ;*/

#define FILE_GENERAL "client_general"
#define FILE_CONNEXION "client_connexion"
#define FILE_STAT "client_stat"
#define CLIENT_CONTRAT "client_contrat"

extern Liste *file_reception , *file_ecriture ;
extern sem_t sem_log , sem_ecr , sem_trt , sem_rcp ;
extern int choix ;
extern pthread_t lec , ecr , trt , lg , recep ;

#endif 
