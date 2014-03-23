/*******************************************
 **
 ** main for T2.cc
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
#include <dlfcn.h>
#include <unistd.h>

using namespace std;

#include "T2.h"
//#include "Socket.h"
#include "Strings.h"
int mainList(ostream& os, T2 *t2, void *data);
int mainjList(ostream& os, T2 *t2, void *data);
int mainHelp(ostream& os, T2 *t2, void *data);

int setup(ostream &os, T2 *t2 , void *data);


tMap Kids;
tMap Types;
tMap gActions;


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

typedef  int (*setup_t)(ostream &os, T2 *t2, void *data);


int runLibTest2(const string& sin)
{
  T2 *t2;
  string sname;
  string sact;
  string slib;
  string satr;
  
  int rc = DecodeName(sname, slib, sact, satr, sin);
  
  cout <<" RC ("<<rc<<")\n";
  cout << " name ["<<sname<<"] \n";
  cout << " lib ["<<slib<<"] \n";
  cout << " action ["<<sact<<"] \n";
  cout << " attrs ["<<satr<<"] \n";
  return 0;
  t2=new T2(sname);
  setup(cout, t2, NULL);

  if ( 0 &&(slib != "")) 
    {
      T2 *t2_t;

      if (!Types[slib])
	{
	  cout << " Creating Type [" << slib <<"]\n";
	  string dlname = "./libt2"+slib+".so";
	  void * handle;
	  action_t setup;
	  Types[slib] = new T2(slib);
	  t2_t = Types[slib];

	  handle =  dlopen(dlname.c_str(), RTLD_NOW);
	  if (!handle) 
	    {
	      cout << "dlopen "<< (char *)dlerror()<<"\n";
	      return -1;
	    }

	  dlerror();  /* clear any current error */
	  setup = (action_t)dlsym(handle, "setup");
	  char *error = (char *)dlerror();
	  if (error != NULL) {
	    cout << "dlsym error \n"<<(char *)dlerror()<<"\n";
	    return -1;
	  }
	  int ret = setup(cout, t2_t, NULL);
	}
      t2_t =  Types[slib];
      cout << t2_t;
      t2->copyAttrs(cout, t2_t); 
      t2->t2_type = t2_t;
    }

  cout << " Setting attrs \n";
  t2->SetAttrs(satr);

  cout << " T2 show \n";
  //cout << t2;
  cout << " running scan action \n";

  //t2->RunAction(cout, "list", NULL);


  return 0;

}

int runLibTest(void)
{
    T2 * t2;
    void *handle;
    setup_t setup;
     // T2->addLib

    cout << "Create a Kid with a Lib " << endl;

    t2 = Types["gpios"]=new T2("gpios");
       
    handle =  dlopen("./libt2gpios.so", RTLD_NOW);
    if ( !handle) {
      cerr << "dlopen "<< (char *)dlerror()<<"\n";
      return -1;
    }

    dlerror();  /* clear any current error */
    setup = (setup_t)dlsym(handle, "setup");
    char * error = (char *)dlerror();
    if (error != NULL) {
      cout << "dlsym error \n"<<(char *)dlerror()<<"\n";
      return -1;
    }
    int ret = setup(cout, t2, NULL);
    cout << " Setup return value [" <<ret<<"] \n";
    if (ret == 0)
      {
	t2->RunAction(cout, "!scan", NULL);
	t2->RunAction(cout, "!show", NULL);
      }
    return 0;
}

int runTcpTest(void)
{

    T2 * t2;
    void *handle;
    setup_t setup;
     // T2->addLib


    cout << "Create a Kid with a Lib " << endl;

    t2 = Types["tcps"]=new T2("tcps");
    t2->AddAction("!list",  (void *)mainList, true);
    t2->AddAction("!jlist",  (void *)mainjList, true);
    //    t2->AddAction("list",  (void *)mainList);
       
    handle =  dlopen("./libt2tcps.so", RTLD_NOW);
    if ( !handle) {
      cerr << "dlopen "<< (char *)dlerror()<<"\n";
      return -1;
    }

    dlerror();  /* clear any current error */
    setup = (setup_t)dlsym(handle, "setup");
    char * error = (char *)dlerror();
    if (error != NULL) {
      cout << "dlsym error \n"<<(char *)dlerror()<<"\n";
      return -1;
    }
    int ret = setup(cout, t2, NULL);
    cout << " Setup return value [" <<ret<<"] \n";
    if (ret == 0)
      {
	t2->RunAction(cout, "!scan", NULL);
	t2->RunAction(cout, "!show", NULL);
      }

    return 0;
}

int runTest(void)
{

  cout << "Create a few and show them " << endl;
  Kids["one"]=new T2("one");
  Kids["two"]=new T2("two");
  Kids["gpios"]=new T2("gpios");
  Kids["gpios"]->AddKid("gpio_1");

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

int mainHelp(ostream& os, T2 *t2, void *data)
{
  os << "mainAction: help ["<<t2->name<<"] \n";
  return 0;
}

int mainList(ostream& os, T2 *t2, void *data)
{
  os << "mainAction: list ["<<t2->name<<"] \n";
  os << t2;
  return 0;
}

int mainjList(ostream& os, T2 *t2, void *data)
{
  os << "\n";
  t2->jShow(os);
  return 0;
}


int setup(ostream &os, T2 *t2 , void *data)
{
  t2->AddAction("!help", (void*)mainHelp, true);
  //t2->AddAction("scan",  (void*)tcpScan);
  //t2->AddAction("get",   (void*)tcpGet);
  //t2->AddAction("set",   (void *)tcpSet);
  t2->AddAction("!list",  (void *)mainList);
  t2->AddAction("list",  (void *)mainList);
  //    t2->SetAttrs((string)"?port=5566");
  return 0;
}

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
  else if (( argc == 1 ) || ((string)argv[1] == "lib"))
  {
      runLibTest();
  }
  else if (( argc == 1 ) || ((string)argv[1] == "tcp"))
  {
      runTcpTest();
  }
  else if (( argc == 1 ) || ((string)argv[1] == "name"))
  {
    if (argc > 2) {
      runLibTest2((string)argv[2]);
    }
    if (argc > 3) {
      runLibTest2((string)argv[3]);
    }
    return 0;
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
