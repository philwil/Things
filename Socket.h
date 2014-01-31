#ifndef _TSOCKET_H
#define _TSOCKET_H

#include <string>
#include <pthread.h>
using namespace std;

class sClient
{
 public:

 sClient(int sock) :sock(sock) {};
  ~sClient(){};
 int sock;
 struct sockaddr_in *client;
 pthread_t thr;
};


int socketServer(int port, void (*HandleClient)(int sock, void *data,  struct sockaddr_in *client), 
		 void *data);

int RecvClient(int sock, char *buffer, int len);
int SendClient(int sock, string msg);

#endif
