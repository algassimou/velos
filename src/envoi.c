#include <fcntl.h>    
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include "liste.h"
#include "frame.h"
#include "log.h"
#include "telechargement.h"

void * envoi (void * v)
{
  Param_Envoi * pe = (Param_Envoi *) v ;
  FILE * f = PARAM_ENVOI_FILE(pe) ;
  Liste * lecr = PARAM_ENVOI_LISTE_ECR(pe);
  Liste * file_envoi = PARAM_ENVOI_LISTE_ENVOI(pe);
  sem_t *sem_ecr = PARAM_ENVOI_SEM_ECR(pe);
  sem_t *sem_ev = PARAM_ENVOI_SEM_EV(pe) ;
  int * res = NULL ;
  frame * fr  = NULL ;
  entete_frame * ef = NULL ;
  void * mesg = NULL ;
  int size = 0 , offset = 0 ;

  if ((res = malloc (sizeof(int))) == NULL)
    goto sortie ;

  *res = -1 ;
  
  while (1)
    {
      sem_wait(sem_ev);
      sem_wait(&LISTE_SEMAPHORE(file_envoi));
      liste_rm(file_envoi , NULL , (void **) &fr) ;
      sem_post(&LISTE_SEMAPHORE(file_envoi));

      if (fr != NULL)
	{
	  switch (SOUS_TYPE_FRAME(ENTETE_FRAME(fr)))
	    {
	    case ST_GT_FIN_TRANS_CONTRAT_OK :
	      destroy_frame(fr);
	      fclose(f) ;  
	      *res = 0 ;
	      goto netoyage ;

	    case ST_GT_INTERROMPT_TRANS:
	      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT , 
			       ST_GT_INTERROMPT_TRANS_OK  , 0 , 0) ;
		  
	      ENTETE_FRAME(fr) = ef ;
	      DATA_FRAME(fr) = NULL ;
	      ecrit_frame_liste(fr , lecr , 0);
	      sem_post(sem_ecr);

	      goto netoyage ;

	    case ST_GT_INTERROMPT_TRANS_OK :
	      destroy_frame(fr);
	      fclose(f) ;  
	      *res = 0 ;
	      goto netoyage ;


	    case ST_GT_TRANSIT_BLOCK :
	      break ;

	    case ST_GT_DEB_TRANS_CONTRAT_OK :
	    case ST_GT_RECEP_BLOCK_CONTRAT :

	      ef = ENTETE_FRAME(fr) ;

	      if (f != NULL)
		{
		  if (feof(f) == 0)
		    {
		      if ((mesg = malloc(BLOCK_CONTRAT_SIZE)) == NULL)
			pthread_exit(res) ;

		      if ((size = fread(mesg , 1 , BLOCK_CONTRAT_SIZE , f)) < 0)
			{
			  SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT ,
					   ST_GT_INTERROMPT_TRANS , 0 , 0);
			  
			  ENTETE_FRAME(fr) = ef ;
			  DATA_FRAME(fr) = NULL ;
			  if ( ecrit_frame_liste(fr , lecr , 0) < 0)
			    goto netoyage ;

			  sem_post(sem_ecr);
			}

		      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT ,
				       ST_GT_TRANS_BLOCK_CONTRAT , offset , size);

		      DATA_FRAME(fr) = mesg ;

		      offset++ ;

		      if (ecrit_frame_liste (fr , lecr , 0) < 0)
			goto netoyage ;

		      sem_post(sem_ecr);
		    }
		  else 
		    {
		      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT ,
				       ST_GT_FIN_TRANS_CONTRAT , 0 , 0);

		      ENTETE_FRAME(fr) = ef ;
		      DATA_FRAME(fr) = NULL ;

		      if (ecrit_frame_liste (fr , lecr , 0) < 0)
			goto netoyage ;

		      sem_post(sem_ecr);
		    }
		}
	      else
		sem_post(sem_ecr);
	      /*destroy_frame(fr);*/
	      break ;
	    }
	}
    }
 netoyage :
  /*  free(file_envoi);*/
 sortie :
  free(pe);
  pthread_exit(res);
}
