/*******************************************
 **
 ** main for T2.cc
 **
 ******************************************/

#include <iostream>
#include <limits>
#include <map>
#include <cstring>
#include <string>
#include <sstream>
#include <istream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unistd.h>

using namespace std;

#include "T2.h"
#include "Socket.h"


tMap Kids;

// todo add this as another operator
int showKids(ostream& os)
{ 
  tMap::iterator iter;
  for (iter=Kids.begin(); iter != Kids.end(); ++iter)
    {
      os << Kids[iter->first];
    }
  return 0;
}

int runTest(void)
{

  cout << "Create a few and show them " << endl;
  Kids["one"]=new T2("one");
  Kids["two"]=new T2("two");
  Kids["gpios"]=new T2("gpios");
  Kids["gpios"]->addKid("gpio_1");

  Kids["gpios"]->getKid("gpio_1")->addAttr("pin","1");
  Kids["gpios"]->getKid("gpio_1")->addAttr("dir","out");
  string foo = "gpio_2";
  Kids["gpios"]<< foo;
  Kids["gpios"]<< (string)"/pwms/pwm1/output";
  string foo1("/foo1/starts?ip=1234&node=1122/with?dir=out&value=1/slash");
  string foo2("/foo1/starts/with/slash");
  Kids["gpios"] << foo1;
  cout<< endl<<"Testing Show" << endl << endl;

  Kids["gpios"]->Show(cout);
  cout<< endl<<"Testing << operator" << endl << endl;
  cout << Kids["gpios"];

  cout<< endl<<"Testing all Kids" << endl << endl;
  showKids(cout);

  cout << "testing some string stuff" << endl;


  size_t start = 0;
  size_t eq1 = foo1.find('/',start);
  size_t eq2 = foo1.find('/',start+1);
  cout << "eq1 ("<<eq1<<") eq2 (" <<eq2 <<") foo1["<<foo1<<"]"<< endl;
  string name, attrs,remains,src;
  src="/some?dir=our&name=foo/funny/string/";
  int rc = SplitString(name, attrs, remains, src);
  cout <<  " name["<<name<<"] attrs ["<<attrs<<"] remains ["<<remains<<"] src ["<<src<<"]"<<endl;

}

int main(int argc, char *argv[])
{

  if (( argc == 1 ) || ((string)argv[1] == "test"))
    {
      runTest();
    }
  else
    {
      cout << "Nothing to do " << endl;
    }
  return 0;
}
