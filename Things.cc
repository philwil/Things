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

char *Thing::setAttrs(string stuff)
{
  ostringstream ocout;

  vector <string> args;
  vector <string>::iterator it;
  Split(args, stuff, "?&", true);
  ocout<<"{ ";
  bool s1 = true;
  for (it = args.begin(); it != args.end(); ++it) 
    {
      ocout << *it << endl;
      
      vector <string> subs;
      Split(subs, *it, "=", true);
      
      if (!Attrs[subs[0]]) 
	{
	  ocout << " Adding attribute ["<<subs[0]<<"] in ["<< name <<"]\n";
	  Attrs[subs[0]]=new Thing(subs[0]);
	  (Attrs[subs[0]])->value = subs[1];
	}
      else
	{
	  ocout << " Set value of  ["<<subs[0]<<"] in ["<< name <<"] to value ["<< subs[1]<<"]\n";
	  (Attrs[subs[0]])->value = subs[1];
	  if (s1) 
	    {
	      s1 = false;
	      ocout <<"\""<<subs[0]<<"\"=\""<< subs[1]<<"\"";
	    }
	  else
	    {
	      ocout <<",\"" <<subs[0] << "\"=\""<<subs[1]<< "\"";
	    }
	}	  
    }
  ocout << "}";
  return (char *)(ocout.str()).c_str();
}

// a basic command parser
// Start with parsing the command split it up into words
//
char *Thing::docmd(tMap&things, string &cmd)
{
  ostringstream ocout;
  vector <string> myCmds;
  Split(myCmds, cmd," ", true);
  // for now just run commands from a basic decoder
  if ((myCmds[0] == "name") && (myCmds.size() > 1))
    {
      name=myCmds[1];
      ocout << " Set new name to " << name <<" \n";
    }
  else if ((myCmds[0] == "set") && (myCmds.size() > 1))
    {
      ocout << setAttrs(myCmds[1]);
    }
  else if ( myCmds[0]=="list" )
    {
      string ss="  ";
      ocout << ListThings(things, ss);
    }
  else
    {
      ocout <<"Hello from me [" << name <<"] cmd was ["<<cmd<<"] \n\n";
    }
  return (char *)(ocout.str()).c_str();
}

int Thing::doIsa(tMap &Isas, Thing &myIsa)
{
  cout <<" \n\ndoing Isa for ["<< myIsa.value<<"]\n";


  if (!Isas[myIsa.value]) 
  {
      cout << " Creating Isa called ["<<myIsa.value<<"] \n"; 
      Isas[myIsa.value]= new Thing;
  }
  else
  {
    cout << " Found Isa called ["<<myIsa.value<<"] \n"; 
  }

  // add this things to list of other things that are also isas 
  if (!Isas[myIsa.value]->IsaList[this])
    {
      Isas[myIsa.value]->IsaList[this] = this;
    }

  cout <<" \n\ndone Isa for ["<< myIsa.value<<"]\n";
  isaThing = Isas[myIsa.value];

  // now copy all current attributes to the isa, if needed
  tMap::iterator iter;
  tIMap::iterator iIter;
  for (iter = Attrs.begin(); iter != Attrs.end(); ++iter )
    {
      if (iter->first=="isa") continue;
      if(!isaThing->Attrs[iter->first])
	{
	  cout << " Creating  Isa attribute ["<<iter->first<<"] from this ["<< this <<"]\n";
	  isaThing->Attrs[iter->first] = new Thing(iter->first);
	  cout << " Sending  Isa attribute ["<<iter->first<<"] to all others that need it \n";
	  for (iIter = Isas[myIsa.value]->IsaList.begin(); iIter != Isas[myIsa.value]->IsaList.end(); ++iIter )
	    {
	      Thing * iThing = iIter->first;
	      if (!iThing->Attrs[iter->first])
		{
		  iThing->Attrs[iter->first]=new Thing(iter->first);
		}
	    }
	} 
    }

  for (iter = isaThing->Attrs.begin(); iter != isaThing->Attrs.end(); ++iter )
    {
      if(!Attrs[iter->first])
	{
	  cout << " Transferring attribute ["<<iter->first<<"] from ISA to This["<<this<<"]\n";
	  Attrs[iter->first] = new Thing(iter->first);
	} 
    }
  // also need to add this entity to the list of users of the isa if it is not already there 
  // what about the kids of the attributes .. not yet


  return 0;
#if 0
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
#endif

}

//
// Add / create attribures
// stuff contains 
// ?pin=1&dir=output&value=1
// or
//  ?pin&dir&value
// watchout for isa=something
// in the case 
// this has to be processed at the top level
// 
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
	    (Attrs[subs[0]])->parent = this;
	}
	
	if (subs.size() > 1)
	{
	    (Attrs[subs[0]])->value = subs[1];
	    reply += "\"" +subs[0] + "\":\"" + subs[1] + "\"";
	} else {
	  reply += "\"" +subs[0] + "\", ";
	}
    }
    reply += "}";
    if(debug)cout << " Add Attrs reply [" << reply <<"]\n";
    
    return 0;
}

// given an arbitrary list of things, find a named thing
Thing *Thing::findThing(tMap &things, string name)
{
  tMap::iterator it;
  cout << "findThing ["<< name <<"]\n";

  for (it = things.begin(); it != things.end(); ++it) 
    {
      cout << "looking for ["<< name <<"] found ["<<(*it).first<<"]\n";
      if((*it).first == name)
	return (*it).second;
    }
  return NULL;
}

//get a list of attributes and values
int Thing::getAttrs(string &reply, string stuff)
{
    tMap::iterator it;
    reply="{ ";
    bool s1 = true;
    for (it = Attrs.begin(); it != Attrs.end(); ++it) 
    {
	if((*it).second) 
	{
	  //cout <<" getting ["<<(*it).first<<"]\n";
	    if (s1)
	    {
	        s1 = false;
	    } 
	    else 
	    {
	        reply += ",";
	    }
	    reply += "\"" +(*it).first + "\":\"" + (*it).second->value + "\"";
	}
    }
    reply+="}";
    return 0;
}

char *Thing::ListThings(tMap &things, string dummy)
{
  ostringstream ocout;

  tMap::iterator iter;
  for ( iter = things.begin() ; iter != things.end(); ++iter )
  {
    if (iter->second->isa.empty()) {
      ocout<<dummy<<iter->first<< ": [" <<(iter->second)->value<<"]"<<endl;
    } else {
      ocout<<dummy<<iter->first<< " isa [" << (iter->second)->isa <<"]" <<endl;
    }
    ocout << ListThings(iter->second->Attrs, dummy + "{attr}   ");
    ocout << ListThings(iter->second->Kids, dummy + "   ");
  }
  return (char *)(ocout.str()).c_str();
}
