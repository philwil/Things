//
// copyright 2014
//    Phil Wilshire <sysdcs@gmail.com>
// Released under the GPL V2
//
// Things have attiibutes
//   These can be set or queried by 

//  simple urls
//    /some_io/some_gpio?value
//    /some_io/some_gpio?value=1
//
// we'll allow attributes to be set up on an adhoc basis.
// If a thing has one or more isa features then the reading / writing the attributes 
// will have a function associated with it
//   the isa will cause the thing to join a class of things
//
//     /some_io/some_gpio?isa=gpio
//
//   If nothing else, in this case, mything will have an attribute called some_gpio
//   this means we can collect things together under some_io.
//
// A thing can be a connection or simply a collection.
//   Lets look at a connection
//     /my_new_pi can define a connection or a system
//    
//      /my_new_pi?isa=connection?ip_address=10.0.30.110&node=3456
//
// the command  /my_new_pi?action=run 
//   will actually start the /my_new_pi server
//   other systems can talk to it 
//   for example /my_desktop can talk to it
//       /my_new_pi/some_io/some_gpio?value
//       /my_new_pi/some_io/some_gpio?value=1
//
// Messy code at the moment but well get it worked oput.
// 
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


// list my attributtes
//  /some_gpio?action=list_attrs (TODO)

int Thing::ListAttrs()
{
  map<string, Thing*>::iterator iter;
  for ( iter = Attrs.begin() ; iter != Attrs.end(); ++iter )
    {
      cout<<iter->first<<endl;
    }
  return 0;
}

// split a string by delimiters
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

// create a new thing in a Thing map 
int Thing::createNewThing(tMap &things, string name)
{
  // warning this will create th thing in tMap
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


int Thing::createNewThing(tMap &things, string name, string value)
{
  createNewThing(things,name);
  things[name]->value = value;
  return 0;
}


int Thing::createNewThing(string name)
{
  return createNewThing(Attrs, name);
}

int Thing::createNewThing(string name, string isa)
{
  cout << " New this isa .. Not yet set up"<<endl;

  return createNewThing(Attrs, name);
}

int Thing::addAction(string name, void *stuff)
{
  if (!Actions[name]) {
    Actions[name]=new Thing;
  }
  Actions[name]->stuff = stuff;
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
      
      if (!Attrs[subs[0]]) 
	{
	  cout << " No attribute ["<<subs[0]<<"] in ["<< name <<"]\n";
	  //thing.attrs[subs[0]]=new Thing(subs[0]);
	  //(thing.attrs[subs[0]])->value = subs[1];
	}
      else
	{
	  cout << " Set value of  ["<<subs[0]<<"] in ["<< name <<"] to value ["<< subs[1]<<"]\n";
	  (Attrs[subs[0]])->value = subs[1];
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


int Thing::addAttrs(string &reply, string stuff)
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
      
      if (!Attrs[subs[0]]) 
	{
	  cout << " Adding attribute ["<<subs[0]<<"] in ["<< name <<"]\n";
	  Attrs[subs[0]]=new Thing(subs[0]);
	  if (s1)
	    {
	      s1 = false;
	    } else {
	    reply += ",";
	  }

	  if (subs.size() > 1){
	    (Attrs[subs[0]])->value = subs[1];
	    reply += "\"" +subs[0] + "\":\"" + subs[1] + "\"";
	  } else {
	    reply += "\"" +subs[0] + "\", ";
	  }
	}
      else
	{
	  cout << " Set value of  ["<<subs[0]<<"] in ["<< name <<"] to value ["<< subs[1]<<"]\n";
	  (Attrs[subs[0]])->value = subs[1];
	  if (s1) 
	    {
	      s1 = false;
	    } else {
	    reply += ",";
	  }
	  reply += "\"" +subs[0] + "\":\"" + subs[1] + "\"";
	}	  
    }
  reply += "}";
  cout << " Add Attrs reply [" << reply <<"]\n";

  return 0;
}

int Thing::getAttrs(string &reply, string stuff)
{
    tMap::iterator it;
    reply="{ ";
    bool s1 = true;
    for (it = Attrs.begin(); it != Attrs.end(); ++it) 
    {
	if((*it).second) 
	{
	    cout <<" getting ["<<(*it).first<<"]\n";
	  if (s1)
	    {
	      s1 = false;
	    } else {
	    reply += ",";
	  }
	  reply += "\"" +(*it).first + "\":\"" + (*it).second->value + "\"";
	}
    }
    reply+="}";
    return 0;
}
