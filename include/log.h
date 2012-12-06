#ifndef __LOG__
#define __LOG__

#include <semaphore.h>
#include "liste.h"
#include <stdio.h>
#include <time.h>

enum type_log { LOG_GENERAL , LOG_CONNEXION , LOG_STAT , LOG_DECONNEXION } ;

typedef struct {
  int id ;
  enum type_log t ;
  char * mesg ;
  time_t time ;
} Log ;

#define LOG_ID(l) ((l)->id)
#define LOG_TYPE(l) ((l)->t)
#define LOG_MESG(l) ((l)->mesg)
#define LOG_TIME(l) ((l)->time)
#define TIME_BUF_SIZE 26

typedef struct {
  FILE * f_general ;
  FILE * f_connexion ;
  FILE * f_stat ;
} Param_log ;

#define PARAM_LOG_FG(pr) ((pr)->f_general)
#define PARAM_LOG_FC(pr) ((pr)->f_connexion)
#define PARAM_LOG_FS(pr) ((pr)->f_stat)

extern void * logs (void *) ;
extern void destroy_log (void *) ;
extern int log_mesg (int , int , enum type_log , char * , ...);
extern Param_log* creer_param_log (FILE * , FILE * , FILE * );

extern Liste *file_logs ;
extern sem_t sem_log ;

#define LOG_ENTETE_FRAME(fr,id,taille,type_log,chaine)		\
  log_mesg (id , taille , type_log , chaine , TYPE_FRAME(ENTETE_FRAME(fr)) , SOUS_TYPE_FRAME(ENTETE_FRAME(fr)) ,  OFFSET_FRAME(ENTETE_FRAME(fr)) , LONGUEUR_DATA(ENTETE_FRAME(fr)))

#endif 
