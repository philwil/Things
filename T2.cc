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


void T2::Show(string dummy)
{

     cout<<dummy << name <<endl;
    tMap::iterator iter;
    for (iter=Attrs.begin(); iter != Attrs.end(); ++iter)
      {
	cout << dummy + "   " << iter->first<< " ["<<Attrs[iter->first]->value<<"]"<<endl;
      }
    for (iter=Kids.begin(); iter != Kids.end(); ++iter)
      {
	Kids[iter->first]->Show(dummy + "   ");;
      }

}

T2* operator<<(T2* t2, const string &name)
{
    t2->Kids[name] = new T2(name);
    return t2;
}
ostream& operator<<(ostream& os, T2* t2)
{
    string dummy = "  ";
    tMap::iterator iter;

    os << dummy << t2->name<< endl;
    for (iter=t2->Attrs.begin(); iter != t2->Attrs.end(); ++iter)
      {
	os << dummy << iter->first<< " ["<<t2->Attrs[iter->first]->value<<"]"<<endl;
      }
    for (iter=t2->Kids.begin(); iter != t2->Kids.end(); ++iter)
      {
	os << dummy +"  " << t2->Kids[iter->first];
      }
    return os;
}

