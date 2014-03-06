// string::find_first_of
#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>        // std::size_t
using namespace std;

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

void TestDecodeName(const string &sin)
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

int main ()
{
 
 string str ("Please, replace the vowels in this sentence by asterisks.");
  size_t found = str.find_first_of("aeiou");
  while (found!=string::npos)
  {
    str[found]='*';
    found=str.find_first_of("aeiou",found+1);
  }

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

  return 0;
}
