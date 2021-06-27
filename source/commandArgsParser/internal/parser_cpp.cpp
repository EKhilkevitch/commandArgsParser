
// =========================================================

#include "commandArgsParser/internal/parser_cpp.h"
#include "commandArgsParser/parser.h"

// =========================================================

commandArgumentsParser::exception::exception( const std::string &W ) :
  What(W)
{
}

// ---------------------------------------------------------
      
commandArgumentsParser::exception::~exception() throw()
{
}

// ---------------------------------------------------------

const char* commandArgumentsParser::exception::what() const throw()
{
  return What.c_str();
}

// =========================================================

struct commandArgumentsParser::map::impl
{
  public:
    struct commandArgsParsedMap *Map;

    explicit impl( struct commandArgsParsedMap *M );
    ~impl();

  private:
    impl( const impl& );
    impl& operator=( const impl& );
};

// ---------------------------------------------------------

commandArgumentsParser::map::impl::impl( struct commandArgsParsedMap *M ) :
  Map(M)
{
}

// ---------------------------------------------------------

commandArgumentsParser::map::impl::~impl()
{
  commandArgsParsedMapDelete(Map);
}

// ---------------------------------------------------------

commandArgumentsParser::map::map() :
  Impl( new impl(NULL) )
{
}

// ---------------------------------------------------------
      
commandArgumentsParser::map::map( const map &Map ) :
  Impl( new impl( commandArgsParsedMapCopy(Map.Impl->Map) ) )
{
  if ( Impl->Map == NULL )
    throw exception( "Can not copy map" );
}

// ---------------------------------------------------------
 
commandArgumentsParser::map& commandArgumentsParser::map::operator=( const map &Map )
{
  map Copy(Map);
  swap( Copy );
  return *this;
}

// ---------------------------------------------------------

commandArgumentsParser::map::~map()
{
  delete Impl;
}

// ---------------------------------------------------------

void commandArgumentsParser::map::swap( map &Other )
{
  impl *Tmp = Impl;
  Impl = Other.Impl;
  Other.Impl = Tmp;
}

// ---------------------------------------------------------

bool commandArgumentsParser::map::isSuccess() const
{
  const int IsSuccess = commandArgsParsedMapIsSuccess(Impl->Map);
  if ( IsSuccess != 0 )
    return true;
  return false;
}

// ---------------------------------------------------------
      
std::string commandArgumentsParser::map::failMessage() const
{
  const int IsSuccess = commandArgsParsedMapIsSuccess(Impl->Map);
  if ( IsSuccess != 0 )
    return std::string();

  const char *ErrorString = commandArgsParsedMapErrorString(Impl->Map);
  if ( ErrorString == NULL )
    return "Unknown error";

  return ErrorString;
}

// ---------------------------------------------------------
      
std::string commandArgumentsParser::map::value( char Short, const std::string &Default ) const
{
  const char *Value = commandArgsParsedMapShortOptionValue( Impl->Map, Short );

  if ( Value == NULL )
    return Default;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return Default;

  return Value;
}

// ---------------------------------------------------------

std::string commandArgumentsParser::map::value( const std::string &Long, const std::string &Default ) const
{
  const char *Value = commandArgsParsedMapLongOptionValue( Impl->Map, Long.c_str() );

  if ( Value == NULL )
    return Default;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return Default;
  
  return Value;
}

// ---------------------------------------------------------

bool commandArgumentsParser::map::exists( char Short ) const
{
  const char *Value = commandArgsParsedMapShortOptionValue( Impl->Map, Short );

  if ( Value == NULL )
    return false;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return true;

  return true;
}

// ---------------------------------------------------------

bool commandArgumentsParser::map::exists( const std::string &Long ) const
{
  const char *Value = commandArgsParsedMapLongOptionValue( Impl->Map, Long.c_str() );
  
  if ( Value == NULL )
    return false;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return true;

  return true;
}

// ---------------------------------------------------------
      
std::vector<std::string> commandArgumentsParser::map::files() const
{
  char **Files = commandArgsParsedMapFiles( Impl->Map );
  
  std::vector<std::string> Result;
  if ( Files == NULL )
    return Result;

  for ( size_t i = 0; Files[i] != NULL; i++ )
    Result.push_back( Files[i] );

  return Result;
}

// ---------------------------------------------------------
      
std::string commandArgumentsParser::map::program() const
{
  const char *Program = commandArgsParsedMapProgramName( Impl->Map );
  if ( Program == NULL )
    return std::string();
  return Program;
}

// =========================================================

struct commandArgumentsParser::parser::impl
{
  public:
    commandArgsParser *Parser;

    explicit impl( commandArgsParser *Parser );
    ~impl();

  private:
    impl( const impl& );
    impl& operator=( const impl& );
};

// ---------------------------------------------------------

commandArgumentsParser::parser::impl::impl( commandArgsParser *P ) :
  Parser( P )
{
}

// ---------------------------------------------------------

commandArgumentsParser::parser::impl::~impl()
{
  commandArgsParserDelete(Parser);
}

// ---------------------------------------------------------

commandArgumentsParser::parser::parser() :
  Impl( new impl( commandArgsParserCreate() ) )
{
  if ( Impl->Parser == NULL )
    throw exception( "Can not allocate parser" );
}

// ---------------------------------------------------------
      
commandArgumentsParser::parser::parser( const parser &Parser ) :
  Impl( new impl( commandArgsParserCopy(Parser.Impl->Parser) ) )
{
  if ( Impl->Parser == NULL )
    throw exception( "Can not copy parser" );
}

// ---------------------------------------------------------

commandArgumentsParser::parser& commandArgumentsParser::parser::operator=( const parser &Parser )
{
  parser Copy(Parser);
  swap(Copy);
  return *this;
}

// ---------------------------------------------------------

commandArgumentsParser::parser::~parser()
{
  delete Impl;
}

// ---------------------------------------------------------

void commandArgumentsParser::parser::swap( parser &Other )
{
  impl *Tmp = Impl;
  Impl = Other.Impl;
  Other.Impl = Tmp;
}

// ---------------------------------------------------------

void commandArgumentsParser::parser::addOption( char Short, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, Short, COMMAND_ARGS_PARSER_NO_LONG, Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------
      
void commandArgumentsParser::parser::addOption( const std::string &Long, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, COMMAND_ARGS_PARSER_NO_SHORT, Long.c_str(), Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------

void commandArgumentsParser::parser::addOption( char Short, const std::string &Long, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, Short, Long.c_str(), Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------

commandArgumentsParser::map commandArgumentsParser::parser::parse( char **argv )
{
  commandArgsParsedMap *Map = commandArgsParserParse( Impl->Parser, argv );

  if ( Map == NULL )
    throw exception( "Can not create parsing result" );

  map Result;
  Result.Impl->Map = Map;
  return Result;
}

// =========================================================

