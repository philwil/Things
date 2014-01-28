// things.cc lets see how this worksout
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

using namespace std;

#include "Things.h"

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
map<string, Thing*> Things;
map<string, Thing*> Commands;


int createNewThing(map<string, Thing*> &things, string name)
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


int ListThings(map<string, Thing*> &things, string dummy)
{
  map<string, Thing*>::iterator iter;
  for ( iter = things.begin() ; iter != things.end(); ++iter )
  {
    if (iter->second->isa.empty()) {
      cout<<dummy<<iter->first<<endl;
    } else {
      cout<<dummy<<iter->first<< " isa [" << (iter->second)->isa <<"]" <<endl;
    }
    ListThings(iter->second->attrs, dummy + "   ");
  }

  return 0;
}

int KillThings(void)
{
  map<string, Thing*>::iterator iter;
  for ( iter = Things.begin() ; iter != Things.end(); ++iter )
  {
    cout << "killing ["<< (iter->second)->name <<"]" << endl;
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

int setAttrs(Thing &thing, string &reply, string stuff)
{
  vector <string> args;
  vector <string>::iterator it;
  Split(args, stuff, "?&", true);
  reply="{ ";
  bool s1 = true;
  for (it = args.begin(); it != args.end(); ++it) 
    {
      cout << *it << endl;
      
      vector <string> subs;
      Split(subs, *it, "=", true);
      
      if (!thing.attrs[subs[0]]) 
	{
	  cout << " No attribute ["<<subs[0]<<"] in ["<< thing.name <<"]\n";
	  //thing.attrs[subs[0]]=new Thing(subs[0]);
	  //(thing.attrs[subs[0]])->value = subs[1];
	}
      else
	{
	  cout << " Set value of  ["<<subs[0]<<"] in ["<< thing.name <<"] to value ["<< subs[1]<<"]\n";
	  (thing.attrs[subs[0]])->value = subs[1];
	  if (s1) 
	    {
	      s1 = false;
	      reply += "\"" +subs[0] + "\"=\"" + subs[1] + "\"";
	    }
	  else
	    {
	      reply += ",\"" +subs[0] + "\"=\"" + subs[1] + "\"";
	    }
	  
	}	  
    }
  reply += "}";
  
  return 0;
}

int getAttrs(Thing &thing, string &reply, string stuff)
{
  map <string, Thing *>::iterator it;
  reply="{ ";
  bool s1 = true;
  for (it = thing.attrs.begin(); it != thing.attrs.end(); ++it) 
    {
      if((*it).second) {
	cout <<" getting ["<<(*it).first<<"]\n";
	if (s1)
	  {
	    s1 = false;
	    
	    reply += "\"" +(*it).first + "\"=\"" + (*it).second->value + "\"";
	  }
	else
	  {
	    reply += ",\"" +(*it).first + "\"=\"" + (*it).second->value + "\"";
	    
	  }
      }
    }
  reply+="}";
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
  return setAttrs(thing, reply, stuff);
}

int getGpioFunction (Thing &thing, string &reply, string name, string stuff)
{
  // cout << " running get gpio function [" << endl;
  //return 0; 
  cout << " running get gpio function [" << name << "] in Thing [" 
       << thing.name 
       << "] with [" << stuff<< "]"<< endl;
  return getAttrs(thing, reply, stuff);

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
    return setAttrs(thing, reply, stuff);
  }

  if (thing.fd == 0) {
    if(!thing.attrs["ip_address"]) {
      cout<< " no ip_address\n"<<endl;
      return -1;
    }
    if(!thing.attrs["node"]) {
      cout<< " no node\n"<<endl;
      return -1;
    }

    thing.fd = sock_connect(thing.attrs["ip_address"]->value, thing.attrs["node"]->value, 2000);
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
    return getAttrs(thing, reply, stuff);
  }

  if (thing.fd == 0) {
    if(!thing.attrs["ip_address"]) {
      cout<< " no ip_address\n"<<endl;
      return -1;
    }
    if(!thing.attrs["node"]) {
      cout<< " no node\n"<<endl;
      return -1;
    }

    thing.fd = sock_connect(thing.attrs["ip_address"]->value, thing.attrs["node"]->value, 2000);
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
  map<string, Thing*> myFunctions;
  myFunctions = thing.myFunctions;

  if (thing.isa.empty())
    {
      cout<<"Thing ["<< thing.name<<"]" <<" isa is empty\n";
    } 
  else 
    {
      cout<<"Thing ["<< thing.name<<"]" <<" isa is <" << thing.isa<<">.. NOT empty\n";
      myFunctions = thing.isaThing->myFunctions;
    }

  if ( !myFunctions[name] ) 
    {
      cout << " no function [" << name << "] in Thing called [" 
	   << thing.name << "]"<< endl;
    }
  else
    {
      cout << " run function [" << name << "] in Thing called [" << thing.name  
	   << "] with [" << stuff<<"]"<< endl;
      int (*runFunction) (Thing &thing, string &reply, string name, string stuff) =
	(int (*) (Thing &, string &, string, string))myFunctions[name]->stuff;
      runFunction(thing, reply, name, stuff);
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
  map<string, Thing*>::iterator iter;
  for (iter = isa->attrs.begin(); iter != isa->attrs.end(); ++iter )
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


//
// sends any  command via a node if needs be
//
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
    
    if(!targ->myFunctions[stuff[0]]) 
      {
	cout << " no  command ["<< stuff[0] << "] in myFunctions ON["<< targ->name<<"]" << endl;
      }
    else 
      {
	cout << " YES  command ["<< stuff[0] << "] in myFunctions" << endl;
	runFunction(*targ, reply, stuff[0], nstr);
      }
    if(!isa->myFunctions[stuff[0]]) 
      {
	cout << " no  ISA command ["<< stuff[0] << "] in myFunctions ON["<< isa->name<<"]" << endl;
      }
    else 
      {
	
	cout << " YES  command ["<< stuff[0] << "] in ISA myFunctions" << endl;
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
    
    if(!targ->myFunctions[stuff[0]]) 
      {
	cout << " no  command ["<< stuff[0] << "] in myFunctions ON["<< targ->name<<"]" << endl;
      }
    else 
      {
	cout << " YES  command ["<< stuff[0] << "] in myFunctions" << endl;
	runFunction(*targ, reply, stuff[0], nstr);
      }
    if(!isa->myFunctions[stuff[0]]) 
      {
	cout << " no  ISA command ["<< stuff[0] << "] in myFunctions ON["<< isa->name<<"]" << endl;
      }
    else 
      {
	
	cout << " YES  command ["<< stuff[0] << "] in ISA myFunctions" << endl;
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
  if(!things[name]->attrs[object])
    {
      cout << " SetValue thing "<<name<<" no object " << object<<"] found \n";
      return -1;
    } 
  things[name]->attrs[object]->value = value;
  
  return 0;
}

int doCommand(string cmd) 
{
  vector <string> myStrings;
  Split(myStrings, cmd," ", true);
  
  //anyCommand
  vector <string>::iterator it;
  for (it = myStrings.begin(); it != myStrings.end(); ++it) 
    {
      cout << *it << endl;
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

int main(int argc, char *argv[])
{


  createNewThing(Things,"sysfoo");

  createNewThing(Commands,"set");
  createNewThing(Commands,"get");
  createNewThing(Commands,"show");
  createNewThing(Commands,"help");

  // basic commands
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


  Things["gpio"]->addFunction("set", (void*)setGpioFunction);
  Things["gpio"]->addFunction("get", (void*)getGpioFunction);

  // this will connect with the selected node ans send the remainder of th command there
  Things["connection"]->addFunction("set", (void*)setNodeFunction);
  Things["connection"]->addFunction("get", (void*)getNodeFunction);

 
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
 
    doCommand("set  sysfoo/?node=2345&dir=output&pin=3&descr=\"power_switch\"");
    doCommand("set  sysfoo/gpio_1/?value=1&dir=output&pin=3&descr=\"power_switch\"");
    doCommand("get  sysfoo/gpio_1");


    cout << "Press ENTER to quit." << std::flush;
    cin.ignore( numeric_limits <streamsize> ::max(), '\n' );
    KillThings();

    return 0;
}

