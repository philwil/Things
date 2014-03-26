
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void someFunc(string &str, ostringstream &out)
{
    ostringstream ss;
    ss << out.rdbuf();
    str = ss.str();
    cout <<"somefunc str={"<<str<<"}\n";
}


int main(int argc, char * argv[])
{
  string ss;
  ostringstream os;

  os << "hi threre 1\n";
  someFunc(ss, os);
  os << "hi threre 2\n";
  string str=os.str();
  cout << "\nss is ["<<ss<<"]\n";;
  cout << "\nstr is ["<<str<<"] \n";
  cout << "\nos.str is ["<<os.str()<<"] \n";

  

  return 0;

}
