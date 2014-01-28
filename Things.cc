// things.cc lets see how this worksout
// solve the isa problem
// gpio_i isa gpio
// it must deep copy all the stuff that gpio uses
// update gpio and all gpios get the update.
//

#include <iostream>
#include <limits>
#include <map>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

#include "Things.h"



int Thing::showList()
{

  cout<<"["<<name << "] List contains:\n";
  for (vector<Thing *>::iterator it = myList.begin() ; it != myList.end(); ++it)
    cout << ' ' << (*it)->name<<"\n";
  cout << '\n';
}


int Thing::ListAttrs()
{
  map<string, Thing*>::iterator iter;
  for ( iter = attrs.begin() ; iter != attrs.end(); ++iter )
    {
      cout<<iter->first<<endl;
    }
  
  return 0;
}
void Thing::Split(vector<string>& lst, const string& input, const string& separators, bool remove_empty)
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
 
int Thing::createNewThing(map<string, Thing*> &things, string name)
{
  if (!things[name]) 
    {
      things[name]= new Thing;
      cout <<"new Thing [" << name <<"] created" <<"\n";
      
    }
  else
    {
      cout <<"Thing" << name <<" Already created" <<"\n";
    }
  return 0;
}

int Thing::createNewThing(map<string, Thing*> &things, string name, string value)
{
  createNewThing(things,name);
  things[name]->value = value;
  return 0;
}


int Thing::createNewThing(string name)
{
  return createNewThing(attrs, name);
}

int Thing::createNewThing(string name, string isa)
{
  cout << " New this isa .. Not yet set up"<<endl;

  return createNewThing(attrs, name);
}

int Thing::addFunction(string name, void *stuff)
{
  if (!myFunctions[name]) {
    myFunctions[name]=new Thing;
  }
  myFunctions[name]->stuff = stuff;
}


int Thing::setAttrs(string &reply, string stuff)
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
      
      if (!attrs[subs[0]]) 
	{
	  cout << " No attribute ["<<subs[0]<<"] in ["<< name <<"]\n";
	  //thing.attrs[subs[0]]=new Thing(subs[0]);
	  //(thing.attrs[subs[0]])->value = subs[1];
	}
      else
	{
	  cout << " Set value of  ["<<subs[0]<<"] in ["<< name <<"] to value ["<< subs[1]<<"]\n";
	  (attrs[subs[0]])->value = subs[1];
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

int Thing::getAttrs(string &reply, string stuff)
{
    map <string, Thing *>::iterator it;
    reply="{ ";
    bool s1 = true;
    for (it = attrs.begin(); it != attrs.end(); ++it) 
    {
	if((*it).second) 
	{
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
