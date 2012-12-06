#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <net/netopt.h>
#include <netinet/in.h>

#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#include "traitement.h"
#include "telechargement.h"
#include <string.h>
#include "frame.h"
#include "log.h"
#include "park.h"

static int recupere_liste_velos (void ** data)
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


  *data = malloc (s * sizeof(int));
  if (*data == NULL)
    {
      fprintf(stderr , "Impossible allouer data pour les velos ");
      return -1 ;
    }
  elt = LISTE_TETE(liste_velos);
  int * val = *data , cpt = 0 ;
  while(elt != NULL)
    {
      val[cpt] = * (int *) LISTE_ELEM_DATA(elt);
      cpt++ ;
      elt = LISTE_ELEM_NEXT(elt);
    }

  sem_post(&(LISTE_SEMAPHORE(liste_velos)));
  return s * sizeof(int) ;
}

static int initialisation_liste_velos (int * data , int s)
{
  int cpt = 0 ;
  int * val = NULL ;

  sem_wait (&LISTE_SEMAPHORE(liste_velos));
  for ( ; cpt < s ; cpt++)
    {
      if ((val = malloc (sizeof(int))) == NULL)
	return -1 ;

      *val = data[cpt] ;

      if (liste_add (liste_velos , NULL , val ) < 0)
	{
	  sem_post (&LISTE_SEMAPHORE(liste_velos));
	  return -1 ;
	}
    }
  sem_post (&LISTE_SEMAPHORE(liste_velos));
  return 0 ;
}


static int recupere_velo_num (int n)
{
  ListElem * elt = NULL , * pred = NULL ;
  int * val = NULL ;

  sem_wait(&(LISTE_SEMAPHORE(liste_velos)));
  elt = LISTE_TETE(liste_velos);

  while (n--)
    {
      pred = elt ;
      elt = LISTE_ELEM_NEXT(elt) ;
    }

  liste_rm (liste_velos , pred , (void **) &val);
  sem_post(&(LISTE_SEMAPHORE(liste_velos)));
  fprintf(stdout , "velo loue %d\n", *val);
  return  *val ;
}

int trt_velo (Traitement *t)
{
  frame *f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;
  int velo_loue = 0 , size_adr = 0 ;

  f = TRAITEMET_FRAME(t);
  ef = ENTETE_FRAME(f);
  data = DATA_FRAME(f) ;


  switch (SOUS_TYPE_FRAME(ef))
    {
    case ST_GV_LOCATION_VELO :
      LONGUEUR_DATA(ef) = recupere_liste_velos(&(DATA_FRAME(f))) ;
      
      if (LONGUEUR_DATA(ef) < 0)
	return -1 ;
      else if (LONGUEUR_DATA(ef) == 0)
	SOUS_TYPE_FRAME(ef) = ST_GV_PAS_VELOS_A_LOUER ;
      else 
	SOUS_TYPE_FRAME(ef) = ST_GV_VELOS_A_LOUER ;

      OFFSET_FRAME(ef) = 0 ;
      
      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;

      sem_post(&sem_ecr_cli);
      break ;
    case ST_GV_CHOIX_VELO :
      SET_ENTETE_FRAME(ef,TYPE_GESTION_VELO , ST_GV_LOCATION_VELO ,
		       0 , sizeof(int));

      velo_loue = recupere_velo_num (*(int *) DATA_FRAME(f));
      * (int *) DATA_FRAME(f) = velo_loue ;
      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;

      sem_post(&sem_ecr_cli);

      if ((f = malloc (sizeof (frame))) == NULL)
	return -1 ;

      if (creer_entete_frame(&ENTETE_FRAME(f) , TYPE_GESTION_VELO ,
			     ST_GV_VELO_LOUER , 0 , sizeof (int)) < 0)
	return -1 ;

      if ((DATA_FRAME(f) = malloc (sizeof(int))) == NULL)
	return -1 ;

      * (int *) DATA_FRAME(f) = velo_loue ;

      if (ecrit_frame_liste (f , file_ecriture_serveur , 0) < 0)
	return -1 ;

      sem_post (&sem_ecr_srv);

      if ((f = malloc (sizeof (frame))) == NULL)
	return -1 ;

      size_adr = strlen(adresse_serveur) + 1 ;

      if (creer_entete_frame(&ENTETE_FRAME(f) , TYPE_GESTION_VELO ,
			     ST_GV_ADRESSE_SERVEUR , 0 , 
			     size_adr + sizeof(int) ) < 0)
	return -1 ;
      
      if ((DATA_FRAME(f)=malloc(sizeof(int) + size_adr)) == NULL)
	return -1 ;

      memcpy (DATA_FRAME(f), &port_serveur , sizeof(int));
      memcpy (DATA_FRAME(f) + sizeof(int) , adresse_serveur , size_adr);

      if (ecrit_frame_liste (f , file_ecriture_client , 0) < 0)
	return -1 ;

      sem_post (&sem_ecr_cli);
      break ;
    case ST_GV_VELOS_A_LOUER :
      if (initialisation_liste_velos ((int *)DATA_FRAME(f) , LONGUEUR_DATA(ef)/sizeof(int)) < 0)
	return -1 ;

      free(DATA_FRAME(f));
      size_adr = strlen (adresse_locale) + 1 ;
      SET_ENTETE_FRAME (ef , TYPE_GESTION_CONNEXION , ST_GC_ADR_PRT_PARK , 0 ,
			size_adr + sizeof (int));

      if ((DATA_FRAME(f)=malloc(sizeof(int) + size_adr)) == NULL)
	return -1 ;

      ioctl(fd, SIOCGIFADDR, (caddr_t)&ifr, sizeof(struct ifreq));

      memcpy (DATA_FRAME(f), &port_client , sizeof(int));
      memcpy (DATA_FRAME(f) + sizeof(int) , adresse_locale , size_adr);

      break ;
    case ST_GV_PAS_VELOS_A_LOUER :
      SET_ENTETE_FRAME (ef , TYPE_GESTION_CONNEXION , ST_GC_DEMANDE_DECONNEXION ,
			0 , 0);
      if (ecrit_frame_liste (f, file_ecriture_serveur , 0) < 0)
	return -1 ;
      
      sem_post (&sem_ecr_srv);
      break ;
    }
  return 0 ;
}
