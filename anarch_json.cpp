#include "anarch_json.h"
//#include "anarch_json_parse.h"
#include <stdio.h>

using namespace anarch_json;

std::ostream & anarch_json::operator<<(std::ostream &os, const param &prm)
{
   os<<prm.name()<<":"<<prm.data();
   if(len(prm)>0)
   {
      os<<"{";
      std::list<param>::const_iterator it;
      for( it = prm.children().begin();
           it != prm.children().end();
           it++)
      {
         if( it != prm.children().begin()) os<<",";
         os<<*it;
      }

      os<<"}";
   }
  return os;
}

void  anarch_json::read_param( std::istream &is, param &r , int id) 
{
  parse::skip_space_and_comments( is );
  std::string name = parse::str( is );
  r.vname() = name;

  parse::skip_space_and_comments( is );

  if( is.peek() != ':' && is.peek() != '=' ) 
   { r.vdata() = name;
     
     char s[256];  sprintf(s,"%d",id);
     r.vname() = s;
     return;
   }

  char sign = parse::character(  is , ":=");

  parse::skip_space_and_comments( is );
  
  if( is.peek() != '{' && is.peek() != '[' ) 
  {
     r.vdata() = parse::str( is );
     return;
  }

  is.get();

 
  do {
        if( is.peek() == ',')  is.get();  
         param &p  = r.push_new();
         read_param(is, p , len(r)-1 );
         r.link(p);
         parse::skip_space_and_comments( is );
   } while( !is.eof() && is.peek() != '}' && is.peek() != ']' );           
  if(!is.eof()) is.get();

}

void anarch_json::read_params( std::istream &is, param &root)
{ int id = 0;
   while( !is.eof() )
   {
      parse::skip_space_and_comments(is);
      if(! is.eof() ) {
         if( is.peek() == ',') is.get();

         param &p =root.push_new( );
         read_param( is, p,id );
         root.link(p);
         id++ ;
      }
   }

}


using namespace anarch_json::parse;

void anarch_json::parse::skip( std::istream &is,const std::set<char> & delim)
{
  while( !is.eof() && delim.find(is.peek() ) != delim.end() ) is.get();
}

void  anarch_json::parse::skip( std::istream &is, const std::string &delim) 
{
  std::set<char> delim_set; 
  std::string::const_iterator it;
  for( it = delim.begin(); it!=delim.end(); it++)  delim_set.insert(*it);
  skip( is, delim_set );
}

void  anarch_json::parse::skip_space( std::istream &is ) { skip(is, " \t\n" ); }

void  anarch_json::parse::skip_space_and_comments( std::istream &is, char comment_char ) 
{
  do{  
   skip_space(is);
   if( is.peek() == comment_char )
   { int ch;
      do{ ch = is.get(); }  while( !is.eof() && ch != '\n' );
   }
  
  } while( !is.eof() && (is.peek() == ' ' || is.peek() == '\t' || is.peek() == '\n' || is.peek() == comment_char  ));
}


char  anarch_json::parse::character( std::istream &is, const char *range) 
{
  const char *ptr;
  for( ptr = range; *ptr; ptr++) 
    if( is.peek() == *ptr) return is.get();
  throw std::string("unexpected character") + (char)is.peek() +"; expected range :" + range;
}

/*------------------------*/

_isalpha    anarch_json::parse::isalpha;
_isnum      anarch_json::parse::isnum;
_isalphanum anarch_json::parse::isalphanum;
_isstrchar  anarch_json::parse::isstrchar;

/*------------------------*/

std::string anarch_json::parse::unquoted_string( std::istream & is ) {
  std::string s;

  if( !isstrchar(is.peek() ) ) throw std::string("unexpected character in identifer:") + ((char)is.peek());

  s.push_back( is.get() );

  while( !is.eof() && isstrchar( is.peek() ) )  s.push_back( is.get() );
  return s;
}

namespace anarch_json { namespace parse {

// help function for quoted string. gets the content of the string until the quote is found
std::string  quoted_string_content( std::istream &is, char quote )
{ std::string s;
  while( !is.eof() )
   {
      int ch = is.get();

      if( ch == quote) 
          if(is.peek() == quote) s += is.get();
          else return s;

      s+=ch;
   }
   return s;
}

}}

std::string  anarch_json::parse::quoted_string( std::istream &is)
{
  if(is.peek() == '\'' || is.peek() == '\"' ) 
    return quoted_string_content( is, is.get() );
  else 
    throw std::string("unexpected quote character") + (char)is.peek();
}

std::string  anarch_json::parse::str( std::istream &is ) 
{
 return  ( is.peek() == '\'' || is.peek() == '\"' )  
         ?  quoted_string(is)
         :  unquoted_string(is);
}



