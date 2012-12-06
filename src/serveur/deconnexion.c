#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "liste.h"
#include "dliste.h"
#include "serveur.h"
#include "log.h"

void * deconnexion (void * s)
{
  Comm * c = NULL ;
  int val_retour = 0 ;

  while(1)
    {
      sem_wait(&(sem_deconnexion));

      sem_wait(&(liste_deconnexion -> sem));
      liste_rm(liste_deconnexion , NULL , (void **)&c);
      sem_post(&(liste_deconnexion -> sem));

      pthread_join(*COMM_THREAD_LECTURE(c) ,(void *) &val_retour);
      pthread_join(*COMM_THREAD_ECRITURE(c) , (void *) &val_retour);


      sem_wait(&(dliste_connexion -> sem));
      dliste_remove(dliste_connexion , COMM_DLISTE_ELEM(c) , NULL);
      sem_post(&(dliste_connexion -> sem));

      free(COMM_LISTE_ECR(c));
      free(COMM_LISTE_DWL(c));
      free(COMM_THREAD_LECTURE(c));
      free(COMM_THREAD_ECRITURE(c));
      close(COMM_SOCKET(c));
      free(c);
    }
  pthread_exit(0) ;
}
