#ifndef __SERVEUR__
#define __SERVEUR__

#include "liste.h"
#include "dliste.h"
#include "frame.h"
#include <pthread.h>
#include <semaphore.h>

typedef struct {
  int sock ;
  Liste *lecr , *ldwl ;
  pthread_t *lec , *ecr ;		/* thread lecture ecriture */
  sem_t sem_dwl;
  sem_t sem_ecr ;
  DListElem * elt ;
  char * adr ;
  int port ;
} Comm ;

#define COMM_SOCKET(c) ((c)->sock)
#define COMM_LISTE_ECR(c) ((c)->lecr)
#define COMM_LISTE_DWL(c) ((c)->ldwl)
#define COMM_THREAD_LECTURE(c) ((c)->lec)
#define COMM_THREAD_ECRITURE(c) ((c)->ecr)
#define COMM_SEM_DWL(c) ((c)->sem_dwl)
#define COMM_SEM_ECR(c) ((c)->sem_ecr)
#define COMM_DLISTE_ELEM(c) ((c)->elt)
#define COMM_ADR(c) ((c)->adr)
#define COMM_PRT(c) ((c)->port)

extern DListe * dliste_connexion ;
extern Liste * liste_deconnexion , * liste_velos ;
extern int port , nb_velo , nb_client ;
extern int val_retour_lecture ;
extern int val_retour_ecriture ;
extern int identifiant ;
extern sem_t sem_log , sem_trt , sem_deconnexion ;

extern int convert_string_int (char *);
extern int get_line_cmd (int , char ** , const char *);
extern void * connexion (void *);
extern void * deconnexion (void *);

#define PORT 50000
#define NB_VELO 100
#define NB_CLIENT 10
#define LINE_OPTION "p:v:n:" 
#define HELP "serveur options \n" \
  "-p port : port d'ecoute des parks \n" \
  "-v nb : nombre de velos total \n" \
  "-n nb : nombre de client accepte " 


#define SERVEUR_CONTRAT "serveur_contrat"

#define DROIT 0644

#endif 
