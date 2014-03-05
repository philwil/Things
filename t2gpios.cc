/*******************************************
 **
 ** gpiolib for T2.cc
 **
 ** ./T2 name@port starts a service called name 
 **  on port 1234 
 ******************************************/

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
#include <unistd.h>

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


int gpioScan(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: scanning ["<<t2->name<<"] \n";
  return 0;
}

int gpioGet(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: gpio Get ["<<t2->name<<"] \n";
  return 0;
}

int gpioSet(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: Set ["<<t2->name<<"] \n";
  return 0;
}

int gpioShow(ostream& os, T2 *t2, void *data)
{
  os << "gpioAction: Show ["<<t2->name<<"] \n";
  os << t2;
  return 0;
}

extern "C"
{
  int setup(ostream &os, T2 *t2 , void *data)
{
    t2->AddAction("scan", (void*)gpioScan);
    t2->AddAction("get", (void*)gpioGet);
    t2->AddAction("set", (void *)gpioSet);
    t2->AddAction("show", (void *)gpioShow);
    t2->SetAttrs((string)"?pin=1&dir=input&value=on");
    //   t2->AddKid((string)"gpio_1", (string)"?pin=1&dir=input");
    //t2->AddKid((string)"gpio_2", (string)"?pin=2&dir=input");
    //t2->AddKid((string)"gpio_3", (string)"?pin=3&dir=input");
    //os<<t2; 
   return 0;
} 
}
