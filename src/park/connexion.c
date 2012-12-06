#include <semaphore.h>
#include <stdlib.h>

#include "liste.h"
#include "frame.h"
#include "ecriture.h"
#include "traitement.h"
#include "park.h"
#include "log.h"

int trt_connexion (Traitement *t)
{
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GC_DEMANDE_CONNEXION :
      if (LOG_ENTETE_FRAME(f , 0 , 40 , LOG_CONNEXION ,
			   "Demande connexion client %d %d %d %d") <0)
	return -1 ;

      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION ,
		       ST_GC_CONNEXION_ACCEPTER , 0 , 0);

      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr_cli);

      free(t) ;
      break ;
    case ST_GC_DEMANDE_DECONNEXION :
      if (LOG_ENTETE_FRAME(f , 0 , 42 , LOG_CONNEXION ,
			   "Demande deconnexion %d %d %d %d") <0)
	return -1 ;

      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , 
		       ST_GC_DECONNEXION_ACCEPTER , 0 , 0);

      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr_cli);

      sem_post(&sem_accept_connexion);

      free(t) ;
      break ;
    case ST_GC_CONNEXION_ACCEPTER :
      LOG_ENTETE_FRAME(f , 0 , 32 , LOG_CONNEXION ,
		       "Connexion serveur %d %d %d %d");
            
      SET_ENTETE_FRAME (ef , TYPE_GESTION_VELO , ST_GV_RECUPERE_VELO,0,sizeof(int));
      
      if ((DATA_FRAME(f) = malloc (sizeof(int))) == NULL)
	return -1 ;

      * (int *) DATA_FRAME(f) = nb_velo ;
      
      if (ecrit_frame_liste (f , file_ecriture_serveur , 0) < 0)
	return -1 ;

      sem_post(&sem_ecr_srv);
      
      free(t) ;
      break ;
    case ST_GC_DECONNEXION_ACCEPTER :
      LOG_ENTETE_FRAME(f , 0 , 34 , LOG_CONNEXION ,
		       "Deconnexion serveur %d %d %d %d");
      free(ef);
      free(f);
      free(t) ;
      pthread_exit(0);
    }
  return 0 ;
}
