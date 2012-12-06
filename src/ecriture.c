#include <fcntl.h>           /* For O_* constants */
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include "liste.h"
#include "frame.h"
#include "log.h"
#include "ecriture.h"

void * ecriture (void *v)
{
  Param_ecriture * pe = (Param_ecriture *) v ;
  int sock = -1 ; 
  Liste * file_ecriture = NULL ;
  sem_t * sem_ecr = NULL , *sem_log = NULL ;
  
  sock  = PARAM_ECRITURE_SOCKET(pe) ;
  file_ecriture = PARAM_ECRITURE_LISTE_ECR(pe) ;
  sem_ecr = PARAM_ECRITURE_SEM_ECR(pe) ;
  sem_log = PARAM_ECRITURE_SEM_LOG(pe);

  frame * f = NULL ;
  int * res = NULL ;
  
  if ((res = malloc (sizeof(int))) == NULL)
    goto sortie ;

  *res = -1 ;

  while (1)
    {
      f = NULL ;

      sem_wait(sem_ecr);
      sem_wait(&(LISTE_SEMAPHORE(file_ecriture)));
      liste_rm(file_ecriture, NULL , (void **)&f);
      sem_post(&(LISTE_SEMAPHORE(file_ecriture)));

      if (f != NULL)
	{
	  if (affiche && LOG_ENTETE_FRAME(f, 0 , 23 ,LOG_GENERAL , 
					  "Ecriture %d %d %d %d") < 0)
	    goto sortie ;


	  switch(TYPE_FRAME(ENTETE_FRAME(f)))
	    {
	    case TYPE_GESTION_INTERNE :
	      if (SOUS_TYPE_FRAME(ENTETE_FRAME(f)) == ST_GI_DECONNECTION)
		{
		  destroy_frame(f) ;
		  goto end ;
		}
	  destroy_frame(f) ;
	      break ;
	    case TYPE_GESTION_CONNEXION :
	      if (SOUS_TYPE_FRAME(ENTETE_FRAME(f)) == ST_GC_DECONNEXION_ACCEPTER)
		{
		  ecrit_frame_socket (f , sock) ;
		  goto end ;
		}
	    default :
	      if (ecrit_frame_socket (f , sock) < 0)
		goto sortie ;
	      break ;
	    }
	}
    }
 end :
  *res = 0 ;
 sortie :
  free(pe);
  pthread_exit(res);
}

Param_ecriture * creer_param_ecriture (int s , Liste * lecr , 
				       sem_t * s_ecr , sem_t * s_log ) 
{
  Param_ecriture * p = NULL ;
  if ((p = malloc (sizeof(Param_ecriture))) == NULL)
    return NULL ;

  PARAM_ECRITURE_SOCKET(p) = s ;
  PARAM_ECRITURE_LISTE_ECR(p) = lecr ;
  PARAM_ECRITURE_SEM_ECR(p) = s_ecr ;
  PARAM_ECRITURE_SEM_LOG(p) = s_log ;
  return p ;
}
