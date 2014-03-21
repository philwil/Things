// string::find_first_of
#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>        // std::size_t
#include <map>
#include <iostream>
#include <sstream>
# include <stdio.h>

#include "Strings.h"

using namespace std;    

// fix up a manula tcp entry
void fixTcpCmd(string &cmd,char *buffer)
{
    cmd = (string)buffer;
    if(cmd.find('\n') != string::npos)
      cmd.erase(cmd.find('\n'),1);
    if(cmd.find('\r') != string::npos)
      cmd.erase(cmd.find('\r'),1);
    if (cmd[cmd.size()-1] == '\n') 
      cmd.erase(cmd.size()-1 , 1);
}


// OK this is the decode name 
// have a map of delims as an input
// The best Decode name !!!
// this sucks on the first delim
// look for n and if it is in slot 0 then proceed from there
int DecodeDelims(sMap& sMap, const string &dls, const string& sin)
{
  string delims=dls;
  string samp = sin;
  size_t found = -1;
  char skey;
  char sampc;
  int ix;
  int rc = 0;
  bool done = false;
  found=samp.find_first_of(delims);
  if (found == 0)
    {
      skey = samp[0];  // default first key nar redo  this
      samp.erase(0,1);
      delims.erase(delims.find(skey),1);
    } 
  else
    {
      skey = '/';
    }
  while((delims.size() > 0) && ! done)
    {
      rc++;
      found=samp.find_first_of(delims);
      //cout << "rc ("<<rc<<") found ("<<found<<")\n";
      //for (ix =0; ix < samp.size(); ++ix){
      //	cout << "ix["<<ix<<"] char (" << samp[ix];
      //printf(") hex [%x] \n", samp[ix]);
      //}
      //cout << "samp now ["<<samp<<"] delims ["<<delims<<"] key["<<skey<<"]\n";
      if (found==string::npos) {
	if (rc > 0)sMap[skey]=samp;
	done = true;
      } else {
	if (found >0) {
	  sMap[skey]=samp.substr(0, found);
	  //cout <<"Skey was ["<<skey<<"] data ["<< sMap[skey]<<"] \n";
	  skey=samp[found];
	  samp.erase(0,found);
	  //cout << "samp now ["<<samp<<"]\n";
          if(delims.find(skey) != string::npos) {
	    delims.erase(delims.find(skey),1);
	  }
	  //samp.erase(0,found);
	  //cout << "samp now ["<<samp<<"] delims ["<<delims<<"] key["<<skey<<"]\n";
	} else {
	  samp.erase(0,1);
	}
      }
      if(rc > 5) done=true;
    }
  return rc;
}

// Simply splits a string up into cmd and rest 
int DecodeCommand(string &scmd, string &srest, const string &delims, const string &sin)
{
  size_t found;
  int rc = 0;
  found=sin.find_first_of(delims,1);
  if (found == string::npos)
    {
    scmd = sin;
    }
  else
    {
      rc++;
      scmd=sin.substr(0,found);
      srest=sin.substr(found);
    }
  return rc;
}


void TestDecodeName(const string &sin)
{
  string dlims ="!@?";
  sMap sMap;
  sMap::iterator iter;

  cout << "sin  ["<<sin<<"] ======\n";
  int rc = DecodeDelims(sMap, dlims, sin);
  cout << "====output=====>\n";
  cout << "rc->("<<rc<<") \n";
  iter = sMap.begin();
  for ( ; iter != sMap.end(); ++iter) {
    cout << "["<<iter->first<<"] ==> ["<< iter->second <<"] \n";
  }
  return;
}

void TestDecodeCmd(const string &sin)
{
  string scmd;
  string srest;
  int rc = DecodeCommand(scmd,srest,"/", sin);
  cout << "sin  ["<<sin<<"] ======\n";

  cout << "====output=====>\n";
  cout << "rc->("<<rc<<") \n";
  cout << "scmd->("<<scmd<<") \n";
  cout << "srest->("<<srest<<") \n";
  return;
}

#ifdef TEST_MAIN

int main ()
{
  //  cout << str << '\n';
  TestDecodeName("foo");
  TestDecodeName("foo@gpios");
  TestDecodeName("foo@gpios?atrs=1&atrs=2");
  TestDecodeName("foo@gpios?atrs=1&atrs=2!list");
  TestDecodeName("foo@gpios!list");
  TestDecodeName("foo@gpios!list?atrs=1&atrs=2");
  TestDecodeName("foo!list");
  TestDecodeName("foo!list@gpios");
  TestDecodeName("foo!list@gpios?atrs=1&atrs=2");
  TestDecodeName("foo!list?atrs=1&atrs=2@gpios");
  TestDecodeName("foo?atrs=1&atrs=2");
  TestDecodeName("foo?atrs=1&atrs=2@gpios");
  TestDecodeName("foo?atrs=1&atrs=2@gpios!list");
  TestDecodeName("foo?atrs=1&atrs=2!list@gpios");

  TestDecodeCmd("/foo?atrs=1&atrs=2!list@gpios/foo2?atrs=1&atrs=2!list@gpios");
  TestDecodeCmd("numtwo?atrs=1&atrs=2!list@gpios/numthree?atrs=1&atrs=2!list@gpios");

  return 0;
}
#endif

// The rest, below this line, is junk
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
	size_t end1 = sname.find('/',1);
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
// Another split program
// splits an input string 
// ("/foo?att=1&attr=2/fun/some/other/stuff")
//      sname attrs           remains
// into [foo] [?att=1&attr=2] [fun/some/other/stuff]
int SplitString(string &sname, string &remains, string &src)
{
    int ret = 0;
    if (src.size() > 0)
    {
	sname = src;
	if(sname.find('/',0) == 0)
	{
	    ret = 1;
	    //sname.erase(0,1);
	}
	size_t end1 = sname.find('/',1);
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
    }
    return ret;
}


void oldTestDecodeName(const string &sin)
{
  string sname;
  string slib;
  string sact;
  string satr;
  
  cout << "sin  ["<<sin<<"] ======\n";
  int rc = DecodeName(sname, slib, sact, satr, sin);
  cout << "=========>";
  cout << "rc->("<<rc<<") ";
  cout << "sname->["<<sname<<"] ";
  cout << "slib->["<<slib<<"] ";
  cout << "sact->["<<sact<<"] ";
  cout << "satr->["<<satr<<"]\n\n";
  return;
}



// returns 0 for just a name ,1 for lib 2 for cmd
int DecodeName(string &sname, string &slib, string &sact, string &satr, const string& sin)
{
  string delims ="!@?";
  string samp = sin;
  size_t found = 0;
  char skey;
  int rc = 0;
  bool done = false;
  skey = '/';  // default first key
  while((delims.size() > 0) && ! done)
    {
      rc++;
      if(found > 0)
	{
	  skey=samp[found];
	  delims.erase(delims.find(skey),1);
	  samp.erase(0,found);
	}
      found=samp.find_first_of(delims);
      cout << "rc ("<<rc<<") samp now ["<<samp<<"] delims ["<<delims<<"] key["<<skey<<"]\n";
      if(skey == '/')
	{
	  sname=samp.substr(0, found);
	}
      if(skey == '@')
	{
	  slib=samp.substr(0, found);
	}
      if(skey == '!')
	{
	  sact=samp.substr(0, found);
	}
      if(skey == '?')
	{
	  satr=samp.substr(0, found);
	}
      done =(found==string::npos);
    }
  return rc;
}

#if 0

// this is what we came from 
// returns 0 for just a name ,1 for lib 2 for cmd
int badDecodeName(string &sname, string &slib, string &sact, string &satr, const string& sin)
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
#endif
