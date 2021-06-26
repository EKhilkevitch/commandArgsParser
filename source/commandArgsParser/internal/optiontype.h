
#ifndef _OPTIONTYPE_H_
#define _OPTIONTYPE_H_

/* ========================================================= */

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

enum optionType CAPINT_optionTypeOfString( const char *String );

/* ========================================================= */

#endif

