#include "log.h"
#include "frame.h"
#include "traitement.h"
#include "serveur.h"
#include "lecture.h"
#include "ecriture.h"


int trt_erreur (Traitement *t)
{
  return 0 ;
}


int trt_interne (Traitement * t)
{
  frame * f = NULL ;
  entete_frame * ef = NULL ;
  void * data = NULL ;
  Comm * c = NULL ;

  f = TRAITEMET_FRAME(t) ;
  c = TRAITEMET_AUTRE(t);
  ef = ENTETE_FRAME(f) ;
  data = DATA_FRAME(f) ;

  switch (SOUS_TYPE_FRAME(ef))
    {
    default :
      break ;
    }
  return 0 ;
}
