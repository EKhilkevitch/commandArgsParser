
/* ========================================================= */

#include "commandArgsParser/internal/optiontype.h"

#include <stddef.h>

/* ========================================================= */

enum optionType CAPINT_optionTypeOfString( const char *String )
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

/* ========================================================= */

