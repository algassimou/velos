#include <ctype.h>

int convert_string_int (char * s)
{
  int val = 0 ;
  int cpt = 0 ;

  while (s[cpt] != '\0' && isdigit(s[cpt]))
    {
      val = val * 10 + s[cpt] - '0' ;
      cpt++;
    }

  if (s[cpt] != '\0') return -1 ;
  return val ;
}
