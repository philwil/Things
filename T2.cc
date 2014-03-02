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


void *sockThread(void *data)
{

  struct T2::t2Server *t2s = (struct T2::t2Server *)data;

  T2 *t2 = t2s->t2;
  int rc=1;
  char buffer[2048];
  string prompt="\nthings=>";
  string reply;
  char * rep;
  int mysock = t2s->sock;

  cout << " Input  thread  created for {" << t2->name<<"]" << "\n";

  while (rc >0) {
    ostringstream ocout;
    prompt="\n"+ t2->name+"=>";

    if ( rc > 0 ) {
      rc = SendClient(mysock, prompt);
    }
    rc = RecvClient(mysock, buffer, sizeof buffer);
    string cmd = (string)buffer;
    cout << " got rc ["<<rc<<"] cmd ["<< cmd<<"]\n";
    rc = SendClient(mysock, cmd);
  }
  cout <<"client closed \n";
  close(mysock);  
  delete t2s;
  return NULL;
}

void myDie(const char *msg) { perror(msg); exit(1); }
// this sets up a socket and triggers client threads 

int T2::RunServer(string &port)
{
  int mainsock, clientsock;
  struct sockaddr_in server;
  struct hostent *serverh;
  int optval = 1;

  cout << " Running Server on  Port " << port << endl; 
  /* Create the TCP socket */
  if ((mainsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    myDie("Failed to create socket");
  }
  /* set the reuse addr option */
  setsockopt(mainsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* Construct the server sockaddr_in structure */
  memset(&server, 0, sizeof(server));       /* Clear struct */
  server.sin_family = AF_INET;                  /* Internet/IP */
  server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  server.sin_port = htons(atoi(port.c_str()));       /* server port */
           

  /* Bind the server socket */
  if (bind(mainsock, (struct sockaddr *) &server,
	   sizeof(server)) < 0) {
    myDie("Failed to bind the server port");

    // cout << " Failed to bind to  Port " << port << endl; 
  }

  /* Listen on the server socket */
  if (listen(mainsock, /*MAXPENDING*/ 5) < 0) {
    myDie("Failed to listen on server socket");
  }
  
  /* Run until cancelled */
  while (1) {
    struct sockaddr_in *client;
    unsigned int clientlen = sizeof(*client);
    client = (struct sockaddr_in *)malloc(clientlen);
    /* Wait for client connection */
    if ((clientsock =
	 accept(mainsock, (struct sockaddr *) client,
		&clientlen)) < 0) {
      myDie("Failed to accept client connection");
    }
    //    fprintf(stdout, "Client connected: %s\n",
    //	    inet_ntoa(client->sin_addr));
    /* hand over operation to the client call back */

    //HandleClient(clientsock, data, client);
    t2Server *t2s = new t2Server;
    t2s->sock = clientsock;
    t2s->t2=this;

    //sock = clientsock;
    int rc = pthread_create(&t2s->thr, NULL, sockThread, (void*)t2s);

  }
  return mainsock;
}
// sets up the setlink socket
int T2::SetLink(string &addr, string &port)
{
  int mainsock;
  struct sockaddr_in server, client;
  struct hostent *serverh;
  int optval = 1;

  cout << " Running Link to "<<addr<<" on  Port " << port << endl; 
  /* Create the TCP socket */
  if ((mainsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    myDie("Failed to create socket");
  }
  /* set the reuse addr option */
  setsockopt(mainsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* Construct the server sockaddr_in structure */
  memset(&server, 0, sizeof(server));       /* Clear struct */
  server.sin_family = AF_INET;                  /* Internet/IP */
  server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  server.sin_port = htons(atoi(port.c_str()));       /* server port */
           
  if(addr.size() > 0)
    {
      /* gethostbyname: get the server's DNS entry */
      serverh = gethostbyname((const char *)addr.c_str());
      if (serverh == NULL) {
	fprintf(stderr,"ERROR, no such host as %s\n", (char *)addr.c_str());
	return 0;
      }
      
      /* build the server's Internet address */
      bzero((char *) &client, sizeof(client));
      client.sin_family = AF_INET;
      bcopy((char *)serverh->h_addr, 
	    (char *)&client.sin_addr.s_addr, serverh->h_length);
      client.sin_port = htons(atoi(port.c_str()));
      
      /* connect: create a connection with the server */
      if (connect(mainsock, (struct sockaddr *)&client, sizeof(client)) < 0) 
	{
	  printf("ERROR connecting");
	  return 0;
	}
      linksock = mainsock;
    }
  return mainsock;
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
	    t2->RunServer(port);  // will spin forever
            // todo send remains
	    // perhaps we do the remains first ans then run the server
	  }
        if (scount == 2)
	  {
	    t2->name = new_name;
	    t2->SetLink(addr, port);  
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

