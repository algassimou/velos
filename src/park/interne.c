#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "liste.h"
#include "frame.h"
#include "lecture.h"
#include "ecriture.h"
#include "traitement.h"
#include "park.h"

int trt_erreur (Traitement *t)
{
  return 0 ;
}


int trt_interne (Traitement * t)
{
  Param_Lecture *plc = NULL ;
  Param_ecriture *pec = NULL ;
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;

  f = TRAITEMET_FRAME(t) ;
  ef = ENTETE_FRAME(f) ;
  data = DATA_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GI_PP_INITIALISE_CONNECTION_SERVEUR :
      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , ST_GC_DEMANDE_CONNEXION ,
		       0 , sizeof(int));

      if ((data = malloc (sizeof(int))) == NULL)
	return -1 ;

      *(int *) data = id ;

      DATA_FRAME(f) = data ;

      if (ecrit_frame_liste (f , file_ecriture_serveur , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr_srv);

      free(t) ;
      break ;
    case ST_GI_PP_SET_SOCKET_CLIENT:
      sem_init (&sem_ecr_cli , 0 , 0);
      
      if ((file_ecriture_client = malloc (sizeof(Liste))) == NULL)
	return -1 ;

      liste_init (file_ecriture_client , NULL) ;

      if ((file_envoi = malloc (sizeof (Liste))) == NULL)
	return -1 ;

      liste_init (file_envoi , NULL);
      sem_init (&sem_ev , 0 , 0 );

      if ((plc = creer_param_lecture(file_ecriture_client , file_envoi , 
				     &sem_trt , &sem_ecr_cli , &sem_ev , &sem_log , 
				     *(int *)data , NULL)) == NULL)
	return -1 ;

      if ((pec = creer_param_ecriture(*(int*) data , file_ecriture_client ,
				       &sem_ecr_cli , &sem_log)) == NULL)
	return -1 ;

      pthread_create(&lec_cli , NULL , lecture ,plc );
      pthread_create(&ecr_cli , NULL , ecriture , pec );

      destroy_frame(f);
      free(t) ;
      break ;
    default :
      break ;
    }
  return 0 ;
}
