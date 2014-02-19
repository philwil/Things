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


tMap T2s;


int runTest(void)
{
  cout << "Create a few and show them " << endl;
  T2s["one"]=new T2("one");
  T2s["two"]=new T2("two");
  T2s["gpios"]=new T2("gpios");
  T2s["gpios"]->addKid("gpio_1");

  T2s["gpios"]->getKid("gpio_1")->addAttr("pin","1");
  T2s["gpios"]->getKid("gpio_1")->addAttr("dir","out");
  string foo = "gpio_2";
  T2s["gpios"]<< foo;
  T2s["gpios"]<< (string)"gpio_3";
  T2s["gpios"]->Show("  ");
  cout<< " Another test" << endl;
  cout << T2s["gpios"];

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
