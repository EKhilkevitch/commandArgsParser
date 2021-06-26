
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

TEST( parser, create )
{
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );
  ASSERT_NE( nullptr, Parser.get() );
}

// ---------------------------------------------------------

TEST( parser, add_oneItem )
{
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );
  ASSERT_NE( nullptr, Parser.get() );
  
  commandArgsParserAddOption( Parser.get(), 'a', "long", 1 );

  EXPECT_EQ( 1, commandArgsParserIsShortOptionExists( Parser.get(), 'a' ) );
  EXPECT_EQ( 0, commandArgsParserIsShortOptionExists( Parser.get(), 'b' ) );
  
  EXPECT_EQ( 1, commandArgsParserIsLongOptionExists( Parser.get(), "long" ) );
  EXPECT_EQ( 0, commandArgsParserIsLongOptionExists( Parser.get(), "xxxx" ) );
}

// ---------------------------------------------------------

TEST( parser, add_threeItems )
{
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );
  ASSERT_NE( nullptr, Parser.get() );
  
  commandArgsParserAddOption( Parser.get(), 'a', "long1", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "long2", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "long3", 0 );

  EXPECT_EQ( 1, commandArgsParserIsShortOptionExists( Parser.get(), 'a' ) );
  EXPECT_EQ( 1, commandArgsParserIsShortOptionExists( Parser.get(), 'b' ) );
  EXPECT_EQ( 1, commandArgsParserIsShortOptionExists( Parser.get(), 'c' ) );
  EXPECT_EQ( 0, commandArgsParserIsShortOptionExists( Parser.get(), 'd' ) );
  
  EXPECT_EQ( 1, commandArgsParserIsLongOptionExists( Parser.get(), "long1" ) );
  EXPECT_EQ( 1, commandArgsParserIsLongOptionExists( Parser.get(), "long2" ) );
  EXPECT_EQ( 1, commandArgsParserIsLongOptionExists( Parser.get(), "long3" ) );
  EXPECT_EQ( 0, commandArgsParserIsLongOptionExists( Parser.get(), "xxxx" ) );
}

// ---------------------------------------------------------

TEST( parser, add_threeItems_notFull )
{
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );
  ASSERT_NE( nullptr, Parser.get() );
  
  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "long2", 0 );
  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, COMMAND_ARGS_PARSER_NO_LONG, 0 );

  EXPECT_EQ( 1, commandArgsParserIsShortOptionExists( Parser.get(), 'a' ) );
  EXPECT_EQ( 0, commandArgsParserIsShortOptionExists( Parser.get(), 'b' ) );
  EXPECT_EQ( 0, commandArgsParserIsShortOptionExists( Parser.get(), 'c' ) );
  EXPECT_EQ( 0, commandArgsParserIsShortOptionExists( Parser.get(), 'd' ) );
  
  EXPECT_EQ( 0, commandArgsParserIsLongOptionExists( Parser.get(), "long1" ) );
  EXPECT_EQ( 1, commandArgsParserIsLongOptionExists( Parser.get(), "long2" ) );
  EXPECT_EQ( 0, commandArgsParserIsLongOptionExists( Parser.get(), "long3" ) );
  EXPECT_EQ( 0, commandArgsParserIsLongOptionExists( Parser.get(), "xxxx" ) );
}

// ---------------------------------------------------------

TEST( parser, parse_noargs )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_EQ( nullptr, commandArgsParsedMapFiles(Map.get()) );
}

// ---------------------------------------------------------

TEST( parser, parse_short_single )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-b", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_TRUE( nullptr == commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_TRUE( COMMAND_ARGS_PARSER_MAP_EXISTS == commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_TRUE( nullptr == commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_TRUE( nullptr == commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_multiple )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-b", "-a", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_multiple_args_1 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-b", "arg1", "-a", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( "arg1", commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_multiple_args_2 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-b", "argB", "-c", "-a", "argA", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( "argA", commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( "argB", commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_multiple_list )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-adb", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'd', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_multiple_list_arg )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-adb", "argB", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'd', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( "argB", commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_error_1 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-Z", "-a", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 0, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_TRUE( COMMAND_ARGS_PARSER_MAP_EXISTS == commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_TRUE( nullptr == commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_TRUE( nullptr == commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_TRUE( nullptr == commandArgsParsedMapShortOptionValue(Map.get(),'d') );
  EXPECT_TRUE( COMMAND_ARGS_PARSER_MAP_ERROR == commandArgsParsedMapShortOptionValue(Map.get(),'Z') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_error_2 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-a", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 1 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 0, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_ERROR, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_error_3 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-abd", "argB", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 1 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'd', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 0, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_ERROR, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_short_error_4 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "-aZbd", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'b', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'c', COMMAND_ARGS_PARSER_NO_LONG, 0 );
  commandArgsParserAddOption( Parser.get(), 'd', COMMAND_ARGS_PARSER_NO_LONG, 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 0, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_ERROR, commandArgsParsedMapShortOptionValue(Map.get(),'Z') );
}

// ---------------------------------------------------------

TEST( parser, parse_long_single )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "--optB", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 0 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_long_multiple_args )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "--optB", "--optA", "argA", "--optC", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), 'd', "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( "argA", commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );
}

// ---------------------------------------------------------

TEST( parser, parse_long_multiple_shortLong_args )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "--optA", "argA", "-bc", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( "argA", commandArgsParsedMapLongOptionValue(Map.get(),"optA") );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapLongOptionValue(Map.get(),"optB") );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapLongOptionValue(Map.get(),"optC") );
  EXPECT_STREQ( nullptr, commandArgsParsedMapLongOptionValue(Map.get(),"optD") );
  EXPECT_EQ( nullptr, commandArgsParsedMapFiles(Map.get()) );
}

// ---------------------------------------------------------

TEST( parser, parse_long_error_1 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "--optZ", "--optB", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 0, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( nullptr, commandArgsParsedMapLongOptionValue(Map.get(),"optA") );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapLongOptionValue(Map.get(),"optB") );
  EXPECT_STREQ( nullptr, commandArgsParsedMapLongOptionValue(Map.get(),"optC") );
  EXPECT_STREQ( nullptr, commandArgsParsedMapLongOptionValue(Map.get(),"optD") );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_ERROR, commandArgsParsedMapLongOptionValue(Map.get(),"optZ") );
}

// ---------------------------------------------------------

TEST( parser, parse_long_error_2 )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "--optB", "--optA", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), COMMAND_ARGS_PARSER_NO_SHORT, "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 0, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_ERROR, commandArgsParsedMapLongOptionValue(Map.get(),"optA") );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapLongOptionValue(Map.get(),"optB") );
  EXPECT_STREQ( nullptr, commandArgsParsedMapLongOptionValue(Map.get(),"optC") );
  EXPECT_STREQ( nullptr, commandArgsParsedMapLongOptionValue(Map.get(),"optD") );
}

// ---------------------------------------------------------

TEST( parser, parse_files )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "file1", "file2", "file3", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), 'd', "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );

  auto FileList = commandArgsParsedMapFiles(Map.get());
  ASSERT_NE( nullptr, FileList );
  ASSERT_STREQ( "file1", FileList[0] );
  ASSERT_STREQ( "file2", FileList[1] );
  ASSERT_STREQ( "file3", FileList[2] );
  ASSERT_STREQ( nullptr, FileList[3] );
}

// ---------------------------------------------------------

TEST( parser, parse_options_files )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "file1", "-a", "argA", "--optC", "file2", "file3", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), 'd', "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( "argA", commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );

  auto FileList = commandArgsParsedMapFiles(Map.get());
  ASSERT_NE( nullptr, FileList );
  ASSERT_STREQ( "file1", FileList[0] );
  ASSERT_STREQ( "file2", FileList[1] );
  ASSERT_STREQ( "file3", FileList[2] );
  ASSERT_STREQ( nullptr, FileList[3] );
}

// ---------------------------------------------------------

TEST( parser, parse_options_files_end )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "file1", "-a", "argA", "--", "--optC", "file2", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), 'd', "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Map.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Map.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Map.get()) );
  EXPECT_STREQ( "argA", commandArgsParsedMapShortOptionValue(Map.get(),'a') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'b') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Map.get(),'d') );

  auto FileList = commandArgsParsedMapFiles(Map.get());
  ASSERT_NE( nullptr, FileList );
  ASSERT_STREQ( "file1",  FileList[0] );
  ASSERT_STREQ( "--optC", FileList[1] );
  ASSERT_STREQ( "file2",  FileList[2] );
  ASSERT_STREQ( nullptr,  FileList[3] );
}

// ---------------------------------------------------------

TEST( parser, parse_copy )
{
  std::unique_ptr< char*[], decltype(&freeArgv) > Argv( createArgv( "program", "file1", "-a", "argA", "--optC", "file2", "file3", nullptr ), &freeArgv );
  std::unique_ptr< commandArgsParser, decltype(&commandArgsParserDelete) > Parser( commandArgsParserCreate(), &commandArgsParserDelete );

  commandArgsParserAddOption( Parser.get(), 'a', "optA", 1 );
  commandArgsParserAddOption( Parser.get(), 'b', "optB", 0 );
  commandArgsParserAddOption( Parser.get(), 'c', "optC", 0 );
  commandArgsParserAddOption( Parser.get(), 'd', "optD", 0 );

  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Map( commandArgsParserParse(Parser.get(),Argv.get()), &commandArgsParsedMapDelete );
  std::unique_ptr< commandArgsParsedMap, decltype(&commandArgsParsedMapDelete) > Copy( commandArgsParsedMapCopy(Map.get()), &commandArgsParsedMapDelete );

  EXPECT_NE( nullptr, Copy.get() );
  EXPECT_EQ( 1, commandArgsParsedMapIsSuccess(Copy.get()) );
  EXPECT_STREQ( "program", commandArgsParsedMapProgramName(Copy.get()) );
  EXPECT_STREQ( "argA", commandArgsParsedMapShortOptionValue(Copy.get(),'a') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Copy.get(),'b') );
  EXPECT_STREQ( COMMAND_ARGS_PARSER_MAP_EXISTS, commandArgsParsedMapShortOptionValue(Copy.get(),'c') );
  EXPECT_STREQ( nullptr, commandArgsParsedMapShortOptionValue(Copy.get(),'d') );

  auto FileList = commandArgsParsedMapFiles(Copy.get());
  ASSERT_NE( nullptr, FileList );
  ASSERT_STREQ( "file1", FileList[0] );
  ASSERT_STREQ( "file2", FileList[1] );
  ASSERT_STREQ( "file3", FileList[2] );
  ASSERT_STREQ( nullptr, FileList[3] );
}

// =========================================================

