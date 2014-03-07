// string::find_first_of
#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>        // std::size_t
#include <map>

using namespace std;

#ifndef __STRINGS_H
#define __STRINGS_H

typedef map< char,string>sMap;

int DecodeDelims(sMap& sMap, const string &delims, const string& sin);
int DecodeName(string &sname, string &slib, string &sact, string &satr, const string& sin);
void oldTestDecodeName(const string &sin);

void TestDecodeName(const string &sin);

#endif
