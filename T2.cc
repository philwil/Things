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

T2* operator<<(T2* t2, const string &name)
{
    t2->Kids[name] = new T2(name);
    t2->Kids[name]->parent =t2;
    t2->Kids[name]->depth =t2->depth+1;

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

