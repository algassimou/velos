#ifndef __PARK__
#define __PARK__


#define PORT_SERVEUR 50000
#define PORT_CLIENT 5000 
#define ADDR_SERVEUR "localhost"
#define SECURITY 0
#define LINE_OPTION "c:s:a:xv:i:h"
#define NB_VELO 6

#include <pthread.h>
#include <semaphore.h>
#include "liste.h"

#define HELP "Usage park options \n \
-a addr : adresse serveur \n \
-s port : port serveur \n \
-c port : port ecoute client \n \
-x : connection securise \n \
-i id : identifiant du park \n \
-v nb : nombre de velo initial demande"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

extern int port_client , port_serveur , nb_velo , id , securise , sock_cli , sock_srv;
extern char * adresse_serveur ;
extern char cle ;

int get_line_cmd (int , char ** , const char *);
extern int convert_string_int (char *);
extern int creer_socket_serveur() ;

extern int deconnexion_serveur (int );
extern int connection_serveur () ;

extern Liste *file_reception , *file_traitement , *file_ecriture_client ;
extern Liste *file_ecriture_serveur , *file_envoi , *liste_velos ;
extern sem_t sem_log , sem_ecr_srv, sem_ecr_cli, sem_trt, sem_rcp, sem_ev;
extern sem_t sem_accept_connexion  ;
extern pthread_t lec_srv , ecr_srv , lec_cli , ecr_cli , trt , lg , recep , env ;

#define FILE_GENERAL "park_general"
#define FILE_CONNEXION "park_connexion"
#define FILE_STAT "park_stat"
#define PARK_CONTRAT "park_contrat"

#endif  /*__PARK__*/
