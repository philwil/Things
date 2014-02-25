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

// we will loose this soon
extern tMap Kids;

//used to split an attr (s1=s2) into s1 ans s2
int SplitIt(string &s1, string &s2, const string &sattrs, const char &c)
{
    int rc = 0;
    if(sattrs.size() > 0) 
    {
	size_t mid;
	mid = sattrs.find(c,0);
	if(string::npos != mid)
	{
	    s1=sattrs.substr(0,mid);
	    s2=sattrs.substr(mid+1);
	    rc = 2;
	}
	else
	{
	    s1=sattrs;
	    rc = 1;
	}
    }
    return rc;
}

//used to split an attr (s1=s2) into s1 ans s2
int SplitAttr(string &s1, string &s2, const string &sattrs)
{
  return SplitIt(s1,s2,sattrs,'=');
}

int SplitName(string &s1, string &s2, const string &sattrs)
{
  return SplitIt(s1,s2,sattrs,'@');
}

int SplitAddr(string &s1, string &s2, const string &sattrs)
{
  return SplitIt(s1,s2,sattrs,':');
}



// the split program, needs testing
int Split(vector<string>&lst, const string& sinput, const string& seps
	  , bool remove_empty)
{
  //  cout << " input ["<<sinput<<"] seps ["<<seps<<"] size ("<< sinput.size()<<")\n";
    if (sinput.size()>0)
    {
	string input =sinput;

	// strip a leading sep
	
	if(string::npos != seps.find(input[0]))
	{
	    input.erase(0,1);
	}
	ostringstream word;
	for (size_t n = 0; n < input.size(); ++n)
	{
	    if (string::npos == seps.find(input[n]))
	      word << input[n];
	    else
	    {
		//    	     cout <<" found word ["<<word.str()<<"]\n";
		if (!word.str().empty() || !remove_empty)
		    lst.push_back(word.str());
		word.str("");
	    }
	}
	if (!word.str().empty() || !remove_empty)
	{
	    //cout <<" found word ["<<word.str()<<"]\n";
	    lst.push_back(word.str());
	}
    }
}
// Another split program
// splits an input string 
// ("/foo?att=1&attr=2/fun/some/other/stuff")
//      sname attrs           remains
// into [foo] [?att=1&attr=2] [fun/some/other/stuff]
int SplitString(string &sname, string &attrs, string &remains, string &src)
{
    int ret = 0;
    if (src.size() > 0)
    {
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
    }
    return ret;
}


void T2::Show(ostream &os)
{
    setIndent(os); os<< name <<endl;
    tMap::iterator iter;
    for (iter=Attrs.begin(); iter != Attrs.end(); ++iter)
      {
	//Attrs[iter->first]->
        setIndent(os,1);
	os << iter->first
	   << " ["<<Attrs[iter->first]->value<<"]"<<endl;
      }
    os <<"\n";
    for (iter=Kids.begin(); iter != Kids.end(); ++iter)
      {
	Kids[iter->first]->Show(os);;
      }

}



// this is the main object loader
// /gpios/gpio_4  will add gpio_4 to gpios
// /gpios/gpio_4?pin=23&dir=output  
//  will add gpio_4 to gpios with pin=23 and dir = output
// scan for a leading / then take the output up to the end of the 
// string or the next /
// a leading slash will shift the focus onto the main Things register
//

void T2::SetAttrs(string &sattrs)
{
    vector <string> attrs;
    vector <string>::iterator it;

    Split(attrs, sattrs, "?&", true);
    for (it = attrs.begin(); it != attrs.end(); ++it)
    {
      cout << " Set attr ["<<*it<<"] for ["<< name<<"]\n"; 
	SetAttr(*it);
    }
}

void T2::SetAttr(string &sattrs)
{
    vector <string> subs;
    if(sattrs.size() == 0) return;
    //cout <<" sattrs size ("<<sattrs<<")("<<sattrs.size()<<")\n";
    string s1,s2;
    SplitAttr(s1, s2, sattrs);
    cout <<" s1 ["<<s1<<"] s2 [" << s2 <<"]\n"; 
    //return ;

    if(!Attrs[s1])
    {
	addAttr(s1);
    }
    if (s2.size() > 0)
    {
	// check changed
        if(Attrs[s1]->value != s2)
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
    //return t2;
    T2 * myt;
    /// look for leading slash, if found add this to the global Kids
    if(isBrother)
    {
        if (!t2->parent)
        {
	  cout << "adding "<<sname <<" at base \n";
	  //return t2;
	    Kids[sname] = new T2(sname);
	    Kids[sname]->parent = NULL;
	    Kids[sname]->depth = 0;
	    myt = Kids[sname];
	}
	else
        {
	  cout << "adding "<<sname <<" to [" << t2->parent->name<<"] \n";
	    t2->parent->Kids[sname] = new T2(sname);
	    t2->parent->Kids[sname]->parent = NULL;
	    t2->parent->Kids[sname]->depth = t2->parent->depth+1;
	    myt = t2->parent->Kids[sname];
	}
    }
    else 
    {
      cout << "adding "<<sname <<" to [" << t2->name<<"] \n";
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
// show the structure
ostream& operator<<(ostream& os, T2* t2)
{
    t2->Show(os);
    return os;
}

