#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "liste.h"
#include "dliste.h"
#include "frame.h"
#include "log.h"
#include "serveur.h"
#include "traitement.h"

DListe * dliste_connexion = NULL ;
Liste * liste_deconnexion = NULL , *liste_velos = NULL ;
sem_t sem_deconnexion ;

int val_retour_lecture ;
int val_retour_ecriture ;
int affiche = 1  ;

static void init_velos (int i)
{
  int * val = NULL ;
  srand(time(NULL));
  while (i--)
    {
      if ((val = malloc (sizeof(int))) == NULL)
	{
	  fprintf(stderr , "Erreur allocation velo");
	  return ;
	}
      *val = rand () % 100 ;
      liste_add (liste_velos , NULL , (void *) val) ;
    }
}


int main (int argc , char * argv[])
{
  int sock , r = -1 ;
  struct sockaddr_in adr_srv ;
  pthread_t co , dc , lg , tr ;		/* thread connexion et deconnexion */
  Param_log * pl  = NULL ;

  FILE *fg = NULL , *fc = NULL , *fs = NULL ;

  if (get_line_cmd (argc , argv , LINE_OPTION) == -1)
    {
      fprintf (stderr , "%s \n" , HELP);
      return -1 ;
    }

  if ((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
      perror("Socket()");
      return -1;
    }

  adr_srv.sin_family = AF_INET ;
  adr_srv.sin_addr.s_addr = INADDR_ANY ;
  adr_srv.sin_port = htons(port);

  if ((bind(sock , (struct sockaddr *) &adr_srv , sizeof(struct sockaddr_in))) == -1)
    {
      perror("Bind()");
      return -1;
    }

  if ((listen(sock , 5)) == -1)
    {
      perror("Listen()");
      return -1;
    }
    
  sem_init (&(sem_trt) , 0 , 0);
  sem_init (&(sem_log) , 0 , 0);
  sem_init(&sem_deconnexion , 0 , 0);

  liste_velos = malloc (sizeof(Liste)) ;
  if (liste_velos == NULL)
    return -1 ;

  liste_init (liste_velos , NULL) ;
  init_velos(nb_velo);

  
  if ((dliste_connexion = malloc(sizeof(DListe))) == NULL)
    goto  sortie;

  dliste_init(dliste_connexion , NULL);

  if ((liste_deconnexion = malloc(sizeof(Liste))) == NULL)
    goto  sortie;

  liste_init(liste_deconnexion , NULL );

  if ((file_logs = malloc (sizeof(Liste))) == NULL)
    goto  sortie;

  liste_init (file_logs , destroy_log) ;

  if ((file_traitement = malloc (sizeof(Liste))) == NULL)
    goto sortie;

  liste_init (file_traitement , destroy_frame) ;

  /*  if ((fg = fopen (FILE_GENERAL , "w")) == NULL)
      goto netoyage_trt_log ;
  
      if ((fc = fopen (FILE_CONNEXION , "w" )) == NULL)
      goto netoyage_trt_log ;

      if ((fs = fopen (FILE_STAT , "w" )) == NULL)
      goto netoyage_trt_log ;
  */

  if ((pl = creer_param_log (fg , fc , fs )) == NULL)
    goto sortie;
  
  pthread_create(&co , NULL , connexion , (void *) &sock );
  pthread_create(&dc , NULL , deconnexion , (void *) &sock );
  pthread_create(&lg , NULL , logs , (void *)pl);
  pthread_create(&tr , NULL , traitement , NULL);

  pthread_join(co,NULL);
  pthread_join(dc,NULL);
  pthread_join(lg,NULL);
  pthread_join(tr,NULL);

  r = 0 ;
 sortie:
  return r ;
}
