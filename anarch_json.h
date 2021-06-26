#ifndef anarch_json_h
#define anarch_json_h

#include <string>
#include <map>
#include <list>
#include <set>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

namespace anarch_json {

///  param - class that contains the parameters tree
///  use operator[]  to access the sub-nodes of the tree
//     in the brackets is either a name or a number of sub-node
//     consider also a len() function
///  use push_back() to add sub-nodes (consider also read_param function below)
///  conversions to string and double defined
//
///  example code:
///     param a("a","x");
///     param b("b","55.3");
///     param c("z"); c.push_back(a); c.push_back(b);

///     std::cout<<c<<std::endl;

///     std::string cc = c["b"];
///     double dd = c["b"];
///     int ii = c["a"];
///     std::cout<<cc<<dd<<ii<<std::endl;
class param {

protected:
  std::string m_name;
  std::string m_data;
  std::list< param > m_children; // not vector, because vector can grow and change the data locations!!!
  std::map<int, param *> m_id_map; // acces by number (as in vector)
  std::map< std::string, param *> m_name_map; // acces by name

public:

  param( std::string name="", std::string data="" ) 
   : m_name(name), m_data(data) {}
  
  const std::string & name() const  { return m_name;}
        std::string & vname() {return m_name;}

  const std::string & data() const  { return m_data;}
        std::string & vdata() { return m_data;}

  const std::list< param > & children() const  { return m_children;}
        std::list< param > & vchildren() { return m_children; }

  param & child_by_name  (std::string  name)  {  
    if(m_name_map.find(name) == m_name_map.end()) throw std::string("sub-parameter ") + name + " of parameter " + this->name() + " not found";
    return *(m_name_map.find(name)->second);
   }

   param & child_by_id(int id)  {  
    if(m_id_map.find(id) == m_id_map.end()) {
       char s[256]; sprintf(s,"%d",id);  
       throw std::string("invalid id ") + s+ " of parameter " + this->name();
    }
    return *(m_id_map.find(id)->second);
   }
  
  void push_back( param & child ) 
   {  m_children.push_back(child); 
      m_id_map[ m_children.size()-1] = &child; 
      link( child ) ;
   }

  param &push_new() {
      m_children.push_back( param() ); 
      m_id_map[ m_children.size()-1] = &m_children.back();
      char str[256];
      sprintf(str,"%d",(int)m_children.size()-1);
      m_children.back().vname() = str;
      return m_children.back(); 
  }
  
  param &push_new( const char *_name, const char *_data="" )
  {
  	param & p = push_new();
	p.vname() = _name;
	p.vdata() = _data;
	link(p);
	return p;
  }

  param &push_new( const char *_name, double d )
  {
	char s[256];
	sprintf(s,"%30.20lf",d);  
  	return push_new(_name,s); 
  }

  param &push_new( const char *_name, int d )
  {
  char s[256];
  sprintf(s,"%d",d);  
    return push_new(_name,s); 
  }

  param &push_data( const char * _data )
  {
  	param &p = push_new();
	p.vdata() = _data;
	return p;
  }

  param &push_data( double d )
  {
  	char s[256];
	sprintf(s,"%30.20lf",d);
	return push_data(s);
  }

  param &push_data( int d )
  {
    char s[256];
  sprintf(s,"%d",d);
  return push_data(s);
  }


  void link( param & child ) { m_name_map[ child.name() ] = &child; }

   param & operator [] ( int i ) { return child_by_id(i); }
   param & operator [] ( const std::string &name) { return child_by_name(name);   }

   bool hasParam( const std::string &name) const { return m_name_map.find(name) != m_name_map.end();  }

   operator const char *() const { return data().c_str(); }
   operator const std::string &() const { return data(); }
   operator const double ()  const { return strtod(data().c_str(),NULL); }

};

/// number of childs of a param
inline int len(const param &prm) {return prm.children().size();} 

/// output in format  a:{b:{c:{}}...
std::ostream & operator<<(std::ostream &os, const param &prm);


//  read param from file
//  the format is "anarchoJson" 
//   name_value_pair is value or  name : value or name = value
//   name is a  (alphanumeric) string without quotes or 
//               string with any chars in quotes (double or single). To insert the quote char in the string - just repeat it. 
//   value is also a string or a "complex value", which is 
//                     {  name_value_pair1 , name_value_pair2, ... }
//                  or [  name_value_pair1 , name_value_pair2, ...  ]
//     comas can be omitted.
//     unnamed parameters will get the names according to their number. 
// example use:
/*
std::string s = " a = # Comment1 \n { x :  'aa\";' , #Comment2 \n  e : [ x :'f g' 7:y z] }    ";
std::stringstream ss(s);
param p;
read_param( ss, p);
std::cout << p << std::endl;
*/
void  read_param( std::istream &is, param &r , int id = 0); 

void read_params( std::istream &is, param &root );



namespace parse {


// skip the characters in input stream which are in delim set 
void skip( std::istream &is,const std::set<char> & delim);

// the same as above, but instead of a set use the string
void skip( std::istream &is, const std::string &delim); 
 
// skip  space tab and newlines
inline void skip_space( std::istream &is ); // { skip(is, " \t\n" ); }

// skips spaces and comment. comments start with comment char (by defaul #) and end with a newline
void skip_space_and_comments( std::istream &is, char comment_char = '#' );

// get character from istream and cheks that it belongs to the set of chars range
// on success return the caracter otherwise throws an exception
char character( std::istream &is, const char *range); 


// base class for is... functionals (see below). 
// defines a char-table and the functions to assign the range of chars (will be used in derived classes)
// also, provides operator()  which make it possible to use the class as function ( issomething(...) for derived object issomething )
class _ischar {

 private:
   bool chartab[256];

 protected:

   void assign_chars( const char *chars, bool value = true) 
   {
      const char *ptr;
      for( ptr = chars; *ptr; ptr++)  chartab[(unsigned char)(*ptr)] = value;
   }
   void remove_chars( const char *chars) { assign_chars(chars,false); }

   void assign_char(unsigned char ch, bool value=true) { chartab[ch]=value; }
   void remove_char(unsigned char ch) { assign_char(ch,false); } 

   void assign_range(unsigned char from,unsigned char to,bool value = true) 
   {
      int ch;
      for(ch=from; ch<=to; ch++) chartab[ch] = value;
   }
   void remove_range(unsigned char from, unsigned char to) { assign_range(from,to,false); }

 
 public:

  // by default empty range is selected. derived classes will change that behavior. 
  _ischar() {  remove_range(0,255);  }

  // enables use as a functional
  bool operator()(unsigned char ch)  { return chartab[ch];  }

};


// a-zA-Z_
class _isalpha : public virtual _ischar {
  public:
   _isalpha() {
        assign_range('a','z');
        assign_range('A','Z');
        assign_char('_');
    }
};

// 0-9
class _isnum : public virtual _ischar {
 public:
  _isnum() {
     assign_range('0','9');
  }
};


// alha or number
class _isalphanum : public virtual _isalpha, public virtual _isnum {
public:
   _isalphanum() { }
};


// includes ./ , wich make it possible to define a path without quotes
class _isstrchar: public virtual _isalphanum {
public:
   _isstrchar()  {
      assign_chars("./-+");
   }
};


/*------------------------*/
// global objects which implement the functionals
// usage as functions, e.g isalpha('x') or isnum('8') or etc
extern _isalpha    isalpha;
extern _isnum      isnum;
extern _isalphanum isalphanum;
extern _isstrchar  isstrchar;
/*------------------------*/


// gets string from the istream 
// string - sequence of strchars (alphanum + ./_)
// if something else is in the stream throws the (string)  exception
std::string unquoted_string( std::istream & is );


// gets the quoted string from the istream
// quoted string is  <quote><nonquotechars><quote>
// quote char is given
// to include the quote char in the string one need to repeat it twice
std::string quoted_string( std::istream &is);

// gets string from istream
// the string is either a quoted or unquoted string
std::string str( std::istream &is );


}

}

#endif
