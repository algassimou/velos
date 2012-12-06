#ifndef __LISTE__
#define __LISTE__

#include <semaphore.h>

typedef struct ListElem {
  void * data ;
  struct ListElem * next ;
} ListElem ;

#define LISTE_ELEM_DATA(le) ((le)->data)
#define LISTE_ELEM_NEXT(le) ((le)->next)

typedef struct {
  int size ;
  ListElem * tete ;
  ListElem * queue ;
  void (*remove) (void*);
  sem_t sem ;
} Liste ;

#define LISTE_SEMAPHORE(l) ((l)->sem)
#define LISTE_SIZE(l) ((l)->size)
#define LISTE_TETE(l) ((l)->tete)
#define LISTE_QUEUE(l) ((l)->queue)
#define LISTE_REMOVE(l) ((l)->remove)

int liste_init (Liste * , void (*)(void *));
int liste_add(Liste * , ListElem * , const void *);
int liste_rm (Liste * , ListElem * , void **);
int liste_rm_n (Liste * , ListElem * , void **);
int liste_destroy(Liste *);

#endif /*__LISTE__*/
