#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "liste.h"
#include "traitement.h"
#include "telechargement.h"
#include "serveur.h"
#include "log.h"
#include "frame.h"

int trt_contrat (Traitement *t)
{
  pthread_t * dwl = NULL ;
  Param_Envoi * pe = NULL ;
  FILE * fl = NULL ;
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;
  Comm * c = NULL ;
  
  c = (Comm *) TRAITEMET_AUTRE(t) ;
  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GCT_DEMANDE_CONTRAT :
      if ((fl = fopen (SERVEUR_CONTRAT , "rb")) == NULL)
	{
	  if (log_mesg (0,0,LOG_GENERAL , "pas de contrat") < 0)
	    break ;
	  /*SET_ENTETE_FRAME (ef , TYPE_GESTION_ERREUR , */
	}
      
      if ((pe = malloc (sizeof(Param_Envoi))) == NULL)
	break ;

      liste_init (COMM_LISTE_DWL(c) , NULL);

      PARAM_ENVOI_FILE(pe) = fl ;
      PARAM_ENVOI_LISTE_ENVOI(pe) = COMM_LISTE_DWL(c) ;
      PARAM_ENVOI_LISTE_ECR(pe) = COMM_LISTE_ECR(c) ;
      PARAM_ENVOI_SEM_EV(pe) = &COMM_SEM_DWL(c) ;
      PARAM_ENVOI_SEM_ECR(pe) = &COMM_SEM_ECR(c) ;

      if (((dwl = malloc (sizeof(pthread_t))) == NULL) ||
	  (pthread_create(dwl , NULL , envoi , (void *) pe) != 0))
	pthread_exit (0);	

      SET_ENTETE_FRAME(ef,TYPE_GESTION_TELECHARGEMENT,ST_GT_DEB_TRANS_CONTRAT,0,0);

      if (ecrit_frame_liste (f , COMM_LISTE_ECR(c) , 0) < 0)
	return -1 ;
      sem_post(&COMM_SEM_ECR(c));

      free(t) ;
      break ;
    default :
      break ;
    }
  return 0 ;
}
