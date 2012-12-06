#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <getopt.h>

#include "client.h"
#include "lecture.h"
#include "ecriture.h"

int port = PORT , temps = TEMPS , fixe = FIXE  ;
char * adresse_park = ADDR_PARK ;
int opterr , optind , optopt ;
char * optarg ;

int creer_socket (char * adresse , int port )
{
  int socket_descriptor , longueur ;
  sockaddr_in adresse_locale ;

  if ((socket_descriptor = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
      perror("Socket()");
      return -1;
    }

  memset(&adresse_locale , 0 , sizeof(sockaddr_in));    
  adresse_locale.sin_family = AF_INET ; 
  adresse_locale.sin_port = htons(port);

  if((inet_aton(adresse , &(adresse_locale.sin_addr))) == -1)
    {
      perror("inet_aton()");
      return -1 ;
    }
  
  if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(sockaddr_in))) < 0) {
    perror("erreur : impossible de se connecter.");
    return -1 ;
  }
  return socket_descriptor ;
}

int creer_connection (char *adresse , int port , Liste * lrcp , sem_t *sem_rcp)
{
  Param_Lecture *pl = NULL ;
  Param_ecriture *pe = NULL ;
  int sock ;

  if ((sock = creer_socket (adresse_park , port)) < 0)
    return -1 ;

  sem_init (&sem_ecr , 0 , 0);
   
  file_ecriture = malloc (sizeof(Liste)) ;
  if (file_ecriture == NULL)
    return -1 ;

  liste_init (file_ecriture , NULL) ;

  if ((pl = creer_param_lecture(file_ecriture , lrcp  ,
				&sem_trt , &sem_ecr , sem_rcp , &sem_log , 
				sock , NULL )) == NULL)
				
    return -1 ;

  if ((pe = creer_param_ecriture(sock , file_ecriture , &sem_ecr ,
				 &sem_log )) == NULL)
    return -1 ;
      
  pthread_create(&lec , NULL , lecture ,pl );
  pthread_create(&ecr , NULL , ecriture , pe );

  return sock ;
}



int choix_velos (void * data , int s)
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

int get_line_cmd (int argc , char ** argv , const char *opt)
{
  int c ;
  opterr = 0 ;

  while ((c = getopt(argc , argv , opt)) != -1)
    {
      switch (c) {
      case 'a' :
	adresse_park = optarg ;
	break ;
      case 'p':
	if ((port = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 't':
	if ((temps = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'f' :
	fixe = 1 ;
      case 'h' :
	return -1 ;
      default :
	return -1 ;
      }
    }
  return 0 ;
}
