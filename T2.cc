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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;

#include "T2.h"
void myDie(const char *msg) { perror(msg); exit(1); }

// we will loose this soon
extern tMap Kids;

//used to split an attr (s1=s2) into s1 and s2
int SplitIt(string &s1, string &s2, const string &sattrs, const char &c)
{
    int rc = 0;
    if(sattrs.size() > 0) 
    {
	size_t mid;
	mid = sattrs.find(c, 0);
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

// returns 0 for just a name ,1 for lib 2 for cmd
int DecodeName(string &sname, string &slib, string &sact, string &satr, const string& sin)
{
  size_t lib = sin.find('@', 0);
  size_t act = sin.find('!', 0);
  size_t atr = sin.find('?', 0);
  int rc2 =0;
  int rc = 0;
  string s2;
  cout << " lib ["<<lib<<"] act ["<<act<<"] atr ["<<atr<<"] \n";
  // options here are find the lowest of fcn,cmd,atr
  if (
      (lib == string::npos)
      && (act == string::npos)
      && (atr == string::npos))
    {
      sname = sin;
      return 0;
    }

  if (
         (act == string::npos)
      && (atr == string::npos))  // name + lib
    {

      sname=sin.substr(0, lib);
      slib=sin.substr(lib+1);
      return 1;
    }

  if (
         (lib == string::npos)
      && (atr == string::npos))  // name + act
    {

      sname=sin.substr(0, act);
      sact=sin.substr(act+1);
      return 1;
    }


  if (
         (lib == string::npos)
      && (act == string::npos))  // name + atr
    {

      sname=sin.substr(0, atr);
      satr=sin.substr(atr);
      return 1;
    }

 //foo222@gpios\!list
  if (
      (atr == string::npos))// name + lib + act no attr
    {
      if ( act < lib ) {
	sname=sin.substr(0, act);
	sact=sin.substr(act+1, lib-act-1);
	slib=sin.substr(lib+1);
      } else {
	sname=sin.substr(0, lib);
	slib=sin.substr(lib+1, act-lib-1);
	sact=sin.substr(act+1);
      }
      return 2;
    }


  //foo222@gpios?list=21\&fww=2
  if (
      (act == string::npos))// name + lib + atr no act
    {
      if ( atr < lib ) {
	sname=sin.substr(0, atr);
	satr=sin.substr(atr, lib-atr);
	slib=sin.substr(lib+1);
      } else {
	sname=sin.substr(0, lib);
	slib=sin.substr(lib+1, atr-lib-1);
	satr=sin.substr(atr);
      }
      return 2;
    }

  //foo222\!show?list=21\&fww=2
  if (
      (lib == string::npos))// name + act + atr no lib
    {
      if ( atr < act ) {
	sname=sin.substr(0, atr);
	satr=sin.substr(atr, act-atr);
	sact=sin.substr(act+1);
	return 2;
      } else {
	sname=sin.substr(0, act);
	sact=sin.substr(act+1, atr-act-1);
	satr=sin.substr(atr);
	return 2;
      }
    }

  // OK we have all three
  //foo222\!show@gpios?list=21\&fww=2

  if (act < lib) 
    if (lib < atr)
      {
	sname=sin.substr(0, act);
	sact=sin.substr(act+1, lib-act-1);
	slib=sin.substr(lib+1, atr-lib-1);
	satr=sin.substr(atr);
	return 3;
      }
  //foo222\!show?list=21\&fww=2@gpios

  if (act < atr) 
    if (atr < lib)
      {
	sname=sin.substr(0, act);
	sact=sin.substr(act+1, atr-act-1);
	satr=sin.substr(atr,lib-atr);
	slib=sin.substr(lib+1);
	return 3;
      }

  //foo222@gpios?list=21\&fww=2\!show
  if (lib < atr) 
    if (atr < act)
      {
	sname=sin.substr(0, lib);
	slib=sin.substr(lib+1, atr-lib-1);
	satr=sin.substr(atr,act-atr);
	sact=sin.substr(act+1);
	return 3;
      }

  //foo222@gpios\!show?list=21\&fww=2

  if (lib < act) 
    if (act < atr)
      {
	sname=sin.substr(0, lib);
	slib=sin.substr(lib+1, act-lib-1);
	sact=sin.substr(act+1, atr-act-1);
	satr=sin.substr(atr);
	return 3;
      }

  //foo222?list=21\&fww=2@gpios\!show

  if (atr < lib) 
    if (lib < act)
      {
	sname=sin.substr(0, atr);
	slib=sin.substr(lib+1, act-lib-1);
	satr=sin.substr(atr,lib-atr);
	sact=sin.substr(act+1,atr);
	return 3;
      }

  //foo222?list=21\&fww=2\!show@gpios
  if (atr < act) 
    if (act < lib)
      {
	sname=sin.substr(0, atr);
	slib=sin.substr(lib+1);
	satr=sin.substr(atr,act-atr);
	sact=sin.substr(act+1,lib-act-1);
	return 3;
      }

#if 0
  if ((fcn < atr) && ( fcn < cmd )) 
    {
      s2=sin.substr(fcn); // still have to find any attrs in here

      if ((atr == string::npos)
	  && (cmd == string::npos))
	sfcn = s2

      if ((cmd < atr) && (atr == string::npos))
	sact = sin.substr(atr);

      if ((atr < cmd) && (cmd == string::npos))
	sact = sin.substr(atr);
	
      if (atr < cmd) rc2 =3;
    } 

  if ((atr < fcn) && ( atr < cmd )) 
    {
      sname=sin.substr(0, atr);
      s2=sin.substr(atr); // still have to find any attrs in here
      rc2 = 4;
      if (cmd < fcn) rc2 =5;
      if (fcn < cmd) rc2 =6;
    } 
  if ((cmd < fcn) && ( cmd < atr )) 
    {
      sname=sin.substr(0, cmd);
      s2=sin.substr(cmd); // still have to find any attrs in here
      rc2 = 7;
      if (atr < fcn) rc2 =8;
      if (fcn < atr) rc2 =9;
    } 
  
  cout << " sname ["<<sname<<"] s2 ["<<s2<<"] rc2 ["<<rc2<<"]\n";
  switch (rc2) {
  case 1:
    break;
  case 4:
  case 7:
    break;
  case 2: // s2 = fcn/cmd/
    

  }
  rc = 0;
  // we now have sname and s2 we now have to find commands and atttrs
  if (
#if 0
  if(string::npos != fcn)
    {
      sname=sin.substr(0, fcn);
      s2=sin.substr(fcn+1); // still have to find any attrs in here
      SplitIt(sfcn, sattrs, s2, '?');
      rc = 1;
    }
  else if(string::npos != cmd)
    {
      sname=sin.substr(0, cmd);
      s2=sin.substr(cmd+1); // still have to find any attrs in here
      SplitIt(sact, sattrs, s2, '?');
      rc = 2;
    }
  else 
    {
      sname = sin;
    }
#endif
#endif
  return rc;
}


int SplitAddr(string &s1, string &s2, const string &sattrs)
{
  return SplitIt(s1,s2,sattrs,':');
}

// make this much more generic
// name@fcn[?attrs&stuff]
// name!action[?attrs&stuff]
// name@fcn inherits the actions from fcn
// name!action run the action
// this crap can die
//
// rc = 1 just a name
// rc = 2 name + port            we are a server
// rc = 3 name + address + port  we are a link
//
int StringNewName(string& new_name, string &addr, string &port, const string &sin)
{
  int rc = 1;
  string s2;
  rc = SplitName(new_name, s2, sin);
  if (rc != 1)
    {
      rc = 2;
      rc = SplitAddr(addr, port, s2);
  
      if(addr.size() > 0) rc=3;
    }
  return rc;
}

// 
//



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

void T2::SetAttrs(const string &sattrs)
{
    vector <string> attrs;
    vector <string>::iterator it;
    if (sattrs.size() == 0) 
      {
	cout << " No attrs to set \n";
	return;
      }
    Split(attrs, sattrs, "?&", true);
    for (it = attrs.begin(); it != attrs.end(); ++it)
    {
      cout << " Set attr ["<<*it<<"] for ["<< name<<"]\n"; 
	SetAttr(*it);
    }
}

void T2::SetAttr(const string &sattrs)
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



// revise the isBrother concept to mean isMe
// this means that the /name@addr:port
//            or      /name@:port
// will work
T2* operator<<(T2* t2, const string &insrc)
{
    string sname;
    string src = insrc;
    string attrs;
    string remains;
    if (src.size() == 0) return t2;

    int isMe=SplitString(sname, attrs, remains, src);
    cout << "src ["<< src <<"] sname ["<<sname<<"] attrs ["<<attrs<<"] remains ["<< remains<<"] "<< endl;
    //return t2;
    T2 * myt;
    /// look for leading slash, if found, affect me
    // TODO restore attrs and remains
    if(isMe)
    {
        string new_name, addr, port;
        int scount= StringNewName(new_name, addr, port, insrc);
	new_name.erase(0,1);

        if (scount == 1)
	  {
	    t2->name = new_name;
	    //return t2;
	  }
        if (scount == 2)
	  {
	    t2->name = new_name;
	    //t2->RunServer(port);  // will spin forever
            // todo send remains
	    // perhaps we do the remains first ans then run the server
	  }
        if (scount == 2)
	  {
	    t2->name = new_name;
	    //t2->SetLink(addr, port);  
            // todo send remains across link
	    // perhaps we do the remains first ans then run the server
	  }
    }
#if 0
        if (!t2->parent)
        {
	  cout << "adding "<<sname <<" at base \n";
	  //return t2;
	    getMap(Kids, sname);
	    Kids[sname]->parent = NULL;
	    Kids[sname]->depth = 0;
	    myt = Kids[sname];
	}
	else
        {
	    cout << "adding "<<sname <<" to [" << t2->parent->name<<"] \n";
	    getMap(Kids, sname);
	    t2->parent->Kids[sname]->parent = NULL;
	    t2->parent->Kids[sname]->depth = t2->parent->depth+1;
	    myt = t2->parent->Kids[sname];
	}
    }
#endif
    else 
    {
      cout << "adding Kid "<<sname <<" to [" << t2->name<<"] \n";
        t2->getMap(Kids, sname);
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

