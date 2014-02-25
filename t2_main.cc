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

using namespace std;

#include "T2.h"
#include "Socket.h"

int SplitName(string &s1, string &s2, const string &sname);
int SplitAddr(string &s1, string &s2, const string &sname);

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

void *inputThread(void *data)
{
  T2 * t2 = (T2 *)data;
  sClient *sc = t2->client;

  int rc=1;
  char buffer[2048];
  string prompt="\nthings=>";
  string reply;
  char * rep;

  cout << " Input  thread  created for {" << t2->name<<"]" << "\n";

  while (rc >0) {
    ostringstream ocout;
    prompt="\n"+ t2->name+"=>";

    if ( rc > 0 ) {
      rc = SendClient(sc->sock, prompt);
    }
    rc = RecvClient(sc->sock, buffer, sizeof buffer);
    string cmd = (string)buffer;
    cout << " got rc ["<<rc<<"] cmd ["<< cmd<<"]\n";
    rc = SendClient(sc->sock, cmd);
  }
  cout <<"client closed \n";
  close(sc->sock);  
  return NULL;
}


// create a thread to process a client
void input_client(int sock, void *data, struct sockaddr_in *client)
{
  T2 * t2 = (T2*)data;
  sClient *sC = new sClient(sock);
  t2->client =  sC;
  int rc = pthread_create(&sC->thr, NULL, inputThread, data);
   //Clients.push_back(j);
}

int main(int argc, char *argv[])
{
  string s1, s2, s3, s4;
  string name, port,ipaddr;

  if (( argc == 1 ) || ((string)argv[1] == "test"))
  {
      runTest();
  }
  else
  {
      if(SplitName(s1,s2,(string)argv[1]) > 1)
      {
	  name = s1;
	  
	  if (SplitAddr(s3,s4,s2) > 1) 
	  {
	      ipaddr = s3 ;
	      port = s4;
	      cout << "name ["<<name<<"] starting a connection to host:port ["
		   <<ipaddr<<":"<< port <<"] \n";
	  }
	  else 
	  {
	      port = s3;
	      cout << "name ["<<name<<"] starting a service on port ["<< port <<"] \n";
              if (! Kids[name]) 
		{
		  Kids[name] = new T2(name);
		}
	      
	      socketServer(atoi(port.c_str()), input_client, (void *) Kids[name]);
	  }
      }
      else
      {
	  
	cout << "Nothing to do " << endl;
      }
  }

  return 0;
}
