/******************************************
things take two
use operator overrides to make it work
******************************************/
#ifndef _T2__H
#define _T2__H

#include "Socket.h"
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

  T2* getKid(const string name)
  {
    return Kids[name];
  }

  void init() {parent=NULL; depth=0;linksock=0;};

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

  void addKid(const string name)
  {
    Kids[name] = new T2(name);
    Kids[name]->parent = this;
    Kids[name]->depth = this->depth+1;
  }

  void addKid(const string name, string attrs)
  {
    addKid(name);
    Kids[name]->SetAttrs(attrs);
  }

  void addAction(const string name, void *action)
  {
    Actions[name] = new T2(name);
    Actions[name]->parent = this;
    Actions[name]->depth = this->depth+1;
    Actions[name]->action = action;
  }

  void addAttr(const string name)
  {
    Attrs[name] = new T2(name);
    Attrs[name]->parent = this;
    Attrs[name]->depth = this->depth;
  }

  void RunAction(ostream &os, const string &name, void *data)
  {
    T2 *act;
    act = Actions[name];
    action_t action;
    if(act->action)
      {
	action=(action_t)act->action;
	action(os,this,data);
      }
  }

  void addAttr(const string name, const string value)
  {
    addAttr(name);
    Attrs[name]->value = value;
    valChanged = true;
  }

  void SetAttr(string &sattrs);
  void SetAttrs(string &sattrs);

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
