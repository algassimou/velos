#ifndef __DOWNLOAD__
#define __DOWNLOAD__ 

#include <stdio.h>
#include <semaphore.h>
#include "liste.h"

#define BLOCK_CONTRAT_SIZE 1024

typedef struct {
  FILE * f ;
  Liste * liste_ecriture ;
  sem_t * sem_ecr ;
  Liste * liste_envoi ;
  sem_t * sem_ev ;
} Param_Envoi ;

#define PARAM_ENVOI_FILE(pe) ((pe)->f)
#define PARAM_ENVOI_LISTE_ECR(pe) ((pe)->liste_ecriture)
#define PARAM_ENVOI_LISTE_ENVOI(pe) ((pe)->liste_envoi)
#define PARAM_ENVOI_SEM_ECR(pe) ((pe)->sem_ecr)
#define PARAM_ENVOI_SEM_EV(pe) ((pe)->sem_ev)

typedef struct {
  FILE * f ;
  Liste * liste_ecriture ;
  sem_t * sem_ecr ;
  Liste * liste_transfert ;
  sem_t * sem_trs ;
  Liste * liste_reception ;
  sem_t * sem_rcp ;
} Param_Reception ;

#define PARAM_RECEPTION_FILE(pr) ((pr)->f)
#define PARAM_RECEPTION_LISTE_TRANSFERT(pr) ((pr)->liste_transfert)
#define PARAM_RECEPTION_LISTE_ECR(pr) ((pr)->liste_ecriture)
#define PARAM_RECEPTION_LISTE_RECEP(pr) ((pr)->liste_reception)
#define PARAM_RECEPTION_SEM_ECR(pr) ((pr)->sem_ecr)
#define PARAM_RECEPTION_SEM_TSR(pr) ((pr)->sem_trs)
#define PARAM_RECEPTION_SEM_RCP(pr) ((pr)->sem_rcp)

void * envoi (void *);
void * reception (void *);

#define DROIT 0644

#endif /*__DOWNLOAD__*/
