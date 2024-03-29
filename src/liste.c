#include "liste.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Initialisation d'une liste 
 * size = 0 et les pointeur tete queue a null
 */
int liste_init (Liste *l , void (*rm)(void *))
{
  if (l == NULL) return -1 ;
  
  LISTE_SIZE(l) = 0 ;
  LISTE_TETE(l) = NULL ;
  LISTE_QUEUE(l) = NULL ;
  LISTE_REMOVE(l) = rm ;
  sem_init(&LISTE_SEMAPHORE(l), 0, 1);
  return 0 ;
}


/*
 * ajout d'un element dans la liste 
 * si elt est egale a null ajout en debut de liste 
 * sinon ajout apres elt 
 */

int liste_add (Liste * l , ListElem * elt , const void * data)
{
  ListElem * new = NULL ;
  new = malloc(sizeof(ListElem));
  if (new == NULL) return -1 ;
  
  LISTE_ELEM_DATA(new) = (void *)data ;
  LISTE_ELEM_NEXT(new) = NULL ;

  if (LISTE_SIZE(l) == 0)
    {
      LISTE_TETE(l) = new ;
      LISTE_QUEUE(l) = new ;
    }
  else 
    {
      if (elt == NULL)
	{
	  LISTE_ELEM_NEXT(new)  = LISTE_TETE(l) ;
	  LISTE_TETE(l) = new ;
	}
      else 
	{
	  LISTE_ELEM_NEXT(new) = LISTE_ELEM_NEXT(elt) ;
	  LISTE_ELEM_NEXT(elt) = new ;
	  if (LISTE_ELEM_NEXT(new) == NULL)
	    LISTE_QUEUE(l) = new ;
	}
    }
  LISTE_SIZE(l) ++ ;
  return 0 ;
}

int liste_rm (Liste * l , ListElem * elt , void ** data)
{
  ListElem * old = NULL ;

  if (LISTE_SIZE(l) == 0) 
      return 0 ;

  if (elt == NULL)
    {
      old  = LISTE_TETE(l) ;
      LISTE_TETE(l) = LISTE_ELEM_NEXT(LISTE_TETE(l)) ;
      if (LISTE_SIZE(l) == 1)
	LISTE_QUEUE(l) = NULL ;
    }
  else
    {
      old = LISTE_ELEM_NEXT(elt) ;
      if (old == NULL) 
	  return 0 ;

      LISTE_ELEM_NEXT(elt) = LISTE_ELEM_NEXT(old);
      if (LISTE_ELEM_NEXT(old) == NULL)
	{
	  LISTE_QUEUE(l) = elt ;
	}
    }
  LISTE_SIZE(l) -- ;

  if (data != NULL)
    *data = LISTE_ELEM_DATA(old);
  free(old) ;
  return 0 ;
}

int liste_destroy (Liste * l)
{
  void * old ;

  while (LISTE_SIZE(l))
    {
      liste_rm(l , NULL , &old);
      if (LISTE_REMOVE(l))
	LISTE_REMOVE(l)(old);
    }
  memset(l , 0 , sizeof(Liste));
  return 0 ;
}
