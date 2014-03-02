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
#include <unistd.h>

#include "Socket.h"
using namespace std;

class T2;
typedef map<string, T2*> tMap;
typedef  int (*action_t)(ostream &os, T2 *t2, void *data);

class T2
{
public:
 
  typedef struct t2Server 
  {
    int sock;
    pthread_t thr;
    T2 * t2;

  } t2Server;


  T2() 
  {
    cout<<"Default Constructor " << this <<endl;
    init();
  };
  
  T2(string name):name(name),value("none")
  {
    //    T2();
    cout<<"Name Constructor ["<<name<<"] @" << this <<endl;
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


  void init() {parent=NULL; depth=0;linksock=0;};


  T2 *getMap(tMap &tmap, const string &name)
  {
    if (!tmap[name])
      {
	tmap[name] = new T2(name);
      }
    return tmap[name];
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
    t2->parent = this;
    t2->depth = this->depth+1;
  }

  void AddKida(const string &kname, const string &attrs)
  {
    AddKid(kname);
    Kids[kname]->SetAttrs(attrs);
  }

  void AddAction(const string &name, void *action)
  {
    T2 *t2 = getMap(Actions, name); 
    t2->parent = this;
    t2->depth = this->depth+1;
    t2->action = action;
  }

  T2 *AddAction(const string &name)
  {
    T2 *t2 = getMap(Actions, name); 
    t2->parent = this;
    t2->depth = this->depth+1;
    return t2;
  }


  void addAttr(const string &name)
  {
    T2 *t2 = getMap(Attrs, name); 
    t2->parent = this;
    t2->depth = this->depth;
  }

  void RunAction(ostream &os, const string &name, void *data)
  {
    T2 *act;
    act = getMap(Actions, name); ;
    action_t action;
    if(act->action)
      {
	action=(action_t)act->action;
	action(os,this,data);
      }
  }

  void addAttr(const string &name, const string value)
  {
    addAttr(name);
    Attrs[name]->value = value;
    valChanged = true;
  }

  int addLib(ostream &os, string &name)
  {
    string dlname = "./libt2"+name+".so";
    void * handle;
    action_t setup;
    T2 * t2 = AddAction(name);

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


  void SetAttr(const string &sattrs);
  void SetAttrs(const string &sattrs);

  friend T2* operator<<(T2* t2, const string &name);
  friend ostream& operator<<(ostream& os, T2* t2);
  void Show(ostream& os);

  //sClient * client;
  // TODO put these in a special structure

  int RunServer(string &port);
  int SetLink(string &addr, string &port);
  int linksock;
  void *action;

  string name;
  string value;
  T2 *parent;
  int depth;
  bool valChanged;
  tMap Attrs;
  tMap Kids;
  tMap Actions;

};

int SplitString(string &name, string &attrs, string&remains, string &src);
#endif
