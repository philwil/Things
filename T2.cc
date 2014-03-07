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
#include "Strings.h"

void myDie(const char *msg) { perror(msg); exit(1); }

// we will loose this soon
extern tMap Kids;


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

