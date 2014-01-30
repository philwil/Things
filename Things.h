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

class Thing;

typedef map<string, Thing*> tMap;

class Thing
{
public:
  
  Thing() {cout<<"Default Constructor of " << this <<endl;}

 Thing(string name):name(name), fd(0), isa("") {
    cout<<"Name Constructor of ["<<name<<"] @" << this <<endl;
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
    cout<<"Destructor of " << this <<endl;
    tMap::iterator iter;
    for ( iter = Attrs.begin() ; iter != Attrs.end(); ++iter )
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
  int createNewThing(tMap &things, string name);
  int createNewThing(tMap &things, string name, string value);

  // add a function by name 
  int addAction(string name, void *stuff);

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
  Thing *parent;

  // a list of things I have
  tMap Attrs;
  tMap Actions;
  tMap Kids;
  
  vector<Thing *> myList;
};


#endif
