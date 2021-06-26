
/* ========================================================= */

#include "commandArgsParser/parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* ========================================================= */

const char* const COMMAND_ARGS_PARSER_MAP_EXISTS = "(COMMAND_ARGS_PARSER_MAP_EXISTS)";
const char* const COMMAND_ARGS_PARSER_MAP_ERROR  = "(COMMAND_ARGS_PARSER_MAP_ERROR)";

#define COMMAND_ARGS_PARSER_HAS_ARGUMENT ( (void*)(-1) )
#define COMMAND_ARGS_PARSER_NO_ARGUMENT  ( (void*)(0) )

/* --------------------------------------------------------- */

struct option
{
  char Short;
  char *Long;
  char *Argument;
  struct option *Next;
};

/* --------------------------------------------------------- */

struct fileitem
{
  char *Value;
  struct fileitem *Next;
};

/* --------------------------------------------------------- */

struct commandArgsParser
{
  struct option *OptionsList;
};

/* --------------------------------------------------------- */

struct commandArgsParsedMap
{
  char *ProgramName;
  struct option *OptionsList;
  struct fileitem *FileList;
  char **FileListVector;
};

/* --------------------------------------------------------- */

enum optionType 
{
  OptionTypeNull,
  OptionTypeShort,
  OptionTypeShortList,
  OptionTypeLong,
  OptionTypeArg,
  OptionTypeEnd
};

/* ========================================================= */

static void debugPrintMap( const struct commandArgsParsedMap *Map )
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

}

/* --------------------------------------------------------- */

static char* strcopy( const char *String )
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

static struct option* freeOption( struct option *Option )
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

static struct option* createOption( char Short, const char *Long, const char *Argument )
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
    Option->Argument = strcopy( Argument );
    if ( Option->Argument == NULL )
    {
      freeOption( Option );
      return NULL;
    }
  }

  if ( Long == COMMAND_ARGS_PARSER_NO_LONG )
  {
    Option->Long = COMMAND_ARGS_PARSER_NO_LONG;
  } else {
    Option->Long = strcopy( Long );
    if ( Option->Long == NULL )
    {
      freeOption( Option );
      return NULL;
    }
  }

  return Option;
}

/* --------------------------------------------------------- */

static struct fileitem* freeFile( struct fileitem *File )
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

static enum optionType optionTypeOfString( const char *String )
{
  if ( String == NULL )
    return OptionTypeNull;

  if ( String[0] == '\0' )
    return OptionTypeNull;

  if ( String[0] != '-' )
    return OptionTypeArg;

  if ( String[1] == '-' && String[2] == '\0' )
    return OptionTypeEnd;
  
  if ( String[1] == '\0' )
    return OptionTypeArg;

  if ( String[1] == '-' )
    return OptionTypeLong;

  if ( String[2] == '\0' )
    return OptionTypeShort;

  return OptionTypeShortList;
}

/* --------------------------------------------------------- */

static const struct option* findShortOptionInList( const struct option *Current, char Short )
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

static const struct option* findLongOptionInList( const struct option *Current, const char *Long )
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

/* --------------------------------------------------------- */

static void appendNextOptionToMap( struct commandArgsParsedMap *Map, const struct option *ParserOption, const char *Argument )
{
  struct option *MapOption;

  if ( ParserOption == NULL )
    return;
  
  MapOption = createOption( ParserOption->Short, ParserOption->Long, Argument );
  if ( MapOption == NULL )
    return;

  MapOption->Next = Map->OptionsList;
  Map->OptionsList = MapOption;
}

/* --------------------------------------------------------- */

static void appendErrorShortOptionToMap( struct commandArgsParsedMap *Map, char Short )
{
  struct option *MapOption;

  MapOption = createOption( Short, COMMAND_ARGS_PARSER_NO_LONG, COMMAND_ARGS_PARSER_MAP_ERROR );
  if ( MapOption == NULL )
    return;

  MapOption->Next = Map->OptionsList;
  Map->OptionsList = MapOption;
}

/* --------------------------------------------------------- */

static void appendErrorLongOptionToMap( struct commandArgsParsedMap *Map, const char *Long )
{
  struct option *MapOption;

  MapOption = createOption( COMMAND_ARGS_PARSER_NO_SHORT, Long, COMMAND_ARGS_PARSER_MAP_ERROR );
  if ( MapOption == NULL )
    return;

  MapOption->Next = Map->OptionsList;
  Map->OptionsList = MapOption;
}

/* --------------------------------------------------------- */

static int parseNextArgumentOptionTypeShortList( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv )
{
  const struct option *Option = NULL;

  const char *Current = argv[0] + 1;
  const char *Argument = NULL;

  size_t i;

  for ( i = 0; ; i++ )
  {
    if ( Current[i] == '\0' )
      return 1;

    Option = findShortOptionInList( Parser->OptionsList, Current[i] );
    if ( Option == NULL )
    {
      appendErrorShortOptionToMap( Map, Current[i] );
      continue;
    }

    if ( Option->Argument == COMMAND_ARGS_PARSER_HAS_ARGUMENT )
    {
      if ( Current[i+1] != '\0' )
      {
        appendErrorShortOptionToMap( Map, Current[i] );
        continue;
      }

      Argument = argv[1];
      if ( Argument == NULL )
      {
        appendErrorShortOptionToMap( Map, Current[i] );
        return 1;
      }
    
      appendNextOptionToMap( Map, Option, Argument );
      return 2;
    }
  
    appendNextOptionToMap( Map, Option, COMMAND_ARGS_PARSER_MAP_EXISTS );
  }

}

/* --------------------------------------------------------- */

static int parseNextArgumentOptionTypeLong( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv )
{
  const struct option *Option = NULL;

  const char *Current = argv[0] + 2;
  const char *Argument = NULL;

  Option = findLongOptionInList( Parser->OptionsList, Current );
  if ( Option == NULL )
  {
    appendErrorLongOptionToMap( Map, Current );
    return 1;
  }

  if ( Option->Argument == COMMAND_ARGS_PARSER_HAS_ARGUMENT )
  {
    Argument = argv[1];
    if ( Argument == NULL )
    {
      appendErrorLongOptionToMap( Map, Current );
      return 1;
    }
  
    appendNextOptionToMap( Map, Option, Argument );
    return 2;
  }

  appendNextOptionToMap( Map, Option, COMMAND_ARGS_PARSER_MAP_EXISTS );
  return 1;
}

/* --------------------------------------------------------- */

static int parseNextArgumentOptionTypeArg( struct commandArgsParsedMap *Map, char **argv )
{
  const char *Current = argv[0];
  struct fileitem *File = NULL;

  if ( Map == NULL )
    return 0;

  File = malloc( sizeof(*File) );
  if ( File == NULL )
    return 0;

  File->Value = strcopy( Current );
  if ( File->Value == NULL )
  {
    free(File);
    return 0;
  }

  File->Next = Map->FileList;
  Map->FileList = File;
  return 1;
}


/* --------------------------------------------------------- */

static int parseNextArgumentOptionTypeEnd( struct commandArgsParsedMap *Map, char **argv )
{
  size_t i;
  size_t TotalShift;

  if ( Map == NULL )
    return 0;

  assert( strcmp(argv[0],"--") == 0 );

  TotalShift = 1;
  for ( i = 1; argv[i] != NULL; i++ )
  {
    parseNextArgumentOptionTypeArg( Map, argv + i );
    TotalShift += 1;
  }

  return TotalShift;
}

/* --------------------------------------------------------- */

static int parseNextArgument( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv )
{
  enum optionType Type;

  if ( Map == NULL || argv == NULL )
    return 0;

  Type = optionTypeOfString( argv[0] );
  switch ( Type )
  {
    case OptionTypeNull:
      return 0;

    case OptionTypeShort:
    case OptionTypeShortList:
      return parseNextArgumentOptionTypeShortList( Map, Parser, argv );

    case OptionTypeLong:
      return parseNextArgumentOptionTypeLong( Map, Parser, argv );

    case OptionTypeArg:
      return parseNextArgumentOptionTypeArg( Map, argv );

    case OptionTypeEnd:
      return parseNextArgumentOptionTypeEnd( Map, argv );

    default:
      return -1;
  }
}

/* --------------------------------------------------------- */

static void updateMapFileVector( struct commandArgsParsedMap *Map )
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
  if ( CountOfFiles == 0 )
  {
    Map->FileListVector = NULL;
    return;
  }

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
    Current = freeOption(Current);

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

  Option = createOption( Short, Long, HasArgument ? COMMAND_ARGS_PARSER_HAS_ARGUMENT : COMMAND_ARGS_PARSER_NO_ARGUMENT );
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

  Option = findShortOptionInList( Parser->OptionsList, Short );

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

  Option = findLongOptionInList( Parser->OptionsList, Long );

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

  Map->ProgramName = strcopy( argv[0] );
  if ( Map->ProgramName == NULL )
    return Map;

  ++argv;
  while ( 1 )
  {
    Shift = parseNextArgument( Map, Parser, argv );
    if ( Shift <= 0 )
      break;
    argv += Shift;
  }

  updateMapFileVector(Map);

  if ( 0 )
    debugPrintMap(Map);

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
    CurrentOption = freeOption(CurrentOption);

  CurrentFile = Map->FileList;
  while ( CurrentFile != NULL )
    CurrentFile = freeFile(CurrentFile);
  
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

  Option = findShortOptionInList( Map->OptionsList, Short );
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

  Option = findLongOptionInList( Map->OptionsList, Long );
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

