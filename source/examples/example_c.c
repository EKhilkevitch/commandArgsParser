
#include "commandArgsParser/parser.h"

#include <stdio.h>

int main( int argc, char **argv )
{
  struct commandArgsParser *Parser = NULL;
  struct commandArgsParsedMap *Map = NULL;
  char **Files;
  size_t i;

  Parser = commandArgsParserCreate();
  if ( Parser == NULL )
  {
    fprintf( stderr, "Can not create parser\n" );
    return 1;
  }

  commandArgsParserAddOption( Parser, 'h', "help", 0 );
  commandArgsParserAddOption( Parser, 'a', "all", 0 );
  commandArgsParserAddOption( Parser, 'r', "regex", 1 );
  commandArgsParserAddOption( Parser, 't', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser, COMMAND_ARGS_PARSER_NO_SHORT, "exclude", 1 );
  commandArgsParserAddOption( Parser, COMMAND_ARGS_PARSER_NO_SHORT, "include", 1 );

  Map = commandArgsParserParse(Parser,argv);
  if ( Map == NULL )
  {
    fprintf( stderr, "Can not create map\n" );
    commandArgsParserDelete(Parser);
    return 1;
  }

  if ( ! commandArgsParsedMapIsSuccess(Map) )
  {
    fprintf( stderr, "Error while parsing arguments: %s\n", commandArgsParsedMapErrorString(Map) );
    commandArgsParsedMapDelete(Map);
    commandArgsParserDelete(Parser);
    return 2;
  }

  if ( commandArgsParsedMapShortOptionValue(Map,'h') )
  {
    fprintf( stderr, "This program justdemonstrate 'commandArgsParser' library usage and nothing mode. Please see source code.\n"
                     "Usage: %s [-h|--help]\n"
                     "          [-a|--all] [-r|--regex REGEX] [-t] [--exclude NAME] [--include NAME] FILES...\n",
                             commandArgsParsedMapProgramName(Map) );
    commandArgsParsedMapDelete(Map);
    commandArgsParserDelete(Parser);
    return 0;
  }

  printf( "program name: '%s'\n", commandArgsParsedMapProgramName(Map) );
  printf( "options:\n" );
  printf( "  [-a|--all] is (%s) \n", commandArgsParsedMapShortOptionValue(Map,'h') ? "on" : "off" );
  printf( "  [-r|--regex] value is (%s) \n", commandArgsParsedMapShortOptionValue(Map,'r') != NULL ? commandArgsParsedMapShortOptionValue(Map,'r') : "off" );
  printf( "  [-t] is (%s) \n", commandArgsParsedMapShortOptionValue(Map,'t') ? "on" : "off" );
  printf( "  [-exclude] is (%s) \n", commandArgsParsedMapLongOptionValue(Map,"exclude") != NULL ? commandArgsParsedMapLongOptionValue(Map,"exclude") : "off" );
  printf( "  [-include] is (%s) \n", commandArgsParsedMapLongOptionValue(Map,"include") != NULL ? commandArgsParsedMapLongOptionValue(Map,"include") : "off" );

  Files = commandArgsParsedMapFiles(Map);
  if ( Files != NULL )
  {
    printf( "files: \n" );
    for ( i = 0; Files[i] != NULL; i++ )
      printf( "  %s\n", Files[i] );
  }

  commandArgsParsedMapDelete(Map);
  commandArgsParserDelete(Parser);

  return 0;

  (void)argc; /* just for remove warning */
}

