
/* ========================================================= */

#include "commandArgsParser/internal/strcopy.h"

#include <string.h>
#include <stdlib.h>

/* ========================================================= */

char* CAPINT_strcopy( const char *String )
{
  size_t Length;
  char *Result;

  if ( String == NULL )
    return NULL;

  Length = strlen( String );
  Result = malloc( Length + 1 );
  if ( Result == NULL )
    return NULL;

  strcpy( Result, String );
  return Result;
}

/* ========================================================= */

