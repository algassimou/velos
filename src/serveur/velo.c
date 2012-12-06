#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

#include "traitement.h"
#include "telechargement.h"
#include "frame.h"
#include "log.h"
#include "serveur.h"


static int get_parks (void ** data)
{
  DListElem * delt = NULL ;
  int s = 0 , sock = 0 ;
  struct sockaddr name ;
  socklen_t length = 0 ;
  Comm * c = NULL ;

  sem_wait(&DLISTE_SEM(dliste_connexion));
  s = DLISTE_SIZE(dliste_connexion) ;

  if ((*data = malloc (s * sizeof(int))) == NULL)
    return -1 ;

  delt = DLISTE_HEAD (dliste_connexion) ;
  while (delt != NULL)
    {
      c = DLISTE_ELEM_DATA (delt);
      sock = COMM_SOCKET(c) ;
      getsockname (sock , (struct sockaddr *) &name , &length);
      delt = DLISTE_ELEM_NEXT(delt);
    }
  sem_post(&DLISTE_SEM(dliste_connexion));
  return s * sizeof (int);
}

static int recupere_liste_velos (void ** data , int t)
{
  ListElem * elt = NULL ;
  int s = 0 ;

  sem_wait(&(LISTE_SEMAPHORE(liste_velos)));
  s = LISTE_SIZE(liste_velos) ;

  if (s == 0)
    {
      *data = NULL ;
      sem_post(&(LISTE_SEMAPHORE(liste_velos)));
      return 0 ;
    }
  if ( t > s)
    t = s ;

  *data = malloc (t * sizeof(int));
  if (*data == NULL)
    {
      fprintf(stderr , "Impossible allouer data pour les velos ");
      return -1 ;
    }
  elt = LISTE_TETE(liste_velos);
  int * val = *data , cpt = 0 ;
  while(elt != NULL && t--)
    {
      val[cpt] = * (int *) LISTE_ELEM_DATA(elt);
      cpt++ ;
      elt = LISTE_ELEM_NEXT(elt);
    }

  sem_post(&(LISTE_SEMAPHORE(liste_velos)));
  return cpt * sizeof(int) ;
}


int trt_velo (Traitement *t)
{
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;
  Comm * c = NULL ;
  int s = 0 ;

  f = TRAITEMET_FRAME(t);
  c = TRAITEMET_AUTRE(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;


  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GV_VELO_LOUER :
      if (log_mesg (0 , 20 , LOG_STAT ,"Location velo %d" 
		    , *(int *)data) < 0)
	return -1 ;
      destroy_frame(f) ;
      break ;
    case ST_GV_LOCATION_VELO :
      s = get_parks (&DATA_FRAME(f));
      SET_ENTETE_FRAME(ef , TYPE_GESTION_VELO , ST_GV_PARKS_RETOUR , 0 , s);
      ecrit_frame_liste (f , COMM_LISTE_ECR(c) , 0);
      sem_post(&COMM_SEM_ECR(c));
      break ;
    case ST_GV_RECUPERE_VELO :
      SET_ENTETE_FRAME(ef , TYPE_GESTION_VELO , ST_GV_VELOS_A_LOUER , 0 , 0) ;
      s = * (int *) DATA_FRAME(f) ;

      free (DATA_FRAME(f));

      LONGUEUR_DATA(ef) = recupere_liste_velos(&(DATA_FRAME(f)) , s) ;
      
      if (LONGUEUR_DATA(ef) < 0)
	return -1 ;
      else if (LONGUEUR_DATA(ef) == 0)
	SOUS_TYPE_FRAME(ef) = ST_GV_PAS_VELOS_A_LOUER ;

      OFFSET_FRAME(ef) = 0 ;
      
      if (ecrit_frame_liste (f , COMM_LISTE_ECR(c) , 0) < 0)
	return -1 ;

      sem_post(&COMM_SEM_ECR(c));

      break ;
    }
  return 0 ;
}
