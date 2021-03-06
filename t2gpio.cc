/*******************************************
 **
 ** gpiolib for T2.cc
 **
 ** ./T2 name@port starts a service called name 
 **  on port 1234 
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

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;

#include "T2.h"


int gpioScan(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: scanning ["<<t2->name<<"] \n";
  return 0;
}

int gpioGet(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: gpio Get ["<<t2->name<<"] \n";
  return 0;
}

int gpioSet(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: Set ["<<t2->name<<"] \n";
  return 0;
}

int gpioShow(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: Show ["<<t2->name<<"] \n";
  os << t2;
  return 0;
}

extern "C"
{
int setup(ostream &os, T2 *t2)
{
    t2->addAction("scan", (void*)gpioScan);
    t2->addAction("get", (void*)gpioGet);
    t2->addAction("set", (void *)gpioSet);
    t2->addAction("show", (void *)gpioShow);
    t2->addKid("gpio_1","?pin=1&dir=input");
    t2->addKid("gpio_2","?pin=2&dir=input");
    t2->addKid("gpio_3","?pin=3&dir=input");
    os<<t2; 
   return 0;
} 
}
#if 0
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
  string foo1("foo1/starts?ip=1234&node=1122/with?dir=out&value=1/slash");
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


int StringNewName(string& new_name, string &addr, string &port, const string &sin);
int main(int argc, char *argv[])
{
  string s1, s2, s3, s4;
  string new_name, port,ipaddr;
  string argin;
  string sname, attrs, remains;

  if (( argc == 1 ) || ((string)argv[1] == "test"))
  {
      runTest();
  }
  else
  {
    if(argc > 1) 
      {
	argin = string(argv[1]);
	int isMe=SplitString(sname, attrs, remains, argin);
	cout << "src ["<< argin <<"] sname ["<<sname<<"] attrs ["<<attrs<<"] remains ["<< remains<<"] "<< endl;
	//return t2;
	T2 * myt = new T2();;
	/// look for leading slash, if found, affect me
	//if(isMe)

        int rc =  StringNewName(new_name, ipaddr, port, sname);
	cout << "] isMe ("<< isMe <<") \n";
        switch (rc) 
	  {
	  case 1:
	    cout << "Just a name ..["<< new_name <<"] isMe ("<< isMe <<") \n";
            myt<<argin;
	    break;
	  case 2:
	    cout << " run a server name ..["<< new_name <<"] port [" << port <<"] \n";
            myt<<argin;
	    break;

	  case 3:
	    cout << " run a link name ..["<< new_name <<
	                 "] ipaddress [" << ipaddr<<"] port [" << port <<"] \n";
	    break;

	  default:
	    cout << " whatever ["<< argin<<"] \n";
	    break;
	  }
	cout << "quitting for now " << endl;

      }
    else
      {
	
	cout << "Nothing to do " << endl;
      }
  }

  return 0;
}
#endif
