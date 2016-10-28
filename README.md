Procedures to work with "anarchoJson" format.
The format is similar to json, but with much less restrictions

the format of "anarchoJson" 
 name_value_pair is value or  name : value or name = value
 name is a  (alphanumeric) string without quotes or 
               string with any chars in quotes (double or single). To insert the quote char in the string - just repeat it. 
   value is also a string or a "complex value", which is 
                     {  name_value_pair1 , name_value_pair2, ... }
                  or [  name_value_pair1 , name_value_pair2, ...  ]
     comas can be omitted.
     unnamed parameters will get the names according to their number. 

After creating of param structure, (with read_param function) one can access the fileds with operator[], i.e
param p;
std::cout<<p["hello"]["world"] ;


 example use:

std::string s = " a = # Comment1 \n { x :  'aa\";' , #Comment2 \n  e : [ x :'f g' 7:y z] }    ";
std::stringstream ss(s);
param p;
read_param( ss, p);
std::cout << p << std::endl;

see also anarch_json_test.cpp

to compile:
  g++ -o anarch_json_test  anarch_json_test.cpp anarch_json.cpp
tp run
  ./anarch_json_test


