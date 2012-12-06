#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "traitement.h"
#include "telechargement.h"
#include "frame.h"
#include "log.h"
#include "client.h"

static int get_reponse_client ()
{
  FILE * fl = NULL ;
  char * mesg = NULL ;
  int nb = 0 , c ;

  if ((fl = fopen (CLIENT_CONTRAT , "r")) == NULL)
    return -1 ;
  
  if ((mesg = malloc (71)) == NULL)
    return -1 ;

  while (fgets (mesg , 70 , fl) != NULL)
    {
      fprintf (stdout , "%s" , mesg);
      nb++ ;
      if (nb == 10)
	{
	  nb = 0 ;
	  fprintf (stdout , "\n%s : " , "Entrez pour continuer ");
	  getchar() ;
	  fprintf (stdout , "\n");
	}
    }
  do
    {
      fprintf (stdout , "Acceptez vous le contrat ? [o/n] ");
      c = getchar () ;
      if (c == 'o')
	return 1 ;
      else if (c == 'n')
	return 0 ;
      else if (c != '\n')
	getchar() ;
    } while (1) ;
}

int trt_contrat (Traitement *t)
{
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;
  int reponse = 0 ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GCT_REPONSE_CONTRAT :
      reponse = get_reponse_client ();
      if (reponse)
	{
	  SET_ENTETE_FRAME(ef , TYPE_GESTION_VELO ,
			   ST_GV_CHOIX_VELO , 0 , sizeof(int));
	  if ((DATA_FRAME(f) = malloc (sizeof(int))) == NULL)
	    return -1 ;

	  * (int *) DATA_FRAME(f) = choix ;
	}
      else 
	{
	  SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , 
			   ST_GC_DEMANDE_DECONNEXION , 0 , 0);

	  DATA_FRAME(f) = NULL ;
	}

      ecrit_frame_liste (f , file_ecriture ,0); 
      sem_post(&sem_ecr);
      break ;
    default :
      break ;
    }
  return 0 ;
}
