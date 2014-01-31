// things.cc lets see how this works out
// solve the isa problem
// gpio_i isa gpio
// TODO move isa functions ??
//      propogate isa attrs
//
// it must deep copy all the stuff that gpio uses
// update the gpio class and all gpios get the update.
//
// add full socket thing
//   common commands.


// BUGS
// connection always forwards got it to trap on just one /


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

#include "Things.h"
#include "Socket.h"


int anyCommand (int fd, string &stuff, string &reply);
int serv_receive(int fd, string &msg, string &reply);
static void Split(vector<string>& lst, const string& input, const string& separators, bool remove);

// dummy socket stuff
int sock_connect(string ipaddress, string node, int timeout)
{
  cout << " connected to [" << ipaddress << "] node [" << node<<"]\n";
  return 3;
} 

int sock_receive(int fd, string &reply, int timeout)
{
  reply="got a reply";
  return reply.size();
}

int sock_send(int fd, string &msg)
{
  string reply;
  cout << "sending "<< msg << " To fd ("<< fd<<") \n";;

  // this is done at the receive end
  serv_receive(fd, msg, reply);
  cout <<"after serv_receive reply is ["<< reply<<"]\n";

  return msg.size();
}

// server receive command
int serv_receive(int fd, string &msg, string &reply)
{
  anyCommand(fd, msg, reply);

}

vector<string> myVector;
tMap Things;
tMap Commands;


int createNewThing(tMap &things, string name)
{
  int rc = 0;
  if (!things[name]) 
  {
    things[name]= new Thing(name);
    cout <<"new Thing [" << name <<"] created" <<"\n";
    things[name]->name = name;
    rc = 1;
  }
  else
  {
      cout <<"Thing" << name <<" Already created" <<"\n";
  }
  return rc;
}

int createNewThing(string name)
{

  return createNewThing(Things, name);
}


int ListThings(tMap &things, string dummy)
{
  tMap::iterator iter;
  for ( iter = things.begin() ; iter != things.end(); ++iter )
  {
    if (iter->second->isa.empty()) {
      cout<<dummy<<iter->first<< ": [" <<(iter->second)->value<<"]"<<endl;
    } else {
      cout<<dummy<<iter->first<< " isa [" << (iter->second)->isa <<"]" <<endl;
    }
    ListThings(iter->second->Attrs, dummy + "{attr}   ");
    ListThings(iter->second->Kids, dummy + "   ");
  }

  return 0;
}

int ListKids(tMap &things, string dummy)
{
  tMap::iterator iter;
  for (iter=things.begin(); iter != things.end(); ++iter )
  {
    if (iter->second->isa.empty()) {
      cout<<dummy<<iter->first<<endl;
      ListKids((iter->second)->Kids, dummy + "   ");
    }

  }

  return 0;
}

int KillThings(tMap &things)
{
  tMap::iterator iter;
  for ( iter = things.begin() ; iter != things.end(); ++iter )
  {
    cout << "killing ["<< (iter->second)->name <<"]" << endl;
    KillThings((iter->second)->Kids);
    delete iter->second;
  }

  return 0;
}


int setFunction (Thing &thing, string name, string stuff)
{
  cout << " running set function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return 0;
}


// set gpio look for 
// look for &?pin=, &?dir= &?value= 
int setGpioFunction (Thing &thing, string &reply, string name, string stuff)
{

  vector <string> args;
  vector <string>::iterator it;
  cout << " running set Gpio function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return thing.addAttrs(reply, stuff);
}

int getGpioFunction (Thing &thing, string &reply, string name, string stuff)
{
  // cout << " running get gpio function [" << endl;
  //return 0; 
  cout << " running get gpio function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return thing.getAttrs(reply, stuff);

}


int setNodeFunction(Thing &thing, string &reply, string name, string stuff)
{
  vector <string> args;
  Split(args, stuff, "/", true);

  cout << " running setNodefunction [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "] args size ["<<args.size() <<"]"<< endl;

  if (args.size() < 2 ) {
    cout <<"too few args .. running locally \n";
    return thing.addAttrs(reply, stuff);
  }

  if (thing.fd == 0) {
    if(!thing.Attrs["ip_address"]) {
      cout<< " no ip_address\n"<<endl;
      return -1;
    }
    if(!thing.Attrs["node"]) {
      cout<< " no node\n"<<endl;
      return -1;
    }

    thing.fd = sock_connect(thing.Attrs["ip_address"]->value, thing.Attrs["node"]->value, 2000);
  }

  if (thing.fd > 0) {
    string reply;
    string msg;
    int timeout = 2000;  // 2 seconds
    msg = "set " + stuff;
    sock_send(thing.fd, msg);
    if(sock_receive(thing.fd, reply, timeout))
      {
	cout << " setNodeFunction .. we got a reply of [" << reply <<"]\n";
      } 
    else 
      {
	cout << " setNodeFunction .. timed out ]\n";
      }
  } 
  return 0;
}

int getNodeFunction(Thing &thing, string &reply, string name, string stuff)
{
  vector <string> args;
  Split(args, stuff, "/", true);

  cout << " running getNodefunction [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;

  if (args.size() < 1 ) {
    cout <<"too few args .. running locally \n";
    return thing.getAttrs(reply, stuff);
  }

  if (thing.fd == 0) {
    if(!thing.Attrs["ip_address"]) {
      cout<< " no ip_address\n"<<endl;
      return -1;
    }
    if(!thing.Attrs["node"]) {
      cout<< " no node\n"<<endl;
      return -1;
    }

    thing.fd = sock_connect(thing.Attrs["ip_address"]->value, thing.Attrs["node"]->value, 2000);
  }

  if (thing.fd > 0) {
    string reply;
    string msg;
    int timeout = 2000;  // 2 seconds
    msg = "get " + stuff;
    sock_send(thing.fd, msg);
    if(sock_receive(thing.fd, reply, timeout))
      {
	cout << " getNodeFunction .. we got a reply of [" << reply <<"]\n";
      } 
    else 
      {
	cout << " getNodeFunction .. timed out ]\n";
      }
  } 
  return 0;
}


int getFunction (Thing &thing, string name, string stuff)
{
  cout << " running get function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return 0;
}


int showFunction (Thing &thing, string name, string stuff)
{
  cout << " running show function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return 0;
}


int helpFunction (Thing &thing, string name, string stuff)
{
  cout << " running help function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return 0;
}





int runFunction(Thing &thing, string &reply, string name, string stuff)
{
  tMap Actions;
  Actions = thing.Actions;

  if (thing.isa.empty())
    {
      cout<<"Thing ["<< thing.name<<"]" <<" isa is empty\n";
    } 
  else 
    {
      cout<<"Thing ["<< thing.name<<"]" <<" isa is <" << thing.isa<<">.. NOT empty\n";
      Actions = thing.isaThing->Actions;
    }

  if ( !Actions[name] ) 
    {
      cout << " no function [" << name << "] in Thing called [" 
	   << thing.name << "]"<< endl;
    }
  else
    {
      cout << " run function [" << name << "] in Thing called [" << thing.name  
	   << "] with [" << stuff<<"]"<< endl;
      int (*runAction) (Thing &thing, string &reply, string name, string stuff) =
	(int (*) (Thing &, string &, string, string))Actions[name]->stuff;
      if(runAction) runAction(thing, reply, name, stuff);
    }


  return 0;
}


int isa(string name, string Isa)
{

  Thing *me;
  Thing *isa;

  if (!Things[name]) 
  {
      cout << " Creating Thing called ["<<name<<"] \n"; 
      createNewThing(name);
  }
  me=Things[name];

  if (!Things[Isa]) 
  {
      cout << " Creating Isa called ["<<Isa<<"] \n"; 
      createNewThing(Isa);
  }

  isa=Things[Isa];

  me->isa = Isa;
  me->isaThing = isa;

  //  Now copy all the isa attrs to attrs
  // for thngs in  
  //createNewThing("dir");
  tMap::iterator iter;
  for (iter = isa->Attrs.begin(); iter != isa->Attrs.end(); ++iter )
  {
    cout<<" TODO Copying ["<<iter->first<<"]" <<endl;
    me->createNewThing(iter->first);
  }
  // lastle add me to the list of isa(s}
  isa->myList.push_back(me);
  return 0;

}
// some nifty stuff

static void Split(vector<string>& lst, const string& input, const string& separators, bool remove_empty)
{
    std::ostringstream word;
    for (size_t n = 0; n < input.size(); ++n)
    {
        if (std::string::npos == separators.find(input[n]))
            word << input[n];
        else
        {
            if (!word.str().empty() || !remove_empty)
                lst.push_back(word.str());
            word.str("");
        }
    }
    if (!word.str().empty() || !remove_empty)
        lst.push_back(word.str());
}

// TODO put these inline
int getJustThing(string &result, string &cmd)
{
    int rc = 0;
    vector<string> target;

    Split(target, cmd, "?", true);
    rc = target.size();
    if (rc > 0 )
       result=target[0];
    
    return rc;
}

int getJustAttrs(string &result, string &cmd)
{
    int rc = 0;
    vector<string> target;

    Split(target, cmd, "?", true);
    rc = target.size();
    cout << " Attr target size " << rc << " cmd ["<< cmd <<"] \n";
    if (rc > 1 )
       result=target[1];
    
    return rc;
}
//
// handles a single command line   command via a node if needs be
//
// we find or make all the nodes / attributes
//  sysfoo/mygpios/gpio_1/?dir=output&pin=1
//
int oneCommand (tMap &things,sClient &sclient, string &cmd, string &reply, Thing *parent)
{
  vector<string> target;
  Thing *targ=NULL;
  Thing *isa=NULL;
  int rc;

  cout << " running one command("<<cmd<<") [" 
       << "] with [" << cmd<< "]"<< endl;
  
  // find the target
  Split(target, cmd,"/", true);

  //
  // create the rest of the args
  //
  string nstr=cmd;

  nstr.erase(0,target[0].size()+1);
  cout << " target is [" << target[0] << "] next string ["<<nstr<<"]"<< endl;
    
  // target can be compound ie with attributes  //foo?name=myfoo&value=1/

  string newthing ="";
  rc = getJustThing(newthing, target[0]);
  string newattrs="";
  rc = getJustAttrs(newattrs, target[0]);

  cout << " new thing is [" << newthing <<"] \n";
  cout << " new attrs is [" << newattrs <<"] \n";
  //return 0;

  if(!things[newthing]) {
    things[newthing] = new Thing(newthing);
  }

  targ = things[newthing];
  targ->parent=parent;

  // at this point we add attributes if nstr starts with a '?'
  if (rc > 0 ) {
    targ->addAttrs(reply, newattrs);
    cout << " After addAttrs reply is [" << reply << "]\n";
  }
  // or reacll oneCommand with   nstr as an argument
  if (nstr.size() == 0) return 0;

  char ftype=nstr.at(0);

  if (ftype != '?') {
    cout << "found ["<<target[0]<<"] processing next command ["<<nstr<<"] \n";
    return oneCommand(targ->Kids, sclient, nstr, reply, targ);
  }
  cout << " Now processing attributes in [" << nstr << "]\n"; 
  return 0;
}

int anyCommand (int fd, string &cmd, string &reply)
{
  vector<string> target;
  Thing *targ=NULL;
  Thing *isa=NULL;
  
  vector <string> stuff;
  Split(stuff, cmd, " ", true);

  cout << " running any command("<<cmd<<") [" 
       << "] with [" << stuff[1]<< "]"<< endl;
  
  if (stuff[0] == "set") {

    // find the target
    Split(target, stuff[1],"/", true);

    // create the rest of the args
    //
    string nstr=stuff[1];
    nstr.erase(0,target[0].size()+1);
    cout << " set target is [" << target[0] << "]"<<endl;
    
    targ = Things[target[0]];
    isa = targ->isaThing;
    
    if (!targ)  {
      cout << " Sadly ["<<target[0]<<"] is not set up" << endl;
    } else {
      
      cout << " running command ["<< stuff[0]
	   <<"] on  ["<< targ->name
	   <<"] isa ["<< targ->isa<<"]" << " with data[" << nstr <<"]" << endl;
    }
    
    if(!targ->Actions[stuff[0]]) 
      {
	cout << " no  command ["<< stuff[0] << "] in Actions ON["<< targ->name<<"]" << endl;
      }
    else 
      {
	cout << " YES  command ["<< stuff[0] << "] in Actions" << endl;
	runFunction(*targ, reply, stuff[0], nstr);
      }
    if(!isa->Actions[stuff[0]]) 
      {
	cout << " no  ISA command ["<< stuff[0] << "] in Actions ON["<< isa->name<<"]" << endl;
      }
    else 
      {
	
	cout << " YES  command ["<< stuff[0] << "] in ISA Actions" << endl;
	runFunction(*targ, reply, stuff[0], nstr);
	
      }
  } // end of set command
  if (stuff[0] == "get") {

    // find the target
    Split(target, stuff[1],"/", true);

    // create the rest of the args
    //
    string nstr=stuff[1];
    nstr.erase(0,target[0].size()+1);
    cout << " get target is [" << target[0] << "]"<<endl;
    
    targ = Things[target[0]];
    isa = targ->isaThing;
    
    if (!targ)  {
      cout << " Sadly ["<<target[0]<<"] is not set up" << endl;
    } else {
      
      cout << " running command ["<< stuff[0]
	   <<"] on  ["<< targ->name
	   <<"] isa ["<< targ->isa<<"]" << " with data[" << nstr <<"]" << endl;
    }
    
    if(!targ->Actions[stuff[0]]) 
      {
	cout << " no  command ["<< stuff[0] << "] in Actions ON["<< targ->name<<"]" << endl;
      }
    else 
      {
	cout << " YES  command ["<< stuff[0] << "] in Actions" << endl;
	runFunction(*targ, reply, stuff[0], nstr);
      }
    if(!isa->Actions[stuff[0]]) 
      {
	cout << " no  ISA command ["<< stuff[0] << "] in Actions ON["<< isa->name<<"]" << endl;
      }
    else 
      {
	
	cout << " YES  command ["<< stuff[0] << "] in ISA Actions" << endl;
	runFunction(*targ, reply, stuff[0], nstr);
	
      }
  } // end of get command

  return 0;
}


int getCommand (vector <string> stuff)
{
  cout << " running set command [" 
       << "] with [" << stuff.at(1)<< "]"<< endl;
  return 0;
}
int showCommand (vector <string> stuff)
{
  cout << " running show command [" 
       << "] with [" << stuff.at(1)<< "]"<< endl;
  return 0;
}
int helpCommand (vector <string> stuff)
{
  cout << " running help command [" 
       << "] with [" << stuff.at(1)<< "]"<< endl;
  return 0;
}



int setValue(map<string, Thing*>&things, string name, string object, string value)
{
  if(!things[name]) 
    {
      cout << " SetValue No thing ["<<name<<"] found \n";
      return -1;
    }
  if(!things[name]->Attrs[object])
    {
      cout << " SetValue thing "<<name<<" no object " << object<<"] found \n";
      return -1;
    } 
  things[name]->Attrs[object]->value = value;
  
  return 0;
}

int doCommand(string cmd) 
{
  vector <string> myStrings;
  Split(myStrings, cmd," ", true);
  string reply;
  //anyCommand
  // command is of a set or get type
  // 
  vector <string>::iterator it;
  for (it = myStrings.begin(); it != myStrings.end(); ++it) 
  {
      cout << *it << endl;
  }

  if (myStrings.size() == 1) 
    {
      int (*runCommand) (int, string&, string&) =
	(int (*) (int, string&, string&))Commands["any"]->stuff;
      runCommand(0, cmd, reply);

      return 0;
    }
  if (Commands[myStrings.at(0)])
    {
      string reply;
      int (*runCommand) (int, string&, string&) =
	(int (*) (int, string&, string&))Commands[myStrings.at(0)]->stuff;

      void *foo = Commands[myStrings.at(0)]->stuff;
      cout << " Running command ["<<foo<< "] " << endl;
      runCommand(0, cmd, reply);
      cout << "ran command reply is ["<<reply<<"]\n";
    }
  cout << endl << endl;  
 return 0;
}


int runTest(void)
{
  createNewThing(Things,"sysfoo");

  createNewThing(Commands,"any");
  createNewThing(Commands,"set");
  createNewThing(Commands,"get");
  createNewThing(Commands,"show");
  createNewThing(Commands,"help");

  // basic commands
  //  Commands["any"]->stuff = (void*)oneCommand;
  Commands["set"]->stuff = (void*)anyCommand;
  Commands["get"]->stuff = (void*)anyCommand;
  Commands["show"]->stuff = (void*)showCommand;
  Commands["help"]->stuff = (void*)helpCommand;

  // we have gpio(s)
  createNewThing("gpio");
  createNewThing("pwm_out");
  createNewThing("connection");



  // we have pwm_out (s)
  createNewThing("pwm_out");
  Things["gpio"]->createNewThing("dir");
  Things["gpio"]->createNewThing("pin");
  Things["gpio"]->createNewThing("value");

  Things["pwm_out"]->createNewThing("pin");
  Things["pwm_out"]->createNewThing("value");

  Things["connection"]->createNewThing("ip_address","127.0.0.1");
  Things["connection"]->createNewThing("node","3456");


  Things["gpio"]->addAction("set", (void*)setGpioFunction);
  Things["gpio"]->addAction("get", (void*)getGpioFunction);

  // this will connect with the selected node ans send the remainder of th command there
  Things["connection"]->addAction("set", (void*)setNodeFunction);
  Things["connection"]->addAction("get", (void*)getNodeFunction);

 
  // gpio_1 isa gpio
  isa("gpio_1", "gpio");
  // gpio_2 is a gpio
  isa("gpio_2", "gpio");
  // pwm_1 is a pwm_out
  isa("pwm_1", "pwm_out");

  isa("sysfoo", "connection");

  setValue(Things,"sysfoo","ip_address","127.0.0.1");
  setValue(Things, "sysfoo","node","5678");

  ListThings(Things,"  ");

  //runFunction (*Things["gpio"], "foo", "foo stuff");
  //runFunction (*Things["gpio"], "set", "set stuff");
  //runFunction (*Things["gpio_1"], "set", "set stuff");

  // some typical commands
  // set gpio_1/value/23
  // get gpio_1/value
  // make gpio_1 as a gpio
  // give gpio output as a string


#if 0
    cout << "code block 1:\n" << endl;
    Thing *thing1;
    thing1 = new Thing;
    thing1->vnum = 2400;

    map<int, Thing> rMap;
    map<int, Thing>::reverse_iterator iter;

    cout << "\ncode block 2:\n" << endl;
    rMap[thing1->vnum] = *thing1;

    for ( iter = rMap.rbegin() ; iter != rMap.rend(); iter++ )
    {
        cout<<iter->second.vnum<<endl;
    }

    cout << "\ncode block 3:\n" << endl;
    cout<<thing1->vnum<<endl;
    delete thing1;

#endif

    cout << "Press ENTER to show gpios." << flush;
    cin.ignore( numeric_limits <streamsize> ::max(), '\n' );
    Things["gpio"]->showList();

    cout << "Press ENTER to testr strings." << std::flush;
    cin.ignore( numeric_limits <streamsize> ::max(), '\n' );
    
    cout <<" string tests\n\n" ;
 
    doCommand("set  sysfoo/?node=2345&dir=output&pin=3&descr=power_switch");       // implied set
    doCommand("set  sysfoo/gpio_1/?value=1&dir=output&pin=3&descr=power_switch");  // implied set
    doCommand("get  sysfoo/gpio_1");                                               // implied get



    cout << "Press ENTER to quit." << std::flush;
    cin.ignore( numeric_limits <streamsize> ::max(), '\n' );
    return 0;
}





// create a thread to process a client
void *inputThread(void *data)
{
  sClient *sc = (sClient*)data;

  int rc=1;
  char buffer[2048];
  cout << " Input  thread  created:" << "\n";

  while (rc >0) {
    rc = RecvClient(sc->sock, buffer, sizeof buffer);
    if ( rc > 0 ) {
      rc = SendClient(sc->sock, (string)buffer);
    }
  }
  cout <<"client closed \n";
  close(sc->sock);  
  return NULL;
}



// create a thread to process a client
void input_client(int sock, void *data, struct sockaddr_in *client)
{
   int rc;
   sClient *sC = new sClient(sock);
   sC->client =  client;
   rc = pthread_create(&sC->thr, NULL, inputThread, (void *)sC);
   //Clients.push_back(j);
}

int main(int argc, char *argv[])
{

  if (( argc == 1 ) || ((string)argv[1] == "test"))
    {
      runTest();
    }

  if ((string)argv[1] == "server")
    {
      int port = 2345;
      cout <<" Running server on port ["<<port<<"]\n";
      socketServer(port, input_client, (void *)NULL);
    }

  if ((string)argv[1] == "one")
    {
      sClient sclient(1); // dummy for now

      string reply;
      string cmd="sysfoo/gpios/gpio_1?dir=output&pin=1";
      //Things["sysfoo"] = new Thing("sysfoo");
      //oneCommand(Things, sclient, cmd, reply, NULL);                // implied any command
      //cout << "\n\nListing things  "<<" size " << Things.size()<<" cmd ["<<cmd<<"]"<< endl;
      //ListThings(Things,"  ");

      cmd="sysfoo?isa=socket&ip_address=127.0.0.1&port=2234/gpios/gpio_1?dir=output&pin=1&value=0&isa=gpio";
      ///gpio_1?dir=output&pin=1/foo?name=foo";
      oneCommand(Things, sclient, cmd, reply, NULL);                // implied any command
      cout << "\n\nListing things  "<<" size " << Things.size()<<" cmd ["<<cmd<<"]"<< endl;
      ListThings(Things,"  ");

      cout << "Listing things  "<<" size " << Things.size()<<" "<< endl;
      //cout << "Listing things  "<< Things["sysfoo"]->name<<" size " << Things.size()<<" "<< endl;

      ListThings(Things,"  ");
      //ListKids(Things,"  ");
    }
  KillThings(Things);
  
  return 0;
}

