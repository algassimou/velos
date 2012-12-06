#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "frame.h"

int envoi_frame (int sock ,  entete_frame * ettf , void * data )
{
  /*if (log_mesg (0, 40 , LOG_GENERAL ,"envoi frame %d %d %d %d" , 
		ettf -> type , ettf -> sous_type , ettf -> offset ,
		ettf -> longueur ) < 0)
		return -1 ;*/

  if ((write(sock, (void *) ettf , sizeof(entete_frame))) < 0)
    return -1 ;
  
  if (data == NULL || ettf -> longueur == 0 ) return 0 ;

  if ((write(sock, data , ettf->longueur )) < 0)
    return -1 ;

  return 0 ;
}

int reception_frame (int sock , entete_frame * ettf , void ** data )
{
  int longueur = 0 ;
  
  if ((longueur = read(sock, (void *) ettf , sizeof(entete_frame))) < 0)
    {
      perror("error recv");
      return -1 ;
    }
  
  if (ettf->longueur == 0) return 0 ;

  if (data != NULL && *data == NULL)
    *data = malloc(ettf->longueur) ;

  if (*data == NULL) return -1 ;

  if ((longueur = read(sock, *data , ettf->longueur)) < 0)
    {
      perror("error read");
      return -1 ;
    }

  return 0 ;
}

int creer_entete_frame(entete_frame ** e ,int8_t typ ,int8_t s_typ , 
		       int32_t offt , int16_t l)
{
  if ((*e = malloc (sizeof (entete_frame))) == NULL)
    return -1 ;

  TYPE_FRAME(*e) = typ ;
  SOUS_TYPE_FRAME(*e) = s_typ ;
  OFFSET_FRAME(*e) = offt ;
  LONGUEUR_DATA(*e) = l ;

  return 0 ;
}

void destroy_frame (void * fr)
{
  frame * f = (frame *) fr ;
  if (LONGUEUR_DATA(ENTETE_FRAME(f)) && DATA_FRAME(f))
    free(DATA_FRAME(f));
  free(ENTETE_FRAME(f));
  free(f) ;

}

int ecrit_frame_socket (frame * f , int s)
{
  if (envoi_frame (s , ENTETE_FRAME(f) , DATA_FRAME(f)) < 0)
    return -1 ;

  free(ENTETE_FRAME(f));
  free(DATA_FRAME(f));
  free(f);
  return 0 ;
}

int ecrit_frame_liste (frame * f , Liste * l , int deb)
{
  int res ;

  sem_wait(&(LISTE_SEMAPHORE(l)));

  if (deb)
    res = liste_add (l , NULL , (void *) f);
  else
    res = liste_add (l , LISTE_QUEUE(l) , (void *) f);

  sem_post(&(LISTE_SEMAPHORE(l))) ;

  return res ;
}
