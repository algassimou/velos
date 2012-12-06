#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "traitement.h"
#include "telechargement.h"
#include "frame.h"
#include "log.h"
#include "client.h"

static int choix_velos (void * data , int s)
{
  int cpt = 0 ;
  int * val = data ;
  int choix = -1 ;
  fprintf(stdout , "\n\n*****************Vélos Disponibles **************\n");
  for ( ; cpt < s ; cpt ++)
    {
      fprintf (stdout , "%d --> %d\n" , cpt , val[cpt]);
    }
  while (choix < 0 || choix >= s)
    {
      fprintf(stdout , "Quel est votre choix : ");
      scanf ("%d" , &choix);
    }
  return choix ;
}

int trt_velo (Traitement *t)
{
  pthread_t *dwl = NULL ;
  Param_Reception * pr = NULL ;
  FILE * fl = NULL ;

  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;


  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GV_VELOS_A_LOUER :
      choix = choix_velos (DATA_FRAME(f) , LONGUEUR_DATA(ef)/sizeof(int));
      free(DATA_FRAME(f));
      DATA_FRAME(f) = NULL ;

      fl = fopen (CLIENT_CONTRAT , "wb") ;

      if ((pr = malloc (sizeof(Param_Reception))) == NULL)
	break ;

      liste_init (file_reception , NULL) ;
      sem_init (&sem_rcp , 0 , 0);

      PARAM_RECEPTION_FILE(pr) = fl ;
      PARAM_RECEPTION_SEM_ECR(pr) = &sem_ecr;
      PARAM_RECEPTION_SEM_RCP(pr) = &sem_rcp ;
      PARAM_RECEPTION_SEM_TSR(pr) = NULL ;
      PARAM_RECEPTION_LISTE_ECR(pr) = file_ecriture ;
      PARAM_RECEPTION_LISTE_RECEP(pr) = file_reception ;
      PARAM_RECEPTION_LISTE_TRANSFERT (pr) = NULL ;

      if (((dwl = malloc (sizeof(pthread_t))) == NULL) ||
	  (pthread_create(dwl , NULL , reception , (void *) pr) != 0))
	pthread_exit (0);	

      SET_ENTETE_FRAME(ef,TYPE_GESTION_CONTRAT,ST_GCT_DEMANDE_CONTRAT ,
		       0,0);

      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;

      sem_post (&sem_ecr);
      break ;
    case ST_GV_PAS_VELOS_A_LOUER :
      SET_ENTETE_FRAME(ef,TYPE_GESTION_CONNEXION,ST_GC_DEMANDE_DECONNEXION ,0 ,0);
      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post (&sem_ecr);
      break ;

      /*case ST_GV_VELO_LOUER :
      SET_ENTETE_FRAME (ef , TYPE_GESTION_VELO , ST_GV_DEMANDE_PARKS , 
			0 , 0 );
      free (DATA_FRAME(f));
      DATA_FRAME(f) = NULL ;

      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post (&sem_ecr);

      break ;
      */
    case ST_GV_ADRESSE_SERVEUR :

      port = * (int *) DATA_FRAME(f) ;

      adresse_park = malloc (LONGUEUR_DATA(ef) - sizeof(int));

      memcpy (adresse_park , data + sizeof(int) , 
	      LONGUEUR_DATA(ef)-sizeof(int)); 

      SET_ENTETE_FRAME (ef , TYPE_GESTION_CONNEXION , 
			ST_GC_DEMANDE_DECONNEXION , 0 , 0) ;
      
      if (ecrit_frame_liste (f , file_ecriture , 0) < 0)
	return -1 ;
      sem_post (&sem_ecr);
    }
  return 0 ;
}
