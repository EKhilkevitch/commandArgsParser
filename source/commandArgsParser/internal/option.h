
#ifndef _OPTION_H_
#define _OPTION_H_

/* ========================================================= */

struct option
{
  char Short;
  char *Long;
  char *Argument;
  struct option *Next;
};

#define COMMAND_ARGS_PARSER_HAS_ARGUMENT ( (void*)(-1) )
#define COMMAND_ARGS_PARSER_NO_ARGUMENT  ( (void*)(0) )

struct option* CAPINT_freeOption( struct option *Option );
struct option* CAPINT_createOption( char Short, const char *Long, const char *Argument );

const struct option* CAPINT_findShortOptionInList( const struct option *Current, char Short );
const struct option* CAPINT_findLongOptionInList( const struct option *Current, const char *Long );

/* ========================================================= */

#endif

