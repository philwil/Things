/*******************************************
 **
 ** tcplib for T2.cc
 **
 ** ./T2 name@tcp?port=3456 starts a service called name 
 **  on port 3456
 **
 ** ./T2 name@tcp?address=10.0.1.2&port=3456 starts a gateway called name 
 **  to 10.0.1.2:3456
 ** well not quite
 ** name!link
 ** name!serve   actually do the work
 ******************************************/

#include <iostream>
#include <map>
#include <cstring>
#include <string>
#include <sstream>
#include <istream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <fstream>
#include <limits>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <dlfcn.h>

using namespace std;

#include "T2.h"
#include "Socket.h"
#include "t2tcps.h"

// used from C++ code
int SendClient(int sock, string msg)
{
  int rc;
    /* Send back received data */
  rc = send(sock, msg.c_str(), msg.length(), 0);
  return rc;
}

// used from C++ code
int RecvClient(int sock, char *buffer, int len)
{
  int rc;
  /* receive data */
  rc = recv(sock, buffer, len, 0);
  return rc;
}


// server thread
static void *tcpsThread(void *data)
{

  struct t2Sock *t2s = (struct t2Sock *)data;

  T2 *t2 = t2s->t2;
  int rc=1;
  char buffer[2048];
  string prompt="\nthings=>";
  string reply;
  char * rep;

  // todo move this to os
  cout << " Input  thread  created for [" << t2->name<<"]" << "\n";

  while (rc>0) {
    ostringstream ocout;
    prompt="\n"+ t2->name+"=>";

    if ( rc > 0 ) {
      rc = SendClient(t2s->sock, prompt);
    }
    rc = RecvClient(t2s->sock, buffer, sizeof buffer);
    string cmd = (string)buffer;
    cout << " got rc ["<<rc<<"] cmd ["<< cmd<<"]\n";
    rc = SendClient(t2s->sock, cmd);
  }
  cout <<"client closed \n";
  close(t2s->sock);  
  delete t2s;
  return NULL;
}



// this will run a server with access to the T2 object
// t2->Attrs["port"]->value is the port number
int tcpServ(ostream& os, T2 *t2, void *data)
{
  int sock;
  struct sockaddr_in server;
  int optval = 1;
  string pstring;
  int port;
  // TODO handle errors here
  if(t2->Attrs.find("port") == t2->Attrs.end())
    return -1;

  pstring = t2->Attrs["port"]->value;
  port=atoi(pstring.c_str());

  os << " Running Server on  Port (" << port <<") \n";

  /* Create the TCP socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    myDie("Failed to create socket");
  }
  /* set the reuse addr option */
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* Construct the server sockaddr_in structure */
  memset(&server, 0, sizeof(server));           /* Clear struct */
  server.sin_family = AF_INET;                  /* Internet/IP */
  server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  server.sin_port = htons(port);                /* server port */
           

  /* Bind the server socket */
  if (bind(sock, (struct sockaddr *) &server,
	   sizeof(server)) < 0) {
    myDie("Failed to bind the server port");

    // cout << " Failed to bind to  Port " << port << endl; 
  }

  /* Listen on the server socket */
  if (listen(sock, /*MAXPENDING*/ 5) < 0) {
    myDie("Failed to listen on server socket");
  }
  
  /* Run until cancelled */
  while (1) {

    t2Sock *t2s = new t2Sock;
    memset((void *)t2s, 0, sizeof(*t2s));           /* Clear struct */

    t2s->clientlen = sizeof(t2s->client);
    /* Wait for client connection */
    if ((t2s->sock =
	 accept(sock, (struct sockaddr *) &t2s->client,
		&t2s->clientlen)) < 0) {
      myDie("Failed to accept client connection");
    }

    t2s->t2=t2;
    int rc = pthread_create(&t2s->thr, NULL, tcpsThread, (void*)t2s);
  }
  return 0;
}


// this will connect to a remote server
// t2->Attrs["port"]->value is the port number
// t2->Attrs["address"]->value is the address
int tcpLink(ostream& os, T2 *t2, void *data)
{
  int sock;
  struct sockaddr_in server;
  struct hostent *serverh;
  int optval = 1;
  // TODO handle errors here
  if(t2->Attrs.find("port") == t2->Attrs.end())
    return -1;
  if(t2->Attrs.find("address") == t2->Attrs.end())
    return -1;

  t2Sock *t2s = new t2Sock;
  memset((void *)t2s, 0, sizeof(*t2s));           /* Clear struct */

  string pstring;
  string astring;
  int port;

  pstring = t2->Attrs["port"]->value;
  port=atoi(pstring.c_str());
  astring = t2->Attrs["address"]->value;

  os << " Running Link to ["<< astring<<":"<<port<<"] \n";

  /* Create the TCP socket */
  if ((t2s->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    myDie("Failed to create socket");
  }
  /* set the reuse addr option */
  setsockopt(t2s->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* Construct the server sockaddr_in structure */
  memset(&t2s->server, 0, sizeof(server));           /* Clear struct */
  t2s->server.sin_family = AF_INET;                  /* Internet/IP */
  t2s->server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  t2s->server.sin_port = htons(port);                /* server port */
           
  if (astring.size() > 0)
    {
      /* gethostbyname: get the server's DNS entry */
      t2s->serverh = gethostbyname((const char *)astring.c_str());
      if (t2s->serverh == NULL) {
	os << "ERROR, no such host as ["<<astring<<"]\n";
	return -1;
      }
      
      /* build the server's Internet address */
      bzero((char *) &t2s->client, sizeof(t2s->client));
      t2s->client.sin_family = AF_INET;
      bcopy((char *)t2s->serverh->h_addr, 
	    (char *)&t2s->client.sin_addr.s_addr, t2s->serverh->h_length);
      t2s->client.sin_port = htons(port);
      
      /* connect: create a connection with the server */
      if (connect(t2s->sock, (struct sockaddr *)&t2s->client, sizeof(t2s->client)) < 0) 
	{
	  os << "ERROR, connecting to  host ["<<astring<<"]\n";
	  return 0;
	}
      t2->t2Sock = (void *)t2s;
    }

  // you can now send and receive data on t2Sock  
  return 0;
}

int tcpScan(ostream& os, T2 *t2, void *data)
{
  os << "tcpAction: scanning ["<<t2->name<<"] \n";
  tcpServ(os, t2, data);
  return 0;
}

int tcpGet(ostream& os, T2 *t2, void *data)
{
  os << "tcpAction: tcp Get ["<<t2->name<<"] \n";
  return 0;
}

int tcpSet(ostream& os, T2 *t2, void *data)
{
  os << "tcpAction: Set ["<<t2->name<<"] \n";
  return 0;
}

int tcpShow(ostream& os, T2 *t2, void *data)
{
  os << "tcpAction: Show ["<<t2->name<<"] \n";
  os << t2;
  return 0;
}

extern "C"
{
  int setup(ostream &os, T2 *t2 , void *data)
  {
    t2->AddAction("!tserv", (void*)tcpServ);
    t2->AddAction("!scan",  (void*)tcpScan);
    t2->AddAction("!get",   (void*)tcpGet);
    t2->AddAction("!set",   (void *)tcpSet);
    t2->AddAction("!show",  (void *)tcpShow);
    t2->SetAttr((string)"?port=5566");
    return 0;
  } 
}
