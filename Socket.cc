
// modified socket library.
// used to interface to a C++11 main
// g++ -std=c++11 -c socket.cpp
// 
// this is mainly c code but built into a c++ output file.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <string>

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
#include "Socket.h"

using namespace std;
#define MAXPENDING 5    /* Max connection requests */
#define BUFFSIZE 32
void Die(const char *msg) { perror(msg); exit(1); }
    
int socketServer(int port, void (*HandleClient)(int sock, void *data, struct sockaddr_in *client), 
		void *data) 
{
  int sock, clientsock;
  struct sockaddr_in server;
  struct hostent *serverh;
  int optval = 1;

  cout << " Server on  Port " << port << endl; 
  /* Create the TCP socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    Die("Failed to create socket");
  }
  /* set the reuse addr option */
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* Construct the server sockaddr_in structure */
  memset(&server, 0, sizeof(server));       /* Clear struct */
  server.sin_family = AF_INET;                  /* Internet/IP */
  server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  server.sin_port = htons(port);       /* server port */
           

  if (!HandleClient) {
    Die("Must specify client handler");
  }

  /* Bind the server socket */
  if (bind(sock, (struct sockaddr *) &server,
	   sizeof(server)) < 0) {
    cout << " Tried Port " << port << endl; 
    Die("Failed to bind the server socket");
  }
  /* Listen on the server socket */
  if (listen(sock, MAXPENDING) < 0) {
    Die("Failed to listen on server socket");
  }
  
  /* Run until cancelled */
  while (1) {
    struct sockaddr_in *client;
    unsigned int clientlen = sizeof(*client);
    client = (struct sockaddr_in *)malloc(clientlen);
    /* Wait for client connection */
    if ((clientsock =
	 accept(sock, (struct sockaddr *) client,
		&clientlen)) < 0) {
      Die("Failed to accept client connection");
    }
    //    fprintf(stdout, "Client connected: %s\n",
    //	    inet_ntoa(client->sin_addr));
    /* hand over operation to the client call back */
    HandleClient(clientsock, data, client);
  }
  return sock;
}

int connectServer(int port, void *ipname) 
{
  int sock, clientsock;
  struct sockaddr_in server, client;
  struct hostent *serverh;
  int optval = 1;

  cout << " Server on  Port " << port << endl; 
  /* Create the TCP socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    Die("Failed to create socket");
  }
  /* set the reuse addr option */
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* Construct the server sockaddr_in structure */
  memset(&server, 0, sizeof(server));       /* Clear struct */
  server.sin_family = AF_INET;                  /* Internet/IP */
  server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  server.sin_port = htons(port);       /* server port */
           

  if (ipname) {
    /* gethostbyname: get the server's DNS entry */
    serverh = gethostbyname((const char *)ipname);
    if (serverh == NULL) {
      fprintf(stderr,"ERROR, no such host as %s\n", (char *)ipname);
      return 0;
    }
    
    /* build the server's Internet address */
    bzero((char *) &client, sizeof(client));
    client.sin_family = AF_INET;
    bcopy((char *)serverh->h_addr, 
	  (char *)&client.sin_addr.s_addr, serverh->h_length);
    client.sin_port = htons(port);
    
    /* connect: create a connection with the server */
    if (connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0) 
      printf("ERROR connecting");
    
  }
  return sock;
}


// dummy for testing
void HandleClient(int sock, void *data, void * client) {
  char buffer[BUFFSIZE];
  int received = -1;
  /* Receive message */
  if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
    Die("Failed to receive initial bytes from client");
  }
  /* Send bytes and check for more incoming data in loop */
  while (received > 0) {
    /* Send back received data */
    if (send(sock, buffer, received, 0) != received) {
      Die("Failed to send bytes to client");
    }
    /* Check for more data */
    if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
      Die("Failed to receive additional bytes from client");
    }
  }
  close(sock);
}

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


#if defined RUN_MAIN
int main(int argc, char *argv[]) 
{
  int port;
  
  if (argc != 2) {
    fprintf(stderr, "USAGE: server <port>\n");
    exit(1);
  }
  port = atoi(argv[1]);
  socketServer(port, HandleClient, NULL);
  return 0;
}
#endif
 
