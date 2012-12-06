#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "liste.h"
#include "log.h"
#include "frame.h"
#include "traitement.h"
#include "lecture.h"
#include "ecriture.h"
#include "telechargement.h"
#include "park.h"


Liste *file_reception , *file_ecriture_client ;
Liste *file_ecriture_serveur , *file_envoi , *liste_velos ;
sem_t sem_ecr_srv , sem_ecr_cli , sem_rcp , sem_ev;
sem_t sem_accept_connexion ;
pthread_t lec_srv , ecr_srv , lec_cli , ecr_cli , trt , lg , recep , env ;
int affiche = 1 ;

int main (int argc , char * argv[])
{
  Param_log * p = NULL ;
  Param_Lecture *pls = NULL ;
  Param_ecriture *pes = NULL ;
  Param_Reception *pr = NULL ;
  Param_Envoi *pe = NULL ;

  sockaddr_in adr_p , adr_cli ;
  socklen_t adr_cli_len ;

  FILE * fg = NULL , *fc = NULL , *fs = NULL ;
  int *res = NULL , cli , sock_srv , sock_cli ; 
  frame * f = NULL ;

  if (get_line_cmd (argc , argv , LINE_OPTION) == -1)
    {
      fprintf (stderr , "%s \n" , HELP);
      return -1 ;
    }

  if ((sock_srv = creer_socket_serveur (adresse_serveur , port_serveur)) < 0)
    return -1 ;
      
  sem_init (&(sem_trt) , 0 , 0);
  sem_init (&(sem_log) , 0 , 0);
  sem_init (&sem_ecr_srv , 0 , 0);
  sem_init(&sem_accept_connexion , 0 , 0);

  file_traitement = malloc(sizeof(Liste));
  if (file_traitement == NULL)
    return -1 ;

  liste_init(file_traitement , NULL );

  file_ecriture_serveur = malloc (sizeof(Liste)) ;
  if (file_ecriture_serveur == NULL)
    return -1 ;

  liste_init (file_ecriture_serveur , NULL) ;  

  liste_velos = malloc (sizeof(Liste)) ;
  if (liste_velos == NULL)
    return -1 ;

  liste_init (liste_velos , NULL) ;

  file_reception = malloc (sizeof(Liste)) ;
  if (file_reception == NULL)
    return -1 ;

  liste_init (file_reception , NULL) ;

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
    return -1 ;

  if ((pls = creer_param_lecture(file_ecriture_serveur , file_reception , 
				  &sem_trt , &sem_ecr_srv , &sem_rcp , &sem_log ,
				 sock_srv , NULL )) == NULL)
    return -1 ;

  if ((pes = creer_param_ecriture(sock_srv , file_ecriture_serveur ,
				 &sem_ecr_srv , &sem_log)) == NULL)
    return -1 ;
  
  
  if ((res = malloc (sizeof(int))) == NULL)
    return -1 ;

  pthread_create(&lec_srv , NULL , lecture ,pls );
  pthread_create(&ecr_srv , NULL , ecriture , pes );
  pthread_create(&trt , NULL , traitement , NULL );
  pthread_create(&lg , NULL , logs , p );

  /* Intialisation connexion serveur */
  if ((f = malloc (sizeof(frame))) == NULL)
    goto sortie ;

  if ((creer_entete_frame(&ENTETE_FRAME(f), TYPE_GESTION_INTERNE ,
			  ST_GI_PP_INITIALISE_CONNECTION_SERVEUR , 0 , 0 )) < 0)
    goto sortie ;

  if (ecrire_trt_liste (f , NULL) < 0)
    goto sortie ;

  sem_post (&sem_trt);
  

  if ((sock_cli = socket (AF_INET , SOCK_STREAM , 0)) == -1)
    {
      perror("erreur : impossible de creer la socket de connexion avec le client.");
      exit(-1);
    }

  printf ("%s %d %d %d\n", adresse_serveur , port_serveur , port_client , id);
    
  adr_p.sin_family = AF_INET ;
  adr_p.sin_addr.s_addr = INADDR_ANY ;
  adr_p.sin_port = htons(port_client);
  
  if ((bind(sock_cli ,(sockaddr *) &adr_p ,sizeof(sockaddr_in))) == -1)
    {
      perror("Bind()");
      return -1;
    }

  if ((listen(sock_cli , 5)) == -1)
    {
      perror("Listen()");
      return -1;
    }

  while(1)
    {
      frame * f = NULL ;
      sem_wait(&sem_accept_connexion) ;

      if ((cli=accept(sock_cli,(sockaddr*) &adr_cli ,&adr_cli_len)) < 0)
	{
	  perror("Accept()");
	  goto sortie ;
	}	

      if ((f=malloc(sizeof(frame))) == NULL)
	goto sortie ;
	      
      if (creer_entete_frame (&ENTETE_FRAME(f) ,TYPE_GESTION_INTERNE ,
			      ST_GI_PP_SET_SOCKET_CLIENT , 0 , sizeof(int)) < 0)
	goto sortie ;

      if ((DATA_FRAME(f) = malloc (sizeof(int))) == NULL)
	goto sortie ;

      *(int *)DATA_FRAME(f) = cli ;
 
      ecrire_trt_liste(f,NULL);
      sem_post(&sem_trt);
      
      /*if (LOG_ENTETE_FRAME (f , 0, 29, LOG_GENERAL ,"PRK con cli %d %d %d %d") < 0*/
    }
 sortie :
  close(sock_srv);
  return -1 ;
}
