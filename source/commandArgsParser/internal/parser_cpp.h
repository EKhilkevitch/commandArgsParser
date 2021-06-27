
#pragma once

// =========================================================

#include <exception>
#include <string>
#include <vector>

// =========================================================

namespace commandArguments
{

  // ---------------------------------------------------------
  
  class map;
  class parser;
  
  // ---------------------------------------------------------
  
  class exception : public std::exception
  {
    private:
      std::string What;

    public:
      explicit exception( const std::string &What );
      ~exception() throw();
      const char* what() const throw();
  };
  
  // ---------------------------------------------------------
  
  class map
  {
    friend class parser;
    private:
      struct impl;

    private:
      impl *Impl;

    public:
      map();
      map( const map &Map );
      map& operator=( const map &Map );
      ~map();

      void swap( map &Other );

      bool isSuccess() const;
      std::string failMessage() const;

      bool exists( char Short ) const; 
      std::string value( char Short, const std::string &Default = std::string() ) const;
      bool exists( const std::string &Long ) const;
      std::string value( const std::string &Long, const std::string &Default = std::string() ) const;
      bool exists( const char *Long ) const;
      std::string value( const char *Long, const std::string &Default = std::string() ) const;

      std::vector<std::string> files() const;
      std::string program() const;
  };
  
  // ---------------------------------------------------------

  class parser
  {
    public:
      enum argument
      {
        HasArgument,
        NoArgument
      };

    private:
      struct impl;

    private:
      impl *Impl;

    public:
      parser();
      parser( const parser &Parser );
      parser& operator=( const parser &Parser );
      ~parser();

      void swap( parser &Other );

      void addOption( char Short, argument Arguement = NoArgument );
      void addOption( const std::string &Long, argument Arguement = NoArgument );
      void addOption( const char *Long, argument Arguement = NoArgument );
      void addOption( char Short, const std::string &Long, argument Arguement = NoArgument );
      void addOption( char Short, const char *Long, argument Arguement = NoArgument );

      map parse( char **argv );
      map operator()( char **argv );
  };
  
  // ---------------------------------------------------------

}

// =========================================================

