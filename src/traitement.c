#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "liste.h"
#include "log.h"
#include "frame.h"
#include "traitement.h"

Liste * file_traitement = NULL ;
sem_t sem_trt ;
int dec = -1 ;

void * traitement (void *v)
{
  int * res = NULL ;
  Traitement * t = NULL ;
  frame * f = NULL ;

  if ((res = malloc (sizeof(int))) == NULL)
    pthread_exit(0);

  while(1)
    {
      t = NULL ;

      sem_wait(&sem_trt);
      sem_wait(&(LISTE_SEMAPHORE(file_traitement)));
      liste_rm (file_traitement , NULL, (void **)&t);
      sem_post(&(LISTE_SEMAPHORE(file_traitement)));

      if (t != NULL)
	{
	  f = TRAITEMET_FRAME(t) ;
	  switch (TYPE_FRAME(ENTETE_FRAME(f)))
	    {
	    case TYPE_GESTION_CONNEXION :
	      if (trt_connexion(t) < 0)
		goto sortie ;

	      if (dec == 1)
		goto sortie ;

	      break ;
	    case TYPE_GESTION_CONTRAT :
	      if (trt_contrat(t) < 0)
		goto sortie ;
	      break ;
	    case TYPE_GESTION_INTERNE :
	      if (trt_interne(t) < 0)
		goto sortie ;
	      break ;
	    case TYPE_GESTION_VELO :
	      if (trt_velo(t) < 0)
		goto sortie ;
	      break ;
	    case TYPE_GESTION_ERREUR :
	      if (trt_erreur(t) < 0)
		goto sortie ;
	      break ;
	    }
	}
    }

 sortie :
  log_mesg (0,0,LOG_DECONNEXION , "");
  sem_post(&sem_log);
  pthread_exit(res);
}

int ecrire_trt_liste (frame *f , void *v)
{
  Traitement * t = NULL ;
  int res = 0 ;
  if ((t= malloc(sizeof(Traitement))) == NULL)
    return -1 ;

  TRAITEMET_FRAME(t) = f ;
  TRAITEMET_AUTRE(t) = v ;

  sem_wait(&(LISTE_SEMAPHORE(file_traitement)));
  res = liste_add(file_traitement , LISTE_QUEUE(file_traitement) , (void *)t) ;
  sem_post(&(LISTE_SEMAPHORE(file_traitement)));

  return res ;
}
