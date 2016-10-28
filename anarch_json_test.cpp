#include <string>
#include <sstream>
#include <iostream>

#include "anarch_json.h"

main()
{
try{
  std::string s = " prm : {output =../hello.txt # comment 1  \n  "
                  " lj = { rules = LB # comment2 \n"
                  "      }  \n "
                  " coulomb = { type=ewald,  eps = 1e-4     } \n"
                  " atom ={ type =  [ H C O ]  \n"
                        "   mass =  [ 1 12 16 ] \n }\n"
;

  std::stringstream ss(s);

  std::cout<<"INPUT:"<<s;

  anarch_json::param prm;
  anarch_json::read_param(ss,prm);

  std::cout<<prm<<std::endl;

  std::cout<<prm["output"]<<std::endl;
  std::cout<<prm[0]<<std::endl;
  std::cout<<prm["coulomb"]["type"]<<std::endl;
  std::cout<<(std::string)prm["atom"]["type"][2]<<std::endl;
  double total_mass = 0;
  int i;
  for(i=0;i< len(prm["atom"]["mass"]); i++ )
  {
    total_mass += (double)prm["atom"]["mass"][i];
  }
  
  std::cout<<total_mass<<std::endl;  

}catch(std::string err)
{
std::cout<<err<<std::endl;

}

  
}
