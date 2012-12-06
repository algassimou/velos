#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "traitement.h"
#include "telechargement.h"
#include "frame.h"
#include "log.h"
#include "park.h"

int trt_contrat (Traitement *t)
{
  pthread_t * dwl1 = NULL , *dwl2 = NULL ;
  Param_Reception * pr = NULL ;
  Param_Envoi * pe = NULL ;
  FILE * fl = NULL ;
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GCT_DEMANDE_CONTRAT :
      liste_init (file_envoi , NULL);
      sem_init (&sem_ev , 0 , 0);

      if ((pe = malloc (sizeof(Param_Envoi))) == NULL)
	return -1 ;

      PARAM_ENVOI_LISTE_ENVOI(pe) = file_envoi ;
      PARAM_ENVOI_LISTE_ECR(pe) = file_ecriture_client ;
      PARAM_ENVOI_SEM_EV(pe) = &sem_ev ;
      PARAM_ENVOI_SEM_ECR(pe) = &sem_ecr_cli ;

      if ((dwl1 = malloc (sizeof(pthread_t))) == NULL)
	return -1 ;
      
      if ((fl = fopen (PARK_CONTRAT , "rb")) != NULL)
	PARAM_ENVOI_FILE(pe) = fl ;

      if ((pthread_create(dwl1 , NULL , envoi , (void *) pe) != 0))
	return -1 ;

      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT , 
		       ST_GT_DEB_TRANS_CONTRAT , 0,0);

      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;

      sem_post (&sem_ecr_cli);
      
      free(t) ;
      break ;
    case ST_GCT_RECEPTION_CONTRAT :
      SET_ENTETE_FRAME (ef , TYPE_GESTION_CONTRAT , 
			ST_GCT_REPONSE_CONTRAT , 0 , 0);

      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;

      sem_post (&sem_ecr_cli);
      break ;
    default :
      break ;
    }
  return 0 ;
}
