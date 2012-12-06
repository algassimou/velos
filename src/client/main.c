#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "liste.h"
#include "client.h"
#include "log.h"
#include "frame.h"
#include "traitement.h"
#include "lecture.h"
#include "ecriture.h"
#include "telechargement.h"

pthread_t lec , ecr , trt , lg , recep ;
sem_t sem_ecr , sem_rcp ;
Liste *file_reception = NULL , *file_ecriture = NULL ;
int affiche = 1 , choix = 0 ;

int main (int argc , char * argv[])
{
  Param_log * p = NULL ;
  Param_Lecture *pl = NULL ;
  Param_ecriture *pe = NULL ;

  FILE * fg = NULL , *fc = NULL , *fs = NULL ;
  int *res = NULL ; 
  int sock = 0 , r = -1 ;

  frame * f = NULL ;

  if (get_line_cmd (argc , argv , LINE_OPTION) == -1)
    {
      fprintf (stderr , "%s \n" , HELP);
      return -1 ;
    }
  sem_init (&(sem_trt) , 0 , 0);
  sem_init (&(sem_log) , 0 , 0);  
 
  file_traitement = malloc(sizeof(Liste));
  if (file_traitement == NULL)
    return -1 ;

  liste_init(file_traitement , NULL );

  if ((file_reception = malloc (sizeof(Liste))) == NULL)
    return -1 ;

  file_logs = malloc (sizeof(Liste)) ;
  if (file_logs == NULL)
    return -1 ;

  liste_init (file_logs , destroy_log) ;

  /*  if ((fg = fopen (FILE_GENERAL , "w")) == NULL)
      return -1 ;
  
      if ((fc = fopen (FILE_CONNEXION , "w" )) == NULL)
      return -1 ;

      if ((fs = fopen (FILE_STAT , "w" )) == NULL)
      return -1 ;
  */

  if ((p = creer_param_log (fg , fc , fs)) == NULL)
    goto sortie ;

  if ((res = malloc (sizeof(int))) == NULL)
    goto sortie ;

  pthread_create(&trt , NULL , traitement , NULL );
  pthread_create(&lg , NULL , logs , p );

  if (creer_connection (adresse_park , port , file_reception , &sem_rcp ) < 0)
    goto sortie ;

  if ((f = malloc (sizeof(frame))) == NULL)
    goto sortie ;

  if ((creer_entete_frame(&ENTETE_FRAME(f), TYPE_GESTION_INTERNE ,
			  ST_GI_CC_INITIALISE_CONNECTION_PARK , 0 , 0 )) < 0)
    goto sortie ;

  if (ecrire_trt_liste (f , NULL) < 0)
    goto sortie ;

  sem_post (&sem_trt);

  pthread_join(trt , (void **)&res) ;
  pthread_join(lec , (void **)&res);
  pthread_join(ecr , (void **)&res);
  pthread_join(lg , (void **)&res);

  close(sock);
  r = 0 ;
 sortie :
  return r ;
}
