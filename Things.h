#ifndef _THINGS_H
#define THINGS_H
// things.cc lets see how this worksout
// solve the isa problem
// gpio_i isa gpio
// it must deep copy all the stuff that gpio uses
// update gpio and all gpios get the update.
//

#include <vector>
#include <string>

class Thing
{
public:
  
  Thing() {cout<<"Default Constructor of " << this <<endl;}

 Thing(string name):name(name), fd(0), isa("") {
    cout<<"Name Constructor of " << this <<endl;
  }
  
  Thing(const Thing & rd)
  {
    cout << "Copy Constructor of "<< this << " from " << &rd << endl;
    name=rd.name;
    vnum=rd.vnum;
  }
  
  Thing & operator=(const Thing & rd)
  {
    cout << this << '=' << &rd << endl;
    name=rd.name;
    vnum=rd.vnum;
  }
  
  ~Thing() {
    cout<<"Destructor of" << this <<endl;
    map<string, Thing*>::iterator iter;
    for ( iter = attrs.begin() ; iter != attrs.end(); ++iter )
      {
	delete iter->second;
      }
  }
  
  int ListAttrs();

  // give me something on my list of things
  int createNewThing(string name);

  // give me something as something on my list of things
  int createNewThing(string name, string isa);

  // give me something on some other list
  int createNewThing(map<string, Thing*> &things, string name);
  int createNewThing(map<string, Thing*> &things, string name, string value);

  // add a function by name 
  int addFunction(string name, void *stuff);

  // show a list of things connected with me
  int showList();
  void Split(vector<string>& lst, const string& input, const string& separators, bool remove_empty);
  int setAttrs(string &reply, string stuff);
  int getAttrs(string &reply, string stuff);

  // socket connection
  int fd;

  // my name
  string name;

  string value;

  int vnum;
  string type;
  void *stuff;
  // what am i .. this can be blank
  string isa;
  Thing *isaThing;
  // a list of things I have
  map<string, Thing*> attrs;
  map<string, Thing*> myFunctions;

  vector<Thing *> myList;
};

#endif
