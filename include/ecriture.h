#ifndef __ECRITURE__
#define __ECRITURE__

#include <semaphore.h>
#include "liste.h"

typedef struct {
  int sock ;
  Liste * file_ecriture ;
  sem_t * sem_ecr ;
  sem_t * sem_log ;
} Param_ecriture ;

#define PARAM_ECRITURE_SOCKET(pe) ((pe)->sock)
#define PARAM_ECRITURE_LISTE_ECR(pe) ((pe)->file_ecriture)
#define PARAM_ECRITURE_SEM_ECR(pe) ((pe)->sem_ecr)
#define PARAM_ECRITURE_SEM_LOG(pe) ((pe)->sem_log)

extern void * ecriture (void *);
extern Param_ecriture * creer_param_ecriture (int , Liste * , sem_t * , sem_t *) ;

extern int affiche ;

#endif /*__ECRITURE__*/
