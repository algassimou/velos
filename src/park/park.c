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
#include <pthread.h>
#include "liste.h"
#include "frame.h"
#include "log.h"
#include "park.h"


char cle ; 

int creer_socket_serveur ()
{
  int srv ;
  sockaddr_in adresse_locale ;

  if ((srv = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
      perror("Socket() connexion serveur ");
      return -1;
    }

  memset(&adresse_locale , 0 , sizeof(sockaddr_in));
  adresse_locale.sin_family = AF_INET ; 
  adresse_locale.sin_port = htons(port_serveur);

  if((inet_aton(adresse_serveur , &(adresse_locale.sin_addr))) == -1)
    {
      perror("inet_aton() connexion serveur ");
      return -1 ;
    }
  
  if ((connect(srv , (sockaddr*)(&adresse_locale), sizeof(sockaddr_in))) < 0) {
    perror("erreur : impossible de se connecter au serveur.");
    return -1 ;
  }
  return srv ;
}

int connection_serveur ()
{
  /*  entete_frame * e = NULL ;
  void * data = NULL ;
  int sock = creer_socket_serveur() ;
  if (sock < 0) return -1 ;

  if (securise)
    {
      if (creer_entete_frame(&e , TYPE_SRV_PRK , ST_SP_CONNECTION_SECURISE , 0 , sizeof(char)) < 0)
	return -1 ;
      
      if ((data = malloc (sizeof(char))) == NULL)
	return -1 ;

      cle = genere_cle() ;
      *(char *) data = cle ;
    }
  else 
    {
      if (creer_entete_frame(&e , TYPE_SRV_PRK , ST_SP_CONNECTION_NON_SECURISE , 0 , 0) < 0)
	return -1 ;
      cle = 0 ;
    }


  if (envoi_frame(sock , e , data ) < 0)
    return -1 ;

  free(e) ;
  if (securise)
    free(data);
  printf("Connection serveur reussi \n");
  return sock ;*/
  return 0 ;
}

int deconnexion_serveur (int sock)
{
  /*  entete_frame * e = NULL ;
  if (creer_entete_frame(&e , TYPE_SRV_PRK , ST_SP_DECONNECTION , 0 , 0) < 0)
    return -1 ;

  if (envoi_frame(sock , e , NULL ) < 0)
    {
    	free(e);
    	return -1 ;
    }
    
	free(e);
  */
  return 0 ;
}
