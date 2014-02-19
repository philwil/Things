/******************************************
things take two
use operator overrides to make it work
******************************************/
#ifndef _T2__H
#define _T2__H


class T2;
typedef map<string, T2*> tMap;


class T2
{
public:
  
  T2() 
  {
    cout<<"Default Constructor " << this <<endl;
    parent=NULL;
    depth=0;
  };
  
  T2(string name):name(name),value("none")
  {
    cout<<"Name Constructor ["<<name<<"] @" << this <<endl;
    parent=NULL;
    depth=0;
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

  void setIndent(ostream& os)
  {
    int idx = depth+1;
    while (--idx) {os << "  ";} 
  }
  void addKid(const string name)
  {
    Kids[name] = new T2(name);
    Kids[name]->parent = this;
    Kids[name]->depth = this->depth+1;
  }
  void addAttr(const string name, const string value)
  {
    Attrs[name] = new T2(name);
    Attrs[name]->value = value;
    Attrs[name]->parent = this;
    Attrs[name]->depth = this->depth+2;
  }

  friend T2* operator<<(T2* t2, const string &name);
  friend ostream& operator<<(ostream& os, T2* t2);
  void Show(ostream& os, string dummy);

  string name;
  string value;
  T2 *parent;
  int depth;
  tMap Attrs;
  tMap Kids;

};


#endif
