#ifndef __LECTURE__
#define __LECTURE__

#include <semaphore.h>

typedef struct {
  Liste * ecriture ;
  sem_t * sem_ecr ;
  sem_t * sem_trt ;
  Liste * telechargement ;
  sem_t * sem_dwl ;
  sem_t * sem_log ;
  void * autre ;
  int sock ;
} Param_Lecture ;

#define PARAM_LECTURE_LISTE_ECR(pl) ((pl)->ecriture)
#define PARAM_LECTURE_SEM_LOG(pl) ((pl)->sem_log)
#define PARAM_LECTURE_LISTE_DWL(pl) ((pl)->telechargement)
#define PARAM_LECTURE_SEM_TRT(pl) ((pl)->sem_trt)
#define PARAM_LECTURE_SEM_ECR(pl) ((pl)->sem_ecr)
#define PARAM_LECTURE_SEM_DWL(pl) ((pl)->sem_dwl)
#define PARAM_LECTURE_SOCKET(pl) ((pl)->sock)
#define PARAM_LECTURE_AUTRE(pl) ((pl)->autre)

extern void * lecture (void *);
extern Param_Lecture * creer_param_lecture (Liste * , Liste * , sem_t * , sem_t * , sem_t * , sem_t * , int , void *);

extern int affiche ;

#define DROIT 0644

#endif 
