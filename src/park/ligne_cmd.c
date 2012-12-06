#include "park.h"
#include <getopt.h>

int port_client = PORT_CLIENT ;
int port_serveur = PORT_SERVEUR ;
int nb_velo = NB_VELO ;
int id = 0  , securise = SECURITY ;
char * adresse_serveur = ADDR_SERVEUR ;
int opterr , optind , optopt ;
char * optarg ;

int get_line_cmd (int argc , char ** argv , const char *opt)
{
  int c ;
  opterr = 0 ;

  while ((c = getopt(argc , argv , opt)) != -1)
    {
      switch (c) {
      case 'a' :
	adresse_serveur = optarg ;
	break ;
      case 'c':
	if ((port_client = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 's' :
	if ((port_serveur = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'i' :
	if ((id = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'v' :
	if ((nb_velo = convert_string_int(optarg)) == -1)
	  return -1 ;
	break ;
      case 'x' :
	securise = 1 ;
	break ;
      case 'h' :
	return -1 ;
      default :
	return -1 ;
      }
    }
  return 0 ;
}
  
