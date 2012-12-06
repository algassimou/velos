#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "liste.h"
#include "log.h"
#include "frame.h"
#include "telechargement.h"


void * reception ( void * v)
{
  Param_Reception * pr = (Param_Reception *) v ;
  FILE * fl = PARAM_RECEPTION_FILE(pr) ;
  Liste * file_reception = PARAM_RECEPTION_LISTE_RECEP(pr);
  Liste * lecr = PARAM_RECEPTION_LISTE_ECR(pr);
  Liste * ltrans = PARAM_RECEPTION_LISTE_TRANSFERT(pr) ;
  
  sem_t *sem_ecr = PARAM_RECEPTION_SEM_ECR(pr) ;
  sem_t *sem_recep = PARAM_RECEPTION_SEM_RCP(pr)  ;
  sem_t *sem_trs = PARAM_RECEPTION_SEM_TSR(pr);

  frame * f = NULL ;
  entete_frame * ef = NULL , *efr = NULL ;
  void * data = NULL ;
  int * res = malloc (sizeof(int)) ;
  if (res == NULL)
    goto sortie ;

  while (1)
    {	
      sem_wait(sem_recep);

      sem_wait(&LISTE_SEMAPHORE(file_reception));
      liste_rm (file_reception , NULL , (void **) &f);
      sem_post(&LISTE_SEMAPHORE(file_reception)) ;

      if (f != NULL)
	{
	  ef = ENTETE_FRAME(f) ;
	  data = DATA_FRAME(f) ;

	  switch (SOUS_TYPE_FRAME (ef))
	    {
	    case ST_GT_DEB_TRANS_CONTRAT :
	      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT , 
			       ST_GT_DEB_TRANS_CONTRAT_OK , 0 , 0);
	      if (ecrit_frame_liste (f , lecr , 0) < 0)
		goto sortie ;
	      sem_post(sem_ecr);
	      break ;

	    case ST_GT_TRANS_BLOCK_CONTRAT :
	      if (fwrite (data , 1 , LONGUEUR_DATA(ef) ,fl) < LONGUEUR_DATA(ef))
		{
		  SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT ,
				   ST_GT_INTERROMPT_TRANS , 0 , 0) ;

		  free(data);
		  DATA_FRAME(f) = NULL ;

		  if (ecrit_frame_liste (f , lecr , 0) < 0)
		    goto sortie ;
		  sem_post(sem_ecr);
		}
		destroy_frame(f);
	      break ;
	    case ST_GT_INTERROMPT_TRANS :
	      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT ,
			       ST_GT_INTERROMPT_TRANS , 0 , 0) ;

	      DATA_FRAME(f) = NULL ;
	      if (ecrit_frame_liste (f , lecr , 0) < 0)
		goto sortie ;
	      sem_post(sem_ecr);
	      break ;
	    case ST_GT_INTERROMPT_TRANS_OK :
	      destroy_frame(f) ;
	      goto sortie ;

	    case ST_GT_FIN_TRANS_CONTRAT :
	      fclose(fl) ;
	      SET_ENTETE_FRAME(ef , TYPE_GESTION_TELECHARGEMENT ,
			       ST_GT_FIN_TRANS_CONTRAT_OK , 0 , 0) ;

	      DATA_FRAME(f) = NULL ;

	      if (ecrit_frame_liste (f , lecr , 0) < 0)
		goto sortie ;
	      sem_post(sem_ecr);
	      goto sortie ;
	      break ;
	    }
	}
    }
 sortie :
  free(pr);
  pthread_exit(res);
}
