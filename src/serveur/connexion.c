#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "frame.h"
#include "log.h"
#include "lecture.h"
#include "ecriture.h"
#include "traitement.h"
#include "serveur.h"

int identifiant ;

int trt_connexion (Traitement * t)
{
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;
  Comm * c = NULL ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;
  c = TRAITEMET_AUTRE(t);

  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GC_DEMANDE_CONNEXION :

      if (LOG_ENTETE_FRAME(f , 0 , 40 , LOG_CONNEXION ,
			   "Demande connexion park %d %d %d %d") <0)
	return -1 ;

      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , ST_GC_CONNEXION_ACCEPTER ,
		       0 , 0);

      if (ecrit_frame_liste (f , COMM_LISTE_ECR(c) , 0) < 0)
	return -1 ;

      sem_post(&COMM_SEM_ECR(c));

      free(t) ;
      break ;
    case ST_GC_DEMANDE_DECONNEXION :
      if (LOG_ENTETE_FRAME( f , 0 , 42 , LOG_CONNEXION ,
			   "Demande deconnexion park %d %d %d %d") <0)
	return -1 ;

      SET_ENTETE_FRAME(ef , TYPE_GESTION_CONNEXION , ST_GC_DECONNEXION_ACCEPTER ,
		       0 , 0);
      if (ecrit_frame_liste (f , COMM_LISTE_ECR(c) , 0) < 0)
	return -1 ;

      sem_post(&COMM_SEM_ECR(c));

      sem_wait(&LISTE_SEMAPHORE(liste_deconnexion));
      liste_add(liste_deconnexion , LISTE_QUEUE(liste_deconnexion) , (void *) c);
      sem_post(&LISTE_SEMAPHORE(liste_deconnexion));
      sem_post(&(sem_deconnexion));

      free(t) ;
      break ;
    }
  return 0 ;
}
void * connexion (void * s)
{
  int sock = * (int *) s ;
  int cli = 0 ;
  struct sockaddr_in adr_cli ;
  socklen_t  adr_cli_len = 0  ;
  Comm * c = NULL ;

  identifiant = 0 ;

  while(1)
    {
      if ((cli=accept(sock,(struct sockaddr*) &adr_cli ,&adr_cli_len)) < 0)
	{
	  perror("Accept()");
	  break;
	}

      Param_Lecture * pl = NULL ;
      Param_ecriture * pe = NULL ;
      char * nom_sem_ecr = NULL , *nom_sem_dwl = NULL ;
      int size = 0 ;

      if ((c = malloc(sizeof(Comm))) == NULL)
	goto sortie ;

      if ((COMM_LISTE_ECR(c) = malloc (sizeof(Liste))) == NULL)
	goto sortie ;

      if ((COMM_LISTE_DWL(c) = malloc (sizeof(Liste))) == NULL)
	goto sortie ;

      if ((COMM_THREAD_LECTURE(c) = malloc(sizeof(pthread_t))) == NULL)
	goto sortie ;

      if ((COMM_THREAD_ECRITURE(c) = malloc(sizeof(pthread_t))) == NULL)
	goto sortie ;
	
      sem_init (&COMM_SEM_DWL(c) , 0 , 0);
      sem_init (&COMM_SEM_ECR(c) , 0 , 0);

      if ((pl = creer_param_lecture (COMM_LISTE_ECR(c) , COMM_LISTE_DWL(c) ,
				     &sem_trt , &COMM_SEM_ECR(c) , &COMM_SEM_DWL(c) , &sem_log,
				     cli , c)) == NULL)
	goto sortie ;

      if ((pe = creer_param_ecriture (cli,COMM_LISTE_ECR(c),
				      &COMM_SEM_ECR(c) , &sem_log)) == NULL)
	goto sortie ;

      identifiant++ ;
      
      COMM_CLE(c) = 0 ;
      COMM_SOCKET(c) = cli ; 

      liste_init(COMM_LISTE_ECR(c) , NULL );

      sem_wait(&(dliste_connexion -> sem));
      dliste_add_next(dliste_connexion , DLISTE_QUEUE(dliste_connexion) , c);
      COMM_DLISTE_ELEM(c) = DLISTE_QUEUE(dliste_connexion);
      sem_post(&(dliste_connexion -> sem));

      if (pthread_create(COMM_THREAD_LECTURE(c) , NULL ,
			 lecture , (void *)pl ) != 0)
	goto sortie ;

      if (pthread_create(COMM_THREAD_ECRITURE(c) , NULL , 
			 ecriture , (void *) pe ) != 0)
	goto sortie ;

    }
 sortie :
  pthread_exit(0) ;
}

