
/* ========================================================= */

#include "commandArgsParser/internal/option.h"
#include "commandArgsParser/internal/strcopy.h"
#include "commandArgsParser/parser.h"

#include <stdlib.h>
#include <string.h>

/* ========================================================= */

struct option* CAPINT_freeOption( struct option *Option )
{
  struct option *Next = NULL;

  if ( Option == NULL )
    return NULL;

  Next = Option->Next;

  if ( Option->Argument != COMMAND_ARGS_PARSER_HAS_ARGUMENT   && 
       Option->Argument != COMMAND_ARGS_PARSER_NO_ARGUMENT    &&
       Option->Argument != COMMAND_ARGS_PARSER_MAP_ERROR      && 
       Option->Argument != COMMAND_ARGS_PARSER_MAP_EXISTS  
       )
    free( Option->Argument );

  if ( Option->Long != COMMAND_ARGS_PARSER_NO_LONG )
    free( Option->Long );

  free( Option );

  return Next;
}

/* --------------------------------------------------------- */

struct option* CAPINT_createOption( char Short, const char *Long, const char *Argument )
{
  struct option *Option = NULL;

  Option = malloc( sizeof(*Option) );
  if ( Option == NULL )
    return NULL;
  
  Option->Next = NULL;
  Option->Short = Short;
  Option->Long = NULL;
  Option->Argument = NULL;

  if ( Argument == COMMAND_ARGS_PARSER_NO_ARGUMENT ||
       Argument == COMMAND_ARGS_PARSER_HAS_ARGUMENT ||
       Argument == COMMAND_ARGS_PARSER_MAP_EXISTS ||
       Argument == COMMAND_ARGS_PARSER_MAP_ERROR 
       )
  {
    Option->Argument = (char*)Argument;
  } else {
    Option->Argument = CAPINT_strdup( Argument );
    if ( Option->Argument == NULL )
    {
      CAPINT_freeOption( Option );
      return NULL;
    }
  }

  if ( Long == COMMAND_ARGS_PARSER_NO_LONG )
  {
    Option->Long = COMMAND_ARGS_PARSER_NO_LONG;
  } else {
    Option->Long = CAPINT_strdup( Long );
    if ( Option->Long == NULL )
    {
      CAPINT_freeOption( Option );
      return NULL;
    }
  }

  return Option;
}

/* --------------------------------------------------------- */

struct option* CAPINT_copyOption( const struct option *Option )
{
  if ( Option == NULL )
    return NULL;

  return CAPINT_createOption( Option->Short, Option->Long, Option->Argument );
}

/* --------------------------------------------------------- */

const struct option* CAPINT_findShortOptionInList( const struct option *Current, char Short )
{
  if ( Short == COMMAND_ARGS_PARSER_NO_SHORT )
    return NULL;

  while ( Current != NULL )
  {
    if ( Current->Short == Short )
      return Current;
    Current = Current->Next;
  }

  return NULL;
}

/* --------------------------------------------------------- */

const struct option* CAPINT_findLongOptionInList( const struct option *Current, const char *Long )
{
  if ( Long == COMMAND_ARGS_PARSER_NO_LONG || Long == NULL )
    return NULL;

  while ( Current != NULL )
  {
    if ( Current->Long != NULL && strcmp( Long, Current->Long ) == 0 )
      return Current;
    Current = Current->Next;
  }

  return NULL;
}

/* ========================================================= */

