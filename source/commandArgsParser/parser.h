
#ifndef _CAP_PARSER_H_
#define _CAP_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================= */

struct commandArgsParser;
struct commandArgsParsedMap;

#define COMMAND_ARGS_PARSER_NO_SHORT '\0'
#define COMMAND_ARGS_PARSER_NO_LONG  NULL

extern const char* const COMMAND_ARGS_PARSER_MAP_EXISTS;
extern const char* const COMMAND_ARGS_PARSER_MAP_ERROR;

/* ========================================================= */

struct commandArgsParser* commandArgsParserCreate( void );
void commandArgsParserDelete( struct commandArgsParser *Parser );

void commandArgsParserAddOption( struct commandArgsParser *Parser, char Short, const char *Long, int HasArgument );
int commandArgsParserIsShortOptionExists( const struct commandArgsParser *Parser, char Short );
int commandArgsParserIsLongOptionExists( const struct commandArgsParser *Parser, const char *Long );

struct commandArgsParsedMap* commandArgsParserParse( const struct commandArgsParser *Parser, char **argv );
void commandArgsParsedMapDelete( struct commandArgsParsedMap *Map );
int commandArgsParsedMapIsSuccess( const struct commandArgsParsedMap *Map );
const char* commandArgsParsedMapProgramName( const struct commandArgsParsedMap *Map );
const char* commandArgsParsedMapShortOptionValue( const struct commandArgsParsedMap *Map, char Short );
const char* commandArgsParsedMapLongOptionValue( const struct commandArgsParsedMap *Map, const char *Long );
char** commandArgsParsedMapFiles( const struct commandArgsParsedMap *Map );

/* ========================================================= */

#ifdef __cplusplus
}
#endif

#endif

