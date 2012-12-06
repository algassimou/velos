#include <semaphore.h>
#include <stdlib.h>

#include "liste.h"
#include "frame.h"
#include "ecriture.h"
#include "traitement.h"
#include "client.h"
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
    case ST_GC_CONNEXION_ACCEPTER :

      SET_ENTETE_FRAME(ef , TYPE_GESTION_VELO , ST_GV_LOCATION_VELO , 0 , 0) ;
      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr);

      free (t);
      break ;
    case ST_GC_DECONNEXION_ACCEPTER :
      SET_ENTETE_FRAME(ef , TYPE_GESTION_INTERNE , ST_GI_DECONNECTION , 0 , 0);

      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post(&sem_ecr);

      if (dec == -1 )
	{
	  dec = 0 ;
	  if ((f = malloc (sizeof(frame))) == NULL)
	    return -1 ;

	  if (creer_entete_frame (&ENTETE_FRAME(f) , TYPE_GESTION_INTERNE , 
				  ST_GI_CC_SOMMEIL , 0 , 0) < 0)
	    return -1 ;
	  DATA_FRAME(f) = NULL ;
	  ecrire_trt_liste(f , TRAITEMET_AUTRE(t));
	  sem_post(&sem_trt);
	}
      free (t);

      break ;
    }

  return 0 ;
}
