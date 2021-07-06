
/* ========================================================= */

#include "commandArgsParser/internal/map.h"
#include "commandArgsParser/internal/parser.h"
#include "commandArgsParser/internal/fileitem.h"
#include "commandArgsParser/internal/option.h"
#include "commandArgsParser/internal/optiontype.h"
#include "commandArgsParser/parser.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ========================================================= */

int CAPINT_parseNextArgumentOptionTypeShortList( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv )
{
  const struct option *Option = NULL;

  const char *Current = argv[0] + 1;
  const char *Argument = NULL;

  size_t i;

  for ( i = 0; ; i++ )
  {
    if ( Current[i] == '\0' )
      return 1;

    Option = CAPINT_findShortOptionInList( Parser->OptionsList, Current[i] );
    if ( Option == NULL )
    {
      CAPINT_appendOptionErrorShortToMap( Map, Current[i] );
      continue;
    }

    if ( Option->Argument == COMMAND_ARGS_PARSER_HAS_ARGUMENT )
    {
      if ( Current[i+1] != '\0' )
      {
        CAPINT_appendOptionErrorShortToMap( Map, Current[i] );
        continue;
      }

      Argument = argv[1];
      if ( Argument == NULL )
      {
        CAPINT_appendOptionErrorShortToMap( Map, Current[i] );
        return 1;
      }
    
      CAPINT_appendOptionWithArgumentToMap( Map, Option, Argument );
      return 2;
    }
  
    CAPINT_appendOptionWithArgumentToMap( Map, Option, COMMAND_ARGS_PARSER_MAP_EXISTS );
  }
}

/* --------------------------------------------------------- */

int CAPINT_parseNextArgumentOptionTypeLong( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv )
{
  const struct option *Option = NULL;

  const char *Current = argv[0] + 2;
  const char *Argument = NULL;

  Option = CAPINT_findLongOptionInList( Parser->OptionsList, Current );
  if ( Option == NULL )
  {
    CAPINT_appendOptionErrorLongToMap( Map, Current );
    return 1;
  }

  if ( Option->Argument == COMMAND_ARGS_PARSER_HAS_ARGUMENT )
  {
    Argument = argv[1];
    if ( Argument == NULL )
    {
      CAPINT_appendOptionErrorLongToMap( Map, Current );
      return 1;
    }
  
    CAPINT_appendOptionWithArgumentToMap( Map, Option, Argument );
    return 2;
  }

  CAPINT_appendOptionWithArgumentToMap( Map, Option, COMMAND_ARGS_PARSER_MAP_EXISTS );
  return 1;
}

/* --------------------------------------------------------- */

int CAPINT_parseNextArgumentOptionTypeArg( struct commandArgsParsedMap *Map, char **argv )
{
  CAPINT_appendFileItemToMap( Map, argv[0] );
  return 1;
}

/* --------------------------------------------------------- */

int CAPINT_parseNextArgumentOptionTypeEnd( struct commandArgsParsedMap *Map, char **argv )
{
  size_t i;
  int TotalShift;

  if ( Map == NULL )
    return 0;

  assert( strcmp(argv[0],"--") == 0 );

  TotalShift = 1;
  for ( i = 1; argv[i] != NULL; i++ )
  {
    CAPINT_parseNextArgumentOptionTypeArg( Map, argv + i );
    TotalShift += 1;
  }

  return TotalShift;
}

/* --------------------------------------------------------- */

int CAPINT_parseNextArgument( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv )
{
  enum optionType Type;

  if ( Map == NULL || argv == NULL )
    return 0;

  Type = CAPINT_optionTypeOfString( argv[0] );
  switch ( Type )
  {
    case OptionTypeNull:
      return 0;

    case OptionTypeShort:
    case OptionTypeShortList:
      return CAPINT_parseNextArgumentOptionTypeShortList( Map, Parser, argv );

    case OptionTypeLong:
      return CAPINT_parseNextArgumentOptionTypeLong( Map, Parser, argv );

    case OptionTypeArg:
      return CAPINT_parseNextArgumentOptionTypeArg( Map, argv );

    case OptionTypeEnd:
      return CAPINT_parseNextArgumentOptionTypeEnd( Map, argv );
  }

  return -1;
}

/* ========================================================= */

