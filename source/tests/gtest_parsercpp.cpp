
// =========================================================

#include <gtest/gtest.h>

#include "commandArgsParser/parser.h"

#include <memory>
#include <cstdarg>
#include <cstring>

// =========================================================

namespace
{
  char* strdup( const char *Str )
  {
    if ( Str == nullptr )
      return nullptr;

    size_t Length = std::strlen(Str);
    char *Copy = new char[ Length + 1 ];
    std::strcpy( Copy, Str );
    return Copy;
  }


  char** createArgv( const char *Str, ... )
  {
    const size_t MaxArgvSize = 128;

    va_list Args;
    va_start( Args, Str );

    char **Result = new char*[MaxArgvSize];
    Result[0] = strdup( Str );
    size_t i = 1;
    while ( Str != nullptr )
    {
      Str = va_arg( Args, const char* );
      Result[i] = strdup(Str);
      if ( Result[i] == nullptr )
        break;
      i++;
    }

    va_end( Args );
    return Result;
  }

  void freeArgv( char **Argv )
  {
    for ( size_t i = 0; Argv[i] != nullptr; i++ )
      delete [] Argv[i];
    delete [] Argv;
  }
}

// =========================================================

TEST( parserCpp, parse )
{
  commandArgumentsParser::parser Parser;
  Parser.addOption( 'a', "optA", commandArgumentsParser::parser::HasArgument );
  Parser.addOption( 'b', commandArgumentsParser::parser::NoArgument );
  Parser.addOption( "optC", commandArgumentsParser::parser::NoArgument );
  Parser.addOption( 'd', "optD", commandArgumentsParser::parser::HasArgument );

  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "file1", "-a", "argA", "-b", "--optC", "file2", "file3", nullptr ), &freeArgv );
  commandArgumentsParser::map Map = Parser.parse(Argv.get());

  EXPECT_EQ( "program", Map.program() );

  EXPECT_TRUE( Map.exists('a') );
  EXPECT_TRUE( Map.exists("optA") );
  EXPECT_EQ( "argA", Map.value('a') );
  EXPECT_EQ( "argA", Map.value("optA") );
  EXPECT_EQ( "argA", Map.value("optA","Default") );

  EXPECT_TRUE( Map.exists('b') );
  EXPECT_EQ( "", Map.value('b') );
  EXPECT_EQ( "Default", Map.value('b',"Default") );
  
  EXPECT_TRUE( Map.exists("optC") );
  EXPECT_EQ( "Default", Map.value("optC","Default") );
  
  EXPECT_FALSE( Map.exists('d') );
  EXPECT_FALSE( Map.exists("optD") );
  EXPECT_EQ( "Default", Map.value("optD","Default") );
  
  EXPECT_FALSE( Map.exists('e') );
  EXPECT_FALSE( Map.exists("optE") );

  const auto Files = Map.files();
  ASSERT_EQ( 3, Files.size() );
  EXPECT_EQ( "file1", Files.at(0) );
  EXPECT_EQ( "file2", Files.at(1) );
  EXPECT_EQ( "file3", Files.at(2) );
}

// =========================================================


