
#ifndef _MAP_H_
#define _MAP_H_

/* ========================================================= */

struct option;
struct fileitem;
struct commandArgsParser;

struct commandArgsParsedMap
{
  char *ProgramName;
  struct option *OptionsList;
  struct fileitem *FileList;
  char **FileListVector;
  char *ErrorString;
};

/* ========================================================= */

struct commandArgsParsedMap* CAPINT_createMap( void );
void CAPINT_appendOptionWithArgumentToMap( struct commandArgsParsedMap *Map, const struct option *ParserOption, const char *Argument );
void CAPINT_appendOptionErrorShortToMap( struct commandArgsParsedMap *Map, char Short );
void CAPINT_appendOptionErrorLongToMap( struct commandArgsParsedMap *Map, const char *Long );
void CAPINT_appendFileItemToMap( struct commandArgsParsedMap *Map, const char *FileName );
void CAPINT_updateMapFileVectorFromList( struct commandArgsParsedMap *Map );
void CAPINT_updateMapErrorString( struct commandArgsParsedMap *Map, const struct commandArgsParser *Parser );

void CAPINT_debugPrintMap( const struct commandArgsParsedMap *Map );

/* ========================================================= */

#endif

