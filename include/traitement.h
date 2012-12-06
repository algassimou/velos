#ifndef __TRAITEMENT__
#define __TRAITEMENT__

#include <semaphore.h>
#include "frame.h"

typedef struct {
  void * a ;
  frame * f ;
} Traitement ;

#define TRAITEMET_AUTRE(t) ((t)->a)
#define TRAITEMET_FRAME(t) ((t)->f)

extern Liste * file_traitement ;
extern sem_t sem_trt ;
extern int dec  ;

extern void * traitement (void *) ;
extern int trt_connexion (Traitement *);
extern int trt_velo (Traitement *);
extern int trt_contrat (Traitement *);
extern int trt_interne (Traitement *);
extern int trt_erreur (Traitement *);

extern int ecrire_trt_liste (frame *, void * ) ;
#endif /*__TRAITEMENT__*/
