/********************************************
Things try two...
**********************************************/

/*********************************************
 mything <<"some/stuff/etc"
creates a thing

also
 mything <<"some/stuff?attribs=one&attrs=2/etc"

cout << mything  to get a listing
***********************************************/

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

#include "T2.h"
extern tMap Kids;


void T2::Show(ostream &os)
{

    setIndent(os); os<< name <<endl;
    tMap::iterator iter;
    for (iter=Attrs.begin(); iter != Attrs.end(); ++iter)
      {
	Attrs[iter->first]->setIndent(os);
	os << iter->first
	   << " ["<<Attrs[iter->first]->value<<"]"<<endl;
      }
    for (iter=Kids.begin(); iter != Kids.end(); ++iter)
      {
	Kids[iter->first]->Show(os);;
      }

}



// this is the main object loader
// /gpios/gpio_4  will add gpio_4 to gpios
// /gpios/gpio_4?pin=23&dir=output  will add gpio_4 to gpios with pin=23 and dir = output
// scan for a leading / then take the output up to the ebd of the string or the next /
// a leadig slash will shif the focus onto the main Things register
//

int Split(vector<string>&lst, const string& sinput, const string& seps, bool remove_empty)
{
  cout << " input ["<<sinput<<"] seps ["<<seps<<"] size ("<< sinput.size()<<")\n";
  //return 0; 
  string input =sinput;
  input.erase(0,1);
   ostringstream word;
    for (size_t n = 0; n < input.size(); ++n)
    {
        if (string::npos == seps.find(input[n]))
            word << input[n];
        else
        {
    	     cout <<" found word ["<<word.str()<<"]\n";
            if (!word.str().empty() || !remove_empty)
                lst.push_back(word.str());
            word.str("");
        }
    }
    if (!word.str().empty() || !remove_empty)
      {
	cout <<" found word ["<<word.str()<<"]\n";
        lst.push_back(word.str());
      }
}
int SplitString(string &sname, string &attrs, string &remains, string &src)
{
  int ret = 0;
  sname = src;
  if(sname.find('/',0) == 0)
  {
      ret = 1;
      sname.erase(0,1);
  }
  size_t end1 = sname.find('/',0);
  if (end1 == string::npos)
  {
      string empty;
      remains=empty;
  }
  else
  {
      string dummy = sname.substr(0, end1);
      remains = sname.substr(end1);
      if(remains.find('/',0)==0)
      {
	  remains.erase(0,1);
      }

      sname = dummy;
  }
   end1 = sname.find('?',0);
   if (end1 != string::npos)
   {
       attrs = sname.substr(end1);
       sname.erase(end1, string::npos);
   }
   return ret;
}

void T2::SetAttrs(string &sattrs)
{
  vector <string> attrs;
  vector <string>::iterator it;

  Split(attrs, sattrs, "?&", true);
  for (it = attrs.begin(); it != attrs.end(); ++it)
  {
    cout << " attr ["<<*it<<"]\n"; 
    SetAttr(*it);
  }
}
void T2::SetAttr(string &sattrs)
{
    vector <string> subs;
    if(sattrs.size() == 0) return;
    cout <<" sattrs size ("<<sattrs<<")("<<sattrs.size()<<")\n";
    string s1,s2;
    //return;
    size_t mid;
    mid = sattrs.find('=',0);
    if(string::npos != mid)
      {
	s1=sattrs.substr(0,mid);
	s2=sattrs.substr(mid+1);
      }
    else
      {
	s1=sattrs;
      }
    cout <<" s1 ["<<s1<<"] s2 [" << s2 <<"]\n";
    //return;
    //    Split(subs, sattrs, "=", true);

    //return;
    if(!Attrs[s1])
      {
	addAttr(s1);
      }
    if (s2.size() > 0)
      {
	// check changed
	if(Attrs[s1]->value == s2)
	  {
	  }
	else 
	  {
	    Attrs[s1]->value = s2;
	    valChanged=true;
	  }
      }
}
 
T2* operator<<(T2* t2, const string &insrc)
{

    string sname;
    string src = insrc;
    string attrs;
    string remains;
    if (src.size() == 0) return t2;

    int isBrother=SplitString(sname, attrs, remains, src);
    cout << "src ["<< src <<"] sname ["<<sname<<"] attrs ["<<attrs<<"] remains ["<< remains<<"] "<< endl;
    T2 * myt;
    /// look for leading slash, if found add this to the global Kids
    if(isBrother)
    {
        if (!t2->parent)
        {
	    Kids[sname] = new T2(sname);
	    Kids[sname]->parent = NULL;
	    Kids[sname]->depth = 0;
	    myt = Kids[sname];
	}
	else
        {
	    t2->parent->Kids[sname] = new T2(sname);
	    t2->parent->Kids[sname]->parent = NULL;
	    t2->parent->Kids[sname]->depth = 0;
	    myt = t2->parent->Kids[sname];
	}
    }
    else 
    {
        t2->Kids[sname] = new T2(sname);
	t2->Kids[sname]->parent = t2;
	t2->Kids[sname]->depth = t2->depth+1;
	myt = t2->Kids[sname];
    }
    cout << " ****myt name ["<< myt->name << "] attrs ["<<attrs<<"] size "<< attrs.size()<<"\n";
    if(attrs.size() > 0)
      myt->SetAttrs(attrs);
    if(remains.size() > 0)
      return myt<<remains;
    else 
      return myt;
}

ostream& operator<<(ostream& os, T2* t2)
{
    tMap::iterator iter;
    t2->setIndent(os);
    os << t2->name<< endl;
    for (iter=t2->Attrs.begin(); iter != t2->Attrs.end(); ++iter)
      {
	T2 *tx = t2->Attrs[iter->first];
	tx->setIndent(os);
	os << tx->name<< " ["<<tx->value<<"]"<<endl;
      }
    for (iter=t2->Kids.begin(); iter != t2->Kids.end(); ++iter)
      {
	T2 *tx = t2->Kids[iter->first];
	tx->setIndent(os);
	os << t2->Kids[iter->first];
      }
    return os;
}

