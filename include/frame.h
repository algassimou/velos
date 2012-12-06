#ifndef __FRAME__
#define __FRAME__

#include "liste.h"


typedef struct {
  int8_t type ;
  int8_t sous_type ;
  int32_t offset ;
  int16_t longueur ;
} entete_frame ;

#define TYPE_FRAME(ef) ((ef)->type)
#define SOUS_TYPE_FRAME(ef) ((ef)->sous_type)
#define OFFSET_FRAME(ef) ((ef)->offset)
#define LONGUEUR_DATA(ef) ((ef)->longueur)

typedef struct {
  entete_frame * ef ;
  void * data ;
} frame ;

#define ENTETE_FRAME(f) ((f)->ef)
#define DATA_FRAME(f) ((f)->data)

/* type frame */
#define TYPE_GESTION_CONNEXION 1
#define TYPE_GESTION_TELECHARGEMENT 2
#define TYPE_GESTION_VELO 3
#define TYPE_GESTION_CONTRAT 4
#define TYPE_GESTION_INTERNE 5
#define TYPE_GESTION_ERREUR 6

/* gestion connexion */
#define ST_GC_DEMANDE_CONNEXION 1
#define ST_GC_DEMANDE_CONNEXION_SECURISE 2
#define ST_GC_CONNEXION_ACCEPTER 3
#define ST_GC_DEMANDE_DECONNEXION 4
#define ST_GC_DECONNEXION_ACCEPTER 5

/* gestion telechargement */
#define ST_GT_DEB_TRANS_CONTRAT 1
#define ST_GT_DEB_TRANS_CONTRAT_OK 2
#define ST_GT_TRANS_BLOCK_CONTRAT 3
#define ST_GT_RECEP_BLOCK_CONTRAT 4
#define ST_GT_INTERROMPT_TRANS 5
#define ST_GT_INTERROMPT_TRANS_OK 6
#define ST_GT_FIN_TRANS_CONTRAT 7
#define ST_GT_FIN_TRANS_CONTRAT_OK 8
#define ST_GT_TRANSIT_BLOCK 9

/* gestion velos */
#define ST_GV_CHOIX_VELO 1
#define ST_GV_LOCATION_VELO 2
#define ST_GV_VELOS_A_LOUER 3
#define ST_GV_PAS_VELOS_A_LOUER 4
#define ST_GV_VELO_LOUER 5
#define ST_GV_ADRESSE_SERVEUR 6
#define ST_GV_PARKS_RETOUR 7
#define ST_GV_RECUPERE_VELO 8

/* gestion contrat */
#define ST_GCT_DEMANDE_CONTRAT 1
#define ST_GCT_CONTRAT_ACCEPTER 2
#define ST_GCT_CONTRAT_REFUSER 3
#define ST_GCT_RECEPTION_CONTRAT 4
#define ST_GCT_REPONSE_CONTRAT 5

/* gestion interne */

/* park */
#define ST_GI_PP_INITIALISE_CONNECTION_SERVEUR 1
#define ST_GI_PP_SET_SOCKET_CLIENT 2
#define ST_GI_DECONNECTION 3
#define ST_GI_CC_SOMMEIL 4
/* client */
#define ST_GI_CC_INITIALISE_CONNECTION_PARK 1

/*client */

/* serveur  */

#define SET_ENTETE_FRAME(e,t,st,o,l)		\
  do						\
    {						\
      TYPE_FRAME(e) = t ;			\
      SOUS_TYPE_FRAME(e) = st ;			\
      OFFSET_FRAME(e) = o ;			\
      LONGUEUR_DATA(e) = l ;			\
    } while(0)					\

extern int envoi_frame (int , entete_frame * ,void *);
extern int reception_frame (int , entete_frame *,void **);
extern int creer_entete_frame(entete_frame ** ,int8_t ,int8_t , int32_t , int16_t );
extern void destroy_frame (void *);
extern int ecrit_frame_liste (frame * , Liste * , int );
extern int ecrit_frame_socket (frame * , int );

#endif /*__FRAME__*/
