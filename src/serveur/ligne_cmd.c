#include "serveur.h"
#include <getopt.h>

int port=PORT , nb_velo=NB_VELO, nb_client=NB_CLIENT;
int opterr , optind , optopt ;
char * optarg ;

int get_line_cmd (int argc , char ** argv , const char *opt)
{
  int c ;
  opterr = 0 ;

  while ((c = getopt(argc , argv , opt)) != -1)
    {
      switch (c) {
      case 'p':
	if ((port = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'n' :
	if ((nb_client = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'v' :
	if ((nb_velo = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'h' :
	return -1 ;
      default :
	return -1 ;
      }
    }
  return 0 ;
}
