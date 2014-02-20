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

int fixString(string &sname, string &attrs, string &remains, string &src)
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
 
T2* operator<<(T2* t2, const string &sname)
{

  
  /// look for leading slash, if found add this to the global Kids
  if(sname.find('/',0) == 0)
    {
      string nname=sname;
      nname.erase(0,1);
      Kids[nname] = new T2(nname);
      Kids[nname]->parent = NULL;
      Kids[nname]->depth = 0;
    }
  else 
    {
      t2->Kids[sname] = new T2(sname);
      t2->Kids[sname]->parent = t2;
      t2->Kids[sname]->depth = t2->depth+1;
    }
  return t2;
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

