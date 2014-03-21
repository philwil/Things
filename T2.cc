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
extern tMap Types;


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

void T2::SetAttrs(const string &sin)
{
    vector <string> attrs;
    vector <string>::iterator it;
    if (sin.size() == 0) 
      {
	cout << " No attrs to set \n";
	return;
      }
    
    //cout << " Setting  attrs["<<sin<<"] \n";
    string sattrs = sin;
    
    int found = sattrs.find_first_of("?&");
    //cout << " Sattrs 0 ["<<sattrs<<"] for [] found ["<<found <<"]\n"; 
    if(found == 0)
      sattrs.erase(0,1);
    found = sattrs.find_first_of("?&");
    //cout << " Sattrs 1 ["<<sattrs<<"] for ["<< name<<"] found ["<<found <<"]\n"; 

    string satt;
    while ((found != string::npos) && (found > 0))
      {
	satt = sattrs.substr(0,found);
        //cout << " Set attr ["<<satt<<"] for ["<< name<<"] found ["<<found <<"]\n"; 
	SetAttr(satt);
	sattrs.erase(0,found+1);
	found = sattrs.find_first_of("?&");
      }
    if (sattrs.size())
      {
        //cout << " Set attr ["<<sattrs<<"] for ["<< name<<"] found ["<<found <<"]\n"; 
	SetAttr(sattrs);
      }

}
#if 0
int T2::SetLib(const string inname)
{
  string lname = inname;
  lname.erase(0,1);
  cout << "Create a Kid with a Lib " << endl;
  
  if (Types[lname] = NULL)
  {
      t2 = Types[lname]=new T2(lname);
      string lfull = "./libt2"+lname+".so";
    handle =  dlopen(lfull, RTLD_NOW);
    if ( !handle) {
      cerr << "dlopen "<< (char *)dlerror()<<"\n";
      return -1;
    }

    dlerror();  /* clear any current error */
    setup = (setup_t)dlsym(handle, "setup");
    char * error = (char *)dlerror();
    if (error != NULL) {
      cout << "dlsym error \n"<<(char *)dlerror()<<"\n";
      return -1;
    }
    int ret = setup(cout, t2, NULL);
    cout << " Setup return value [" <<ret<<"] \n";
#if 0
    if (ret == 0)
      {
	t2->RunAction(cout, "!scan", NULL);
	t2->RunAction(cout, "!show", NULL);
      }
#endif

    return ret;
}

#endif

void T2::SetAttr(const string &sattrs)
{
  //    vector <string> subs;
    if(sattrs.size() == 0) return;
    //cout <<" sattrs size ("<<sattrs<<")("<<sattrs.size()<<")\n";
    string sat, s1,s2;
    sat = sattrs;
    if ((sat[0] == '?') || (sat[0]=='&'))  
      sat.erase(0,1);
    SplitAttr(s1, s2, sat);
    cout <<" SetAttr sattrs ["<<sattrs<<"] sat [" << sat <<"]\n"; 
    cout <<" SetAttr s1 ["<<s1<<"] s2 [" << s2 <<"]\n"; 
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
 

int T2::addLib(ostream&os, const string&slib_in, void *data)
{
  string slib = slib_in;
  if (slib.size()==0) return -1; 

  if (slib[0]=='@')
    {
      slib.erase(0,1);
    }
  if (slib.size()>0) 
    {
      T2 *t2_t;
      
      if (!Types[slib])
	{
	  cout << " Creating Type [" << slib <<"]\n";
	  string dlname = "./libt2"+slib+".so";
	  void * handle;
	  action_t setup;
	  Types[slib] = new T2(slib);
	  t2_t = Types[slib];

	  handle =  dlopen(dlname.c_str(), RTLD_NOW);
	  if (!handle) 
	    {
	      cout << "dlopen "<< (char *)dlerror()<<"\n";
	      return -1;
	    }
	  
	  dlerror();  /* clear any current error */
	  setup = (action_t)dlsym(handle, "setup");
	  char *error = (char *)dlerror();
	  if (error != NULL) {
	    cout << "dlsym error \n"<<(char *)dlerror()<<"\n";
	    return -1;
	  }
	  int ret = setup(cout, t2_t, NULL);
	}
      t2_t =  Types[slib];
      //cout << t2_t;
      copyAttrs(os, t2_t); 
      t2_type = t2_t;
    }
  return 0;
}

int T2::ServiceInput(ostream&os, const string&insrc, void *data)
{
    string sname;
    string src = insrc;
    string remains;

    if (src.size() == 0) return 0;

    int isMe=SplitString(sname, remains, src);
    cout << "src ["<< src <<"] sname ["<<sname<<"] remains ["<< remains<<"] "<< endl;
    sMap sMap;
    string dlims ="!@?";
    // split up into this command and the "rest"
    int rc = DecodeDelims(sMap, dlims, sname);
    sMap::iterator iter;
    T2 *myt2=this;
    T2 *origt2 = this;
    string kname;
    // the / element is the child of interest
    // the ? element is the attributes
    // the @ element is the lib
    // the ! element is the list of commands
    iter = sMap.begin();
    for ( ; iter != sMap.end(); ++iter) {
      switch (iter->first) {
      case '/':
	cout <<" create / use Kid ["<< iter->second <<"] \n";
	kname = iter->second;
        if(kname[0]=='/')kname.erase(0,1);
        if(myt2->getMap(Kids, kname, false) == NULL)
	  {
	    cout << "adding Kid "<<kname <<" to [" << myt2->name<<"] \n";
	    myt2->AddKid(kname);
	    //	    myt2->Kids[kname] = new T2(kname);
	    myt2= myt2->Kids[kname];
	  }
	else 
	  {
	    cout << "using Kid "<<kname <<" from [" << myt2->name<<"] \n";
	    myt2 = myt2->Kids[kname];
	  }
	break;

      case '?':
	cout <<" process attrs ["<< iter->second <<"] \n";
	myt2->SetAttrs(iter->second);
	break;

      case '@':
	os <<" load lib ["<< iter->second <<"] \n";
	//myt2->addFcn(os, iter->second);
	myt2->addLib(os, iter->second, data);
	myt2->t2_type = Types[iter->second];
	break;
      case '!':
	cout <<" run command ["<< iter->second <<"] \n";
	string act = iter->second;
	myt2->RunAction(os, act, (void *)origt2);
	break;

      }
    }
    if (remains.size() > 0)
      {
	myt2->ServiceInput(os, remains, data);
      }
    return 0;
#if 0

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
#endif
}

// revise the isBrother concept to mean isMe
// this means that the /name@addr:port
//            or      /name@:port
// will work
T2* operator<<(T2*t2, const string &insrc)
{
    string sname;
    string src = insrc;
    string remains;

    if (src.size() == 0) return t2;

    int isMe=SplitString(sname, remains, src);
    cout << "src ["<< src <<"] sname ["<<sname<<"] remains ["<< remains<<"] "<< endl;
    sMap sMap;
    string dlims ="!@?";
    int rc = DecodeDelims(sMap, dlims, sname);
    sMap::iterator iter;
    T2 *myt2=t2;
    T2 *origt2 = t2;
    string kname;
    // the / element is the child of interest
    // the ? element is the attributes
    // the @ element is the lib
    // the ! element is the list of commands
    iter = sMap.begin();
    for ( ; iter != sMap.end(); ++iter) {
      switch (iter->first) {
      case '/':
	cout <<" create / use Kid ["<< iter->second <<"] \n";
	kname = iter->second;
        if(kname[0]=='/')kname.erase(0,1);
        if(myt2->getMap(Kids, kname, false) == NULL)
	  {
	    cout << "adding Kid "<<kname <<" to [" << myt2->name<<"] \n";
	    myt2->Kids[kname] = new T2(kname);
	    myt2= myt2->Kids[kname];
	  }
	else 
	  {
	    cout << "using Kid "<<kname <<" from [" << myt2->name<<"] \n";
	    myt2 = myt2->Kids[kname];
	  }
	break;

      case '?':
	cout <<" process attrs ["<< iter->second <<"] \n";
	myt2->SetAttrs(iter->second);
	break;

      case '@':
	cout <<" load lib ["<< iter->second <<"] \n";
	myt2->addFcn(cout, iter->second);
	myt2->t2_type = Types[iter->second];
	break;
      case '!':
	cout <<" run command ["<< iter->second <<"] \n";
	string act = iter->second;
	myt2->RunAction(cout, act, (void *)origt2);
	break;

      }
    }
    return t2;
#if 0

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
#endif
}

// show the structure
ostream& operator<<(ostream& os, T2* t2)
{
    t2->Show(os);
    return os;
}

