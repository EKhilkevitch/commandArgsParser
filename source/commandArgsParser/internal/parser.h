
#ifndef _PARSER_H_
#define _PARSER_H_

/* ========================================================= */

struct option;

struct commandArgsParser
{
  struct option *OptionsList;
};

/* ========================================================= */

int CAPINT_parseNextArgumentOptionTypeShortList( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv );
int CAPINT_parseNextArgumentOptionTypeLong( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv );
int CAPINT_parseNextArgumentOptionTypeArg( struct commandArgsParsedMap *Map, char **argv );
int CAPINT_parseNextArgumentOptionTypeEnd( struct commandArgsParsedMap *Map, char **argv );
int CAPINT_parseNextArgument( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser, char **argv );

/* ========================================================= */

#endif

