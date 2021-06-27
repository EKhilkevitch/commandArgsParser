
#include "commandArgsParser/parser.h"

#include <iostream>

int main( int argc, char **argv )
{

  commandArguments::parser Parser;
  Parser.addOption( 'h', "help" );
  Parser.addOption( 'a', "all", commandArguments::parser::NoArgument );
  Parser.addOption( 'r', "regex", commandArguments::parser::HasArgument );
  Parser.addOption( 't' );
  Parser.addOption( "exclude", commandArguments::parser::HasArgument );
  Parser.addOption( "include", commandArguments::parser::HasArgument );

  commandArguments::map Map = Parser( argv );

  if ( ! Map.isSuccess() )
  {
    std::cerr << "Error while parsing arguments: " << Map.failMessage() << std::endl;
    return 2;
  }

  if ( Map.exists('h') )
  {
    std::cerr << "This program justdemonstrate 'commandArgsParser' library usage and nothing mode. Please see source code." << std::endl <<
                 "Usage: " << Map.program() << " [-h|--help]" << std::endl << 
                 "          [-a|--all] [-r|--regex REGEX] [-t] [--exclude NAME] [--include NAME] FILES..." << std::endl;
    return 0;
  }


  std::cout << "program name: '" << Map.program() << "'" << std::endl;
  std::cout << "options:" << std::endl;
  std::cout << "  [-a|--all] is (" << ( Map.exists('h') ? "on" : "off" ) << ")" << std::endl;
  std::cout << "  [-r|--regex] value is (" << ( Map.exists('r') ? Map.value('r') : "off" ) << ")" << std::endl;
  std::cout << "  [-t] is (" << ( Map.exists('t') ? "on" : "off" ) << ")" << std::endl;
  std::cout << "  [-exclude] is (" << Map.value("exclude","off") << ")" << std::endl;
  std::cout << "  [-include] is (" << Map.value("include","off") << ")" << std::endl;

  const std::vector<std::string> Files = Map.files();
  if ( ! Files.empty() )
  {
    std::cout << "files:" << std::endl;
    for ( std::vector<std::string>::const_iterator f = Files.begin(); f != Files.end(); ++f )
      std::cout << "  " << *f << std::endl;
  }

  return 0;

  (void)argc; /* just for remove warning */
}

