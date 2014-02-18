#ifndef _THINGS_H
#define _THINGS_H
// things.cc lets see how this work sout
// solve the isa problem
// gpio_i isa gpio
// it must deep copy all the stuff that gpio uses
// update gpio and all gpios get the update.
//
// this is all in development mode so we'll have several options that may no longer be used while we work out
// the final form of all this.
//
#include <vector>
#include <string>

class Thing;

typedef map<string, Thing*> tMap;
typedef map<Thing*, Thing*> tIMap;

class Thing
{
public:
  
  Thing() 
  {
      cout<<"Default Constructor of " << this <<endl;
  }

 Thing(string name):name(name), fd(0), isa("") , value("none") , isaThing(NULL)
  {
      cout<<"Name Constructor of ["<<name<<"] @" << this <<endl;
  }

  // we "should" copy all attributes and all kids
  
  Thing(const Thing &rd)
  {
      cout << "Copy Constructor of "<< this << " from " << &rd << endl;
      name=rd.name;
      value=rd.value;
      tMap::iterator iter;
      tMap rdattrs = rd.Attrs;
      for ( iter = rdattrs.begin() ; iter != rdattrs.end(); ++iter )
      {
      	  Attrs[iter->first] = new Thing(*iter->second);
	  Attrs[iter->first]->parent=this;
      }
      tMap rdkids = rd.Kids;
      for ( iter = rdkids.begin() ; iter != rdkids.end(); ++iter )
      {
      	  Kids[iter->first] = new Thing(*iter->second);
	  Kids[iter->first]->parent=this;
      }
  }
  
  Thing & operator=(const Thing & rd)
  {
      cout << this << '=' << &rd << endl;
      name=rd.name;
      value=rd.value;
  }
  
  // destructor
  ~Thing() 
  {
      cout<<"Destructor of " << this->name  <<endl;
      tMap::iterator iter;
      for ( iter = Attrs.begin() ; iter != Attrs.end(); ++iter )
      {
	  delete iter->second;
      }
  }
  
  Thing *findThing(tMap &things, string name);
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
  int addAttrs(string &reply, string stuff);
  //int setAttrs(string &reply, string stuff);
  int getAttrs(string &reply, string stuff);
  int doIsa(tMap &isas, Thing &myIsa);

  char *ListIsas(tMap&things, ostringstream &ocout, string &isaName);
  char *ListIsaItems(tMap&things, ostringstream &ocout, string &isaName);
  char *ListThings(tMap &things, ostringstream &ocout,string dummy);
  char *setAttrs(ostringstream &ocout,string stuff);
  char *doIsa(ostringstream &ocout,string stuff);

  char *doCMD(tMap&things, ostringstream &ocout,string &cmd);
  char *readCMD(tMap&things, ostringstream &ocout,string &cmd);
  char *setCMD(tMap&things, ostringstream &ocout,string &cmd);

  bool isaBrother(string &cmd);

  char *isaCMD(tMap&things, ostringstream &ocout, string &isaName, string &cmd);
  bool setIsa(ostringstream &ocout, Thing *isaThing);
  bool findNextCmd(string &finder, ostringstream &ocout, string &cmd);
  bool findFirstThing(string &finder, ostringstream &ocout, string &cmd);
  Thing *findThing(tMap&things, ostringstream &ocout, string &cmd);
  Thing *findIsa(tMap&things, ostringstream &ocout, string &isaName);

  Thing *makeThing(tMap&things, ostringstream &ocout, string &tname);

  int getCmdThing(string &result, string &cmd);
  int getCmdAttrs(string &result, string &cmd);
  // socket connection
  int sock;
  int fd;

  // my name
  string name;

  // my value
  string value;
  bool valueChanged;


  string type;
  void *stuff;

  // what am i .. this can be blank
  // this may also have to be a list
  string isa;
  Thing * isaThing;

  /// hi Mom, Dad
  Thing *parent;


  // pointer to global ISAs
  tMap *isap;
  // a list of things I have
  tMap Attrs;
  tMap Actions;
  tMap Kids;
  // list of nodes that are also isas
  tIMap IsaList;
  // this is a list of others like me
  vector<Thing *> myList;

  //debug
  int debug;
};


#endif
