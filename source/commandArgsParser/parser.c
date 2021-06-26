
/* ========================================================= */

#include "commandArgsParser/parser.h"
#include "commandArgsParser/internal/strcopy.h"
#include "commandArgsParser/internal/option.h"
#include "commandArgsParser/internal/fileitem.h"
#include "commandArgsParser/internal/map.h"
#include "commandArgsParser/internal/parser.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ========================================================= */

const char* const COMMAND_ARGS_PARSER_MAP_EXISTS = "(COMMAND_ARGS_PARSER_MAP_EXISTS)";
const char* const COMMAND_ARGS_PARSER_MAP_ERROR  = "(COMMAND_ARGS_PARSER_MAP_ERROR)";

/* ========================================================= */

struct commandArgsParser* commandArgsParserCreate( void )
{
  struct commandArgsParser *Result;

  Result = malloc( sizeof(*Result) );
  if ( Result == NULL )
    return NULL;

  Result->OptionsList = NULL;
  return Result;
}

/* --------------------------------------------------------- */

void commandArgsParserDelete( struct commandArgsParser *Parser )
{
  struct option *Current = NULL;

  if ( Parser == NULL )
    return;

  Current = Parser->OptionsList;
  while ( Current != NULL )
    Current = CAPINT_freeOption(Current);

  memset( Parser, 0, sizeof(*Parser) );
  free(Parser);
}

/* --------------------------------------------------------- */

void commandArgsParserAddOption( struct commandArgsParser *Parser, char Short, const char *Long, int HasArgument )
{
  struct option *Option;

  if ( Parser == NULL )
    return;

  if ( Short == COMMAND_ARGS_PARSER_NO_SHORT && Long == COMMAND_ARGS_PARSER_NO_LONG )
    return;

  Option = CAPINT_createOption( Short, Long, HasArgument ? COMMAND_ARGS_PARSER_HAS_ARGUMENT : COMMAND_ARGS_PARSER_NO_ARGUMENT );
  if ( Option == NULL )
    return;

  Option->Next = Parser->OptionsList;
  Parser->OptionsList = Option;
}

/* --------------------------------------------------------- */

int commandArgsParserIsShortOptionExists( const struct commandArgsParser *Parser, char Short )
{
  const struct option *Option = NULL;

  if ( Parser == NULL )
    return 0;

  Option = CAPINT_findShortOptionInList( Parser->OptionsList, Short );

  if ( Option == NULL )
    return 0;
  return 1;
}

/* --------------------------------------------------------- */

int commandArgsParserIsLongOptionExists( const struct commandArgsParser *Parser, const char *Long )
{
  const struct option *Option = NULL;
  
  if ( Parser == NULL )
    return 0;

  Option = CAPINT_findLongOptionInList( Parser->OptionsList, Long );

  if ( Option == NULL )
    return 0;
  return 1;
}

/* --------------------------------------------------------- */

struct commandArgsParsedMap* commandArgsParserParse( const struct commandArgsParser *Parser, char **argv )
{
  struct commandArgsParsedMap *Map;
  int Shift;

  if ( argv == NULL )
    return NULL;

  Map = malloc( sizeof(*Map) );
  if ( Map == NULL )
    return NULL;
  Map->ProgramName = NULL;
  Map->OptionsList = NULL;
  Map->FileList = NULL;
  Map->FileListVector = NULL;

  if ( argv[0] == NULL )
    return Map;

  Map->ProgramName = CAPINT_strcopy( argv[0] );
  if ( Map->ProgramName == NULL )
    return Map;

  ++argv;
  while ( 1 )
  {
    Shift = CAPINT_parseNextArgument( Map, Parser, argv );
    if ( Shift <= 0 )
      break;
    argv += Shift;
  }

  CAPINT_updateMapFileVectorFromList(Map);

  if ( 0 )
    CAPINT_debugPrintMap(Map);

  return Map;
}

/* --------------------------------------------------------- */

void commandArgsParsedMapDelete( struct commandArgsParsedMap *Map )
{
  struct option *CurrentOption = NULL;
  struct fileitem *CurrentFile = NULL;

  if ( Map == NULL )
    return;

  CurrentOption = Map->OptionsList;
  while ( CurrentOption != NULL )
    CurrentOption = CAPINT_freeOption(CurrentOption);

  CurrentFile = Map->FileList;
  while ( CurrentFile != NULL )
    CurrentFile = CAPINT_freeFile(CurrentFile);
  
  free( Map->ProgramName );
  free( Map->FileListVector );

  memset( Map, 0, sizeof(*Map) );
  free(Map);
}

/* --------------------------------------------------------- */

int commandArgsParsedMapIsSuccess( const struct commandArgsParsedMap *Map )
{
  struct option *Current = NULL;

  if ( Map == NULL )
    return 0;

  if ( Map->ProgramName == NULL )
    return 0;

  Current = Map->OptionsList;
  while ( Current != NULL )
  {
    if ( Current->Argument == COMMAND_ARGS_PARSER_MAP_ERROR )
      return 0;
    Current = Current->Next;
  }

  return 1;
}

/* --------------------------------------------------------- */

const char* commandArgsParsedMapProgramName( const struct commandArgsParsedMap *Map )
{
  if ( Map == NULL )
    return NULL;

  return Map->ProgramName;
}

/* --------------------------------------------------------- */

const char* commandArgsParsedMapShortOptionValue( const struct commandArgsParsedMap *Map, char Short )
{
  const struct option *Option = NULL;

  if ( Map == NULL )
    return NULL;

  if ( Short == COMMAND_ARGS_PARSER_NO_SHORT )
    return NULL;

  Option = CAPINT_findShortOptionInList( Map->OptionsList, Short );
  if ( Option == NULL )
    return NULL;
 
  return Option->Argument;
}

/* --------------------------------------------------------- */

const char* commandArgsParsedMapLongOptionValue( const struct commandArgsParsedMap *Map, const char *Long )
{
  const struct option *Option = NULL;

  if ( Map == NULL )
    return NULL;

  if ( Long == COMMAND_ARGS_PARSER_NO_LONG || Long == NULL )
    return NULL;

  Option = CAPINT_findLongOptionInList( Map->OptionsList, Long );
  if ( Option == NULL )
    return NULL;
 
  return Option->Argument;
}

/* --------------------------------------------------------- */

char** commandArgsParsedMapFiles( const struct commandArgsParsedMap *Map )
{
  if ( Map == NULL )
    return NULL;

  return Map->FileListVector;
}

/* ========================================================= */

