#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "log.h"

Liste * file_logs = NULL ;
sem_t sem_log ;
char * t ;

void * logs (void * v)
{
  Log * lg = NULL ;
  Param_log * p = (Param_log *) v ;

  FILE * f_g = PARAM_LOG_FG(p) ;
  FILE * f_c = PARAM_LOG_FC(p) ;
  FILE * f_s = PARAM_LOG_FS(p) ;

  if (f_g == NULL)
    f_g = stdout ;
  
  if (f_c == NULL)
    f_c = stdout ;

  if (f_s == NULL)
    f_s = stdout ;
  
  int * res = malloc (sizeof(int)) ;
  *res = -1 ;

  while (1)
    {
      lg = NULL ;
      sem_wait (&(sem_log)) ;

      sem_wait(&LISTE_SEMAPHORE(file_logs));
      liste_rm(file_logs , NULL , (void **)&lg);
      sem_post(&LISTE_SEMAPHORE(file_logs));

      if (lg != NULL)
	{
	  t = ctime(&LOG_TIME(lg)) ;
	  t[TIME_BUF_SIZE-2] = '\0' ;

	  fprintf(f_g , "log general %s: %d %s\n"  , t , LOG_ID(lg) , LOG_MESG(lg));
	  switch(LOG_TYPE(lg))
	    {
	    case LOG_CONNEXION : 
	      fprintf(f_c , "log connexion %s: %d %s\n"  , t , LOG_ID(lg) , LOG_MESG(lg));
	      break ;
	    case LOG_STAT :
	      fprintf(f_s , "log stat %s: %d %s\n"  , t , LOG_ID(lg) , LOG_MESG(lg));
	      break ;
	    case LOG_GENERAL :
	      break ;

	    case LOG_DECONNEXION :
	      fclose(f_g);
	      fclose(f_c);
	      fclose(f_s);
	      goto end ;
	    }

	  destroy_log((void *) lg);
	}
    }
 end :
  *res = 0 ;
  free(p);
  pthread_exit(res) ;
}

int log_mesg (int id , int n , enum type_log t , char * mesg , ...)
{
  Log * lg = NULL ;

  if ((lg = malloc (sizeof(Log))) == NULL)
    return -1 ;

  LOG_ID(lg) = id ;
  LOG_TYPE(lg) = t ;
  LOG_TIME(lg) = time(NULL) ;
     
  if (n)
    {
      if ((LOG_MESG(lg) = malloc (n+1)) == NULL)
	{
	  free(lg);
	  return -1 ;
	}
      va_list ap ;
      va_start (ap , mesg) ;
  
      vsnprintf (LOG_MESG(lg) , n , mesg , ap);
      LOG_MESG(lg)[n] = '\0' ;
    }
  else
    {
      if ((LOG_MESG(lg) = strdup(mesg)) == NULL)
	{
	  free(lg);
	  return -1 ;
	}
    }

  sem_wait(&(LISTE_SEMAPHORE(file_logs)));
  liste_add(file_logs , LISTE_QUEUE(file_logs) , lg);
  sem_post(&(LISTE_SEMAPHORE(file_logs)));

  sem_post(&(sem_log)) ;
  return 0 ;
}

void destroy_log (void * l)
{
  Log * lg = (Log *) l ;
  free(LOG_MESG(lg));
  free(lg);
}

Param_log * creer_param_log (FILE *f1 , FILE *f2 , FILE *f3)
{
  Param_log * p = NULL ;
  if ((p = malloc (sizeof(Param_log))) == NULL)
    return NULL ;

  PARAM_LOG_FG(p) = f1 ;
  PARAM_LOG_FC(p) = f2 ;
  PARAM_LOG_FS(p) = f3 ;

  return p ;
}
