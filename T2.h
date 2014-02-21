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

  void init() {parent=NULL; depth=0;};

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

  void addAttr(const string name)
  {
    Attrs[name] = new T2(name);
    Attrs[name]->parent = this;
    Attrs[name]->depth = this->depth;
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

  string name;
  string value;
  T2 *parent;
  int depth;
  bool valChanged;
  tMap Attrs;
  tMap Kids;

};

int SplitString(string &name, string &attrs, string&remains, string &src);
#endif
