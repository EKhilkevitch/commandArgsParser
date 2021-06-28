
// =========================================================

#include "commandArgsParser/internal/parser_cpp.h"
#include "commandArgsParser/parser.h"

#include <cstring>

// =========================================================

commandArguments::exception::exception( const std::string &W ) :
  What(W)
{
}

// ---------------------------------------------------------
      
commandArguments::exception::~exception() throw()
{
}

// ---------------------------------------------------------

const char* commandArguments::exception::what() const throw()
{
  return What.c_str();
}

// =========================================================

struct commandArguments::map::impl
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

commandArguments::map::impl::impl( struct commandArgsParsedMap *M ) :
  Map(M)
{
}

// ---------------------------------------------------------

commandArguments::map::impl::~impl()
{
  commandArgsParsedMapDelete(Map);
}

// ---------------------------------------------------------

commandArguments::map::map() :
  Impl( new impl(NULL) )
{
}

// ---------------------------------------------------------
      
commandArguments::map::map( const map &Map ) :
  Impl( new impl( commandArgsParsedMapCopy(Map.Impl->Map) ) )
{
  if ( Impl->Map == NULL )
    throw exception( "Can not copy map" );
}

// ---------------------------------------------------------
 
commandArguments::map& commandArguments::map::operator=( const map &Map )
{
  map Copy(Map);
  swap( Copy );
  return *this;
}

// ---------------------------------------------------------

commandArguments::map::~map()
{
  delete Impl;
}

// ---------------------------------------------------------

void commandArguments::map::swap( map &Other )
{
  impl *Tmp = Impl;
  Impl = Other.Impl;
  Other.Impl = Tmp;
}

// ---------------------------------------------------------

bool commandArguments::map::isSuccess() const
{
  const int IsSuccess = commandArgsParsedMapIsSuccess(Impl->Map);
  if ( IsSuccess != 0 )
    return true;
  return false;
}

// ---------------------------------------------------------
      
std::string commandArguments::map::failMessage() const
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
      
std::string commandArguments::map::value( char Short, const std::string &Default ) const
{
  const char *Value = commandArgsParsedMapShortOptionValue( Impl->Map, Short );

  if ( Value == NULL )
    return Default;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return Default;

  return Value;
}

// ---------------------------------------------------------

std::string commandArguments::map::value( const std::string &Long, const std::string &Default ) const
{
  const char *Value = commandArgsParsedMapLongOptionValue( Impl->Map, Long.c_str() );

  if ( Value == NULL )
    return Default;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return Default;
  
  return Value;
}

// ---------------------------------------------------------

std::string commandArguments::map::value( const char *Long, const std::string &Default ) const
{
  const char *Value = commandArgsParsedMapLongOptionValue( Impl->Map, Long );

  if ( Value == NULL )
    return Default;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return Default;
  
  return Value;
}

// ---------------------------------------------------------

bool commandArguments::map::exists( char Short ) const
{
  const char *Value = commandArgsParsedMapShortOptionValue( Impl->Map, Short );

  if ( Value == NULL )
    return false;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return true;

  return true;
}

// ---------------------------------------------------------

bool commandArguments::map::exists( const std::string &Long ) const
{
  const char *Value = commandArgsParsedMapLongOptionValue( Impl->Map, Long.c_str() );
  
  if ( Value == NULL )
    return false;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return true;

  return true;
}


// ---------------------------------------------------------

bool commandArguments::map::exists( const char *Long ) const
{
  const char *Value = commandArgsParsedMapLongOptionValue( Impl->Map, Long );
  
  if ( Value == NULL )
    return false;

  if ( Value == COMMAND_ARGS_PARSER_MAP_EXISTS )
    return true;

  return true;
}

// ---------------------------------------------------------
      
std::vector<std::string> commandArguments::map::files() const
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
      
std::string commandArguments::map::program() const
{
  const char *Program = commandArgsParsedMapProgramName( Impl->Map );
  if ( Program == NULL )
    return std::string();
  return Program;
}

// =========================================================

struct commandArguments::parser::impl
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

commandArguments::parser::impl::impl( commandArgsParser *P ) :
  Parser( P )
{
}

// ---------------------------------------------------------

commandArguments::parser::impl::~impl()
{
  commandArgsParserDelete(Parser);
}

// ---------------------------------------------------------

commandArguments::parser::parser() :
  Impl( new impl( commandArgsParserCreate() ) )
{
  if ( Impl->Parser == NULL )
    throw exception( "Can not allocate parser" );
}

// ---------------------------------------------------------
      
commandArguments::parser::parser( const parser &Parser ) :
  Impl( new impl( commandArgsParserCopy(Parser.Impl->Parser) ) )
{
  if ( Impl->Parser == NULL )
    throw exception( "Can not copy parser" );
}

// ---------------------------------------------------------

commandArguments::parser& commandArguments::parser::operator=( const parser &Parser )
{
  parser Copy(Parser);
  swap(Copy);
  return *this;
}

// ---------------------------------------------------------

commandArguments::parser::~parser()
{
  delete Impl;
}

// ---------------------------------------------------------

void commandArguments::parser::swap( parser &Other )
{
  impl *Tmp = Impl;
  Impl = Other.Impl;
  Other.Impl = Tmp;
}

// ---------------------------------------------------------

void commandArguments::parser::addOption( char Short, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, Short, COMMAND_ARGS_PARSER_NO_LONG, Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------
      
void commandArguments::parser::addOption( const std::string &Long, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, COMMAND_ARGS_PARSER_NO_SHORT, Long.c_str(), Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------
      
void commandArguments::parser::addOption( const char *Long, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, COMMAND_ARGS_PARSER_NO_SHORT, Long, Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------

void commandArguments::parser::addOption( char Short, const std::string &Long, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, Short, Long.c_str(), Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------

void commandArguments::parser::addOption( char Short, const char *Long, argument Arguement )
{
  commandArgsParserAddOption( Impl->Parser, Short, Long, Arguement == HasArgument ? 1 : 0 );
}

// ---------------------------------------------------------

commandArguments::map commandArguments::parser::parse( char **argv )
{
  commandArgsParsedMap *Map = commandArgsParserParse( Impl->Parser, argv );

  if ( Map == NULL )
    throw exception( "Can not create parsing result" );

  map Result;
  Result.Impl->Map = Map;
  return Result;
}

// ---------------------------------------------------------

commandArguments::map commandArguments::parser::parse( const std::vector<std::string> &Args )
{
  char **argv = NULL;
  map Map;

  try
  {
    argv = createArgvFromVector(Args);
    Map = parse(argv);
  } catch ( ... ) {
    deleteArgvFromVector(argv);
    throw;
  }
    
  deleteArgvFromVector(argv);
  return Map;
}

// ---------------------------------------------------------

commandArguments::map commandArguments::parser::operator()( char **argv )
{
  return parse(argv);
}


// ---------------------------------------------------------

commandArguments::map commandArguments::parser::operator()( const std::vector<std::string> &Args )
{
  return parse(Args);
}

// ---------------------------------------------------------
      
char** commandArguments::parser::createArgvFromVector( const std::vector<std::string> &Args )
{
  char **argv = NULL;

  try
  {
    argv = new char*[ Args.size() + 1 ];

    for ( size_t i = 0; i < Args.size()+1; i++ )
      argv[i] = NULL;

    for ( size_t i = 0; i < Args.size(); i++ )
    {
      argv[i] = new char[ Args[i].length() + 1 ];
      std::strcpy( argv[i], Args[i].c_str() );
    }

    return argv;

  } catch ( ... ) {
    deleteArgvFromVector(argv);
    throw;
  }
}

// ---------------------------------------------------------

void commandArguments::parser::deleteArgvFromVector( char **argv )
{
  if ( argv == NULL )
    return;

  for ( size_t i = 0; argv[i] != NULL; i++ )
    delete [] argv[i];
  delete [] argv;
}

// =========================================================

