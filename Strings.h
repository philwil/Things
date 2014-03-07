// string::find_first_of
#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>        // std::size_t
#include <map>
#include <vector>

using namespace std;

#ifndef __STRINGS_H
#define __STRINGS_H

typedef map< char,string>sMap;

int DecodeDelims(sMap& sMap, const string &delims, const string& sin);
int DecodeCommand(string &scmd, string &srest, const string &sin);

int DecodeName(string &sname, string &slib, string &sact, string &satr, const string& sin);
void oldTestDecodeName(const string &sin);

void TestDecodeName(const string &sin);
int StringNewName(string& new_name, string &addr, string &port, const string &sin);
int Split(vector<string>&lst, const string& sinput, const string& seps
	  , bool remove_empty);
int SplitString(string &sname, string &attrs, string &remains, string &src);
int DecodeName(string &sname, string &slib, string &sact, string &satr, const string& sin);
int SplitAttr(string &s1, string &s2, const string &sattrs);


#endif
