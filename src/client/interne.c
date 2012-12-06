#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "frame.h"
#include "traitement.h"

int trt_erreur (Traitement *t)
{
  return 0 ;
}

int trt_interne (Traitement *t)
{
  frame * f = NULL ;
  entete_frame *ef = NULL ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GI_CC_INITIALISE_CONNECTION_PARK :
      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , ST_GC_DEMANDE_CONNEXION ,
		       0 , 0);

      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr);
      free(t) ;
      break ;

    case ST_GI_CC_SOMMEIL :
      sleep (temps);
      creer_connection (adresse_park , port , NULL , NULL);
      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , ST_GC_DEMANDE_CONNEXION ,
		       0 , 0);

      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr);
      free(t) ;
      break ;

    default :
      break ;
    }
  return 0 ;
}
