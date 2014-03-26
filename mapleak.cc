
#include <iostream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

typedef struct aClass {
  string name;
  string address;
}aClass;


map<string, aClass*> * cpMap;

void someFunc(string &str, ostringstream &out)
{
    ostringstream ss;
    ss << out.rdbuf();
    str = ss.str();
    cout <<"somefunc str={"<<str<<"}\n";
}


int main(int argc, char * argv[])
{

  aClass * c1;
  cpMap = new map<string, aClass*>;
  c1 = new aClass;
  (*cpMap)["1"] = c1;
  (*cpMap)["2"] = new aClass;
  (*cpMap)["1"] = new aClass;
    if(!(*cpMap)["3"])
  {
    cout << " No value form cpmap3\n";
  }
#if 0
  string ss;
  ostringstream os;

  os << "hi threre 1\n";
  someFunc(ss, os);
  os << "hi threre 2\n";
  string str=os.str();
  cout << "\nss is ["<<ss<<"]\n";;
  cout << "\nstr is ["<<str<<"] \n";
  cout << "\nos.str is ["<<os.str()<<"] \n";
#endif
  
  delete (*cpMap)["1"];
  delete (*cpMap)["2"];
  delete c1;
  cpMap->clear();
  delete cpMap;
  return 0;

}
