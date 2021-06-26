
/* ========================================================= */

#include "commandArgsParser/internal/strcopy.h"

#include <string.h>
#include <stdlib.h>

/* ========================================================= */

char* CAPINT_strdup( const char *String )
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

/* --------------------------------------------------------- */

char* CAPINT_stpncpy( char *Destination, const char *Source, size_t DestinationSize )
{
  size_t Index;

  if ( Destination == NULL )
    return NULL;

  if ( Source == NULL )
    return Destination;

  if ( DestinationSize <= 0 )
    return Destination;

  Index = 0;
  while ( Index < DestinationSize && Source[Index] != '\0' )
  {
    Destination[Index] = Source[Index];
    ++Index;
  }

  if ( Index >= DestinationSize )
    Index = DestinationSize - 1;

  Destination[Index] = '\0';

  return Destination + Index;
}

/* ========================================================= */

