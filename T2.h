/******************************************
things take two
use operator overrides to make it work
******************************************/
#ifndef _T2__H
#define _T2__H

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
#include <stdio.h>

using namespace std;

void myDie(const char *msg);


class T2;
typedef map<string, T2*> tMap;
typedef  int (*action_t)(ostream &os, T2 *t2, void *data);


extern tMap gActions;


class T2
{
public:
 

  T2() 
  {
    cout<<"Default Constructor " << this <<endl;
    init();
  };
  
  T2(string name):name(name),value("none")
  {
    //    T2();
    cout<<"Name Constructor["<<name<<"] @" << this->name <<endl;
    init();
  }

  ~T2()
  {
    cout<<"Default Destructor " << name <<endl;
    tMap::iterator iter;
    for (iter=Attrs.begin(); iter != Attrs.end(); ++iter)
      {
	delete Attrs[iter->first];
      }
    for (iter=Kids.begin(); iter != Kids.end(); ++iter)
      {
	delete Kids[iter->first];
      }
  }


  void init() {
    parent=NULL; 
    depth=0;
    t2_type=NULL;
    action = NULL;
    t2Sock = NULL;
  };



  T2 *getMap(tMap &tmap, const string &name, bool create)
  {
    if (!create)
      {
	tMap::iterator iter;
	iter=tmap.find(name);
	if (iter != tmap.end())
	  return (iter->second);
	else
	  return NULL;
      }
    else
      { 
	if (!tmap[name])
	  {
	    tmap[name] = new T2(name);
	    T2 *t2 = tmap[name];
	    t2->parent = this;
	    t2->depth = this->depth+1;
	  }
	return tmap[name];
      }
  }


  T2 *getMap(tMap &tmap, const string &name)
  {
    return getMap(tmap, name, true);
  }


  T2* getKid(const string &name)
  {
    return getMap(Kids, name);
  }


  void setIndent(ostream& os, int extra)
  {
    int idx = depth+1;
    while (--idx) {os << "  ";} 
    while (extra--){os << " ";} 
  }

  void setIndent(ostream& os)
  {
    setIndent(os, 0);
  }

  void AddKid(const string &kname)
  {
    T2 *t2 = getMap(Kids, kname); 

  }

  void AddKid(const string &kname, const string &attrs)
  {
    AddKid(kname);
    Kids[kname]->SetAttrs(attrs);
  }

  void AddAction(const string &aname, void *action, bool gbl=false);

  T2 *AddAction(const string &name)
  {
    T2 *t2 = getMap(Actions, name); 
    return t2;
  }


  void addAttr(const string &name)
  {
    T2 *t2 = getMap(Attrs, name); 
    t2->depth = this->depth;
  }

  int copyAttrs(ostream &os, T2 *t2_t)
  {
    int rc = 0;
    tMap::iterator iter;
    for (iter=t2_t->Attrs.begin(); iter != t2_t->Attrs.end(); ++iter)
      {
	addAttr(iter->first,(iter->second)->value);
	++rc;
      }
    return rc;
  }


  void RunAction(ostream &os, const string &aname, void *data);

  int addLib(ostream&os, const string&slib, void *data);

  void addAttr(const string &name, const string value)
  {
    addAttr(name);
    Attrs[name]->value = value;
    valChanged = true;
  }
#if 0
  int addFcn(ostream &os, string &name)
  {
    string dname = name;
    if (dname[0] == '@')
      dname.erase(0,1);
    
    
    string dlname = "./libt2"+dname+".so";
    cout <<"dlname =["<<dname<<"]\n";
    void * handle;
    action_t setup;
    T2 * t2 = AddAction(dname);

    handle =  dlopen(dlname.c_str(), RTLD_NOW);
    if (!handle) {
      os << "dlopen "<< (char *)dlerror()<<"\n";
      return -1;
    }

    dlerror();  /* clear any current error */
    setup = (action_t)dlsym(handle, "setup");
    char *error = (char *)dlerror();
    if (error != NULL) {
      os << "dlsym error \n"<<(char *)dlerror()<<"\n";
      return -1;
    }
    int ret = setup(os, t2, NULL);
    return ret;
  }
#endif

  int ServiceInput(ostream &os, const string&insrc, void *data);



  void SetAttrs(const string &sattrs);

  friend T2* operator<<(T2* t2, const string &name);
  friend ostream& operator<<(ostream& os, T2* t2);
  void Show(ostream& os);
  void jShow(ostream& os);

  //sClient * client;
  // TODO put these in a special structure

  //  int linksock;
  void *action;

  string name;
  string value;
  T2 *parent;
  int depth;
  bool valChanged;
  tMap Attrs;
  tMap Kids;
  tMap Actions;  // if we have local actions
  T2 *t2_type;     // type for generic type actions
  void *t2Sock;
  void SetAttr(const string &sattrs); 
  int SetLib(const string inname);

 private:
  int foo;

};

int SplitString(string &name, string &attrs, string&remains, string &src);
#endif
