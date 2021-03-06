
/* ========================================================= */

#include "commandArgsParser/internal/map.h"
#include "commandArgsParser/internal/strcopy.h"
#include "commandArgsParser/internal/fileitem.h"
#include "commandArgsParser/internal/option.h"
#include "commandArgsParser/parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ========================================================= */

struct commandArgsParsedMap* CAPINT_createMap( void )
{
  struct commandArgsParsedMap *Map;
  
  Map = malloc( sizeof(*Map) );
  if ( Map == NULL )
    return NULL;

  Map->ProgramName = NULL;
  Map->OptionsList = NULL;
  Map->FileList = NULL;
  Map->FileListVector = NULL;
  Map->ErrorString = NULL;

  return Map;
}

/* --------------------------------------------------------- */

void CAPINT_debugPrintMap( const struct commandArgsParsedMap *Map )
{
  const struct option *CurrentOption;
  const struct fileitem *CurrentFile;

  fprintf( stderr, "Map: %p\n", (void*)Map );
  fprintf( stderr, "Map->ProgramName: %s\n", Map->ProgramName == NULL ? "(NULL)" : Map->ProgramName );

  CurrentOption = Map->OptionsList;
  while ( CurrentOption != NULL )
  {
    const char *Long, *Argument;
    char Short[64] = { '\0' };

    if ( CurrentOption->Short == COMMAND_ARGS_PARSER_NO_SHORT ) 
    {
      strcpy( Short, "(NO SHORT)" );
    } else {
      Short[0] = CurrentOption->Short;
    }

    Long = CurrentOption->Long == COMMAND_ARGS_PARSER_NO_LONG ? "(NO LONG)" : 
           CurrentOption->Long == NULL ? "(NULL)" : 
           CurrentOption->Long;

    Argument = CurrentOption->Argument == COMMAND_ARGS_PARSER_HAS_ARGUMENT ? "(HAS ARGUMENT)" :
               CurrentOption->Argument == COMMAND_ARGS_PARSER_NO_ARGUMENT ? "(NO ARGUMENT)"   :
               CurrentOption->Argument == COMMAND_ARGS_PARSER_MAP_ERROR ? "(ERROR)" :
               CurrentOption->Argument == COMMAND_ARGS_PARSER_MAP_EXISTS ? "(EXISTS)" :
               CurrentOption->Argument;

    fprintf( stderr, "Option: %s %s %s\n", Short, Long, Argument );

    CurrentOption = CurrentOption->Next;
  }

  CurrentFile = Map->FileList;
  while ( CurrentFile != NULL )
  {
    fprintf( stderr, "File: %s\n", CurrentFile->Value );
    CurrentFile = CurrentFile->Next;
  }
  
  fprintf( stderr, "Map->ErrorString: %s\n", Map->ErrorString == NULL ? "(NULL)" : Map->ErrorString );
}

/* --------------------------------------------------------- */

void CAPINT_appendOptionWithArgumentToMap( struct commandArgsParsedMap *Map, const struct option *ParserOption, const char *Argument )
{
  struct option *MapOption;

  if ( ParserOption == NULL )
    return;
  
  MapOption = CAPINT_createOption( ParserOption->Short, ParserOption->Long, Argument );
  if ( MapOption == NULL )
    return;

  MapOption->Next = Map->OptionsList;
  Map->OptionsList = MapOption;
}

/* --------------------------------------------------------- */

void CAPINT_appendOptionErrorShortToMap( struct commandArgsParsedMap *Map, char Short )
{
  struct option *MapOption;

  MapOption = CAPINT_createOption( Short, COMMAND_ARGS_PARSER_NO_LONG, COMMAND_ARGS_PARSER_MAP_ERROR );
  if ( MapOption == NULL )
    return;

  MapOption->Next = Map->OptionsList;
  Map->OptionsList = MapOption;
}

/* --------------------------------------------------------- */

void CAPINT_appendOptionErrorLongToMap( struct commandArgsParsedMap *Map, const char *Long )
{
  struct option *MapOption;

  MapOption = CAPINT_createOption( COMMAND_ARGS_PARSER_NO_SHORT, Long, COMMAND_ARGS_PARSER_MAP_ERROR );
  if ( MapOption == NULL )
    return;

  MapOption->Next = Map->OptionsList;
  Map->OptionsList = MapOption;
}

/* --------------------------------------------------------- */

void CAPINT_appendFileItemToMap( struct commandArgsParsedMap *Map, const char *FileName )
{
  struct fileitem *File;

  if ( Map == NULL )
    return;

  if ( FileName == NULL )
    return;

  File = CAPINT_createFile( FileName );
  if ( File == NULL )
    return;

  File->Next = Map->FileList;
  Map->FileList = File;
}

/* --------------------------------------------------------- */

void CAPINT_updateMapFileVectorFromList( struct commandArgsParsedMap *Map )
{
  size_t i;
  size_t CountOfFiles;
  struct fileitem *Current = NULL;

  if ( Map == NULL )
    return;

  CountOfFiles = 0;
  Current = Map->FileList;
  while ( Current != NULL )
  {
    CountOfFiles += 1;
    Current = Current->Next;
  }

  free( Map->FileListVector );
  Map->FileListVector = NULL;
  if ( CountOfFiles == 0 )
    return;

  Map->FileListVector = malloc( (CountOfFiles+1) * sizeof(const char*) );
  if ( Map->FileListVector == NULL )
    return;
  
  Map->FileListVector[CountOfFiles] = NULL;

  i = CountOfFiles - 1;
  Current = Map->FileList;
  while ( Current != NULL )
  {
    Map->FileListVector[i] = Current->Value;
    Current = Current->Next;
    i--;
  }
}

/* --------------------------------------------------------- */

void CAPINT_updateMapErrorString( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser )
{
  struct option *Option;
  char *BufferEnd = NULL;
  char ShortOptionString[2] = { 0 };
  const size_t ErrorStringSize = 512;

  if ( Map == NULL )
    return;

  free( Map->ErrorString );
  Map->ErrorString = NULL;

  Option = Map->OptionsList;
  while ( Option != NULL )
  {
    if ( Option->Argument == COMMAND_ARGS_PARSER_MAP_ERROR )
      break;
    Option = Option->Next;
  }

  if ( Option == NULL )
    return;
    
  Map->ErrorString = malloc( ErrorStringSize );
  if ( Map->ErrorString == NULL )
    return;

  if ( commandArgsParserIsLongOptionExists(Parser,Option->Long) )
  {
    BufferEnd = Map->ErrorString;
    BufferEnd = CAPINT_stpncpy( BufferEnd, "Argument for option '", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    BufferEnd = CAPINT_stpncpy( BufferEnd, Option->Long, ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    BufferEnd = CAPINT_stpncpy( BufferEnd, "' not found", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    return;
  }
  
  if ( commandArgsParserIsShortOptionExists(Parser,Option->Short) )
  {
    ShortOptionString[0] = Option->Short;

    BufferEnd = Map->ErrorString;
    BufferEnd = CAPINT_stpncpy( BufferEnd, "Argument for option '", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    BufferEnd = CAPINT_stpncpy( BufferEnd, ShortOptionString, ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    BufferEnd = CAPINT_stpncpy( BufferEnd, "' not found", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    return;
  }

  {
    ShortOptionString[0] = Option->Short;

    BufferEnd = Map->ErrorString;
    BufferEnd = CAPINT_stpncpy( BufferEnd, "Unexpected argument '", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    if ( Option->Long != COMMAND_ARGS_PARSER_NO_LONG )
      BufferEnd = CAPINT_stpncpy( BufferEnd, Option->Long, ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    else if ( Option->Short != COMMAND_ARGS_PARSER_NO_SHORT ) 
      BufferEnd = CAPINT_stpncpy( BufferEnd, ShortOptionString, ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    else
      BufferEnd = CAPINT_stpncpy( BufferEnd, "???", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    BufferEnd = CAPINT_stpncpy( BufferEnd, "' found", ErrorStringSize - ( BufferEnd - Map->ErrorString ) );
    return;

  }

}

/* ========================================================= */

