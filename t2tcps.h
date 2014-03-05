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
#ifndef __T2TCPS_H
#define __T2TCPS_H

#include <iostream>
#include <limits>
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
using namespace std;

#include "T2.h"
#include "Socket.h"

 
typedef struct t2Sock 
{
  int sock;
  pthread_t thr;
  T2 *t2;
  unsigned int clientlen;
  struct sockaddr_in client;
  struct sockaddr_in server;
  struct hostent *serverh;
} t2Sock;

#endif
