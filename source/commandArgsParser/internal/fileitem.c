
/* ========================================================= */

#include "commandArgsParser/internal/fileitem.h"
#include "commandArgsParser/internal/strcopy.h"

#include <stdlib.h>

/* ========================================================= */

struct fileitem* CAPINT_freeFile( struct fileitem *File )
{
  struct fileitem *Next = NULL;

  if ( File == NULL )
    return NULL;

  Next = File->Next;

  free( File->Value );
  free( File );
  return Next;
}

/* --------------------------------------------------------- */

struct fileitem* CAPINT_createFile( const char *Name )
{
  struct fileitem *File = NULL;

  if ( Name == NULL )
    return NULL;
  
  File = malloc( sizeof(*File) );
  if ( File == NULL )
    return NULL;

  File->Value = CAPINT_strcopy( Name );
  if ( File->Value == NULL )
  {
    free(File);
    return NULL;
  }

  return File;
}

/* --------------------------------------------------------- */

struct fileitem* CAPINT_copyFile( const struct fileitem *File )
{
  if ( File == NULL )
    return NULL;

  return CAPINT_createFile( File->Value );
}

/* ========================================================= */

