#include <fcntl.h>    
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "liste.h"
#include "frame.h"
#include "lecture.h"
#include "log.h"
#include "traitement.h"


static int gestion_telechargement (frame *f , Liste *lt , Liste *le ,
				   void * autre , sem_t * sem_trt , 
				   sem_t * sem_ecr , sem_t * sem_dwl)
{
  frame * fr = NULL ;
  entete_frame *efr = NULL ;

  if (SOUS_TYPE_FRAME(ENTETE_FRAME(f)) == ST_GT_TRANS_BLOCK_CONTRAT)
    {
      if (creer_entete_frame(&efr ,TYPE_GESTION_TELECHARGEMENT,
			     ST_GT_RECEP_BLOCK_CONTRAT,
			     OFFSET_FRAME(ENTETE_FRAME(f)) , 0) < 0)
	return -1 ;

      if ((fr = malloc (sizeof(frame))) == NULL)
	return -1 ;

      ENTETE_FRAME(fr) = efr ;
      DATA_FRAME(fr) = NULL ;
      ecrit_frame_liste (fr , le , 0);
      sem_post(sem_ecr);
    }

  else if (SOUS_TYPE_FRAME(ENTETE_FRAME(f)) == ST_GT_FIN_TRANS_CONTRAT_OK)
    {
      if (creer_entete_frame(&efr , TYPE_GESTION_CONTRAT , 
			     ST_GCT_RECEPTION_CONTRAT , 0 , 0) < 0)
	return -1 ;

      if ((fr = malloc (sizeof(frame))) == NULL)
	return -1 ;

      ENTETE_FRAME(fr) = efr ;
      DATA_FRAME(fr) = NULL ;
      ecrire_trt_liste (fr , autre);
      sem_post(sem_trt);
    }

  ecrit_frame_liste (f , lt , 0);
  sem_post(sem_dwl);

  return 0 ;
}

void * lecture (void * v)
{
  Param_Lecture * p = (Param_Lecture *) v ;
  Liste * ldwl = NULL , *lecr = NULL ;
  int sock = -1 ; 
  void * autre = NULL ; 
  
  frame * f = NULL ;  
  entete_frame * ef = NULL;
  void * data = NULL ;
  
  int * res = NULL ;
  
  sock = PARAM_LECTURE_SOCKET(p) ;
  ldwl = PARAM_LECTURE_LISTE_DWL(p);
  lecr = PARAM_LECTURE_LISTE_ECR(p);
  autre = PARAM_LECTURE_AUTRE(p) ;
 
  sem_t *sem_trt = PARAM_LECTURE_SEM_TRT(p)  ;
  sem_t *sem_ecr = PARAM_LECTURE_SEM_ECR(p)  ;
  sem_t *sem_dwl = PARAM_LECTURE_SEM_DWL(p) ;
  sem_t *sem_log = PARAM_LECTURE_SEM_LOG(p)  ;
  
  if ((res = malloc (sizeof(int))) == NULL)
    goto sortie ;

  *res = -1 ;

  while(1)
    {
      data = NULL ;
      f = NULL  ;

      if ((ef = malloc(sizeof(entete_frame))) == NULL)
	goto sortie ;

      if (reception_frame (sock , ef , &data ) < 0)
	goto sortie ;

      if ((f = malloc (sizeof(frame))) == NULL)
	goto sortie ;

      ENTETE_FRAME(f) = ef ;
      DATA_FRAME(f) = data ;

      if (affiche && LOG_ENTETE_FRAME(f, 0 , 22 ,LOG_GENERAL ,
				      "Lecture %d %d %d %d") < 0)
	goto sortie ;


      switch (TYPE_FRAME(ef))
	{
	case TYPE_GESTION_TELECHARGEMENT :
	  if (gestion_telechargement(f , ldwl , lecr , autre , sem_trt ,
				     sem_ecr , sem_dwl) < 0)
	    goto sortie ;
	  break ;
	case TYPE_GESTION_CONNEXION :
	  if (SOUS_TYPE_FRAME(ef) == ST_GC_DECONNEXION_ACCEPTER || 
	      SOUS_TYPE_FRAME(ef) == ST_GC_DEMANDE_DECONNEXION)
	    { 
	      ecrire_trt_liste (f, autre);
	      sem_post(sem_trt);
	      goto end ;
	    }
	default :
	  ecrire_trt_liste (f, autre);
	  sem_post(sem_trt);
	  break ;
	}
    }
 end :
  *res = 0 ;
 sortie :
  free(p) ;
  pthread_exit(res) ;
}

Param_Lecture *creer_param_lecture (Liste * lecr , Liste * ldwl ,
				    sem_t * s_trt , sem_t * s_ecr , sem_t * s_dwl , 
				    sem_t * s_log , int s , void * autre)
{
  Param_Lecture * pl = NULL ;
  if ((pl = malloc (sizeof(Param_Lecture))) == NULL)
    return NULL ;

  PARAM_LECTURE_LISTE_ECR(pl) = lecr ;
  PARAM_LECTURE_LISTE_DWL(pl) = ldwl ;
  PARAM_LECTURE_SEM_TRT(pl) = s_trt ;
  PARAM_LECTURE_SEM_ECR(pl) = s_ecr ;
  PARAM_LECTURE_SEM_DWL(pl) = s_dwl ;
  PARAM_LECTURE_SEM_LOG(pl) = s_log ;
  PARAM_LECTURE_SOCKET(pl) = s ;
  PARAM_LECTURE_AUTRE(pl) = autre ;

  return pl ;
}
