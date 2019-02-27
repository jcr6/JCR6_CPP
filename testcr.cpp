// Lic:
// ***********************************************************
// testcr.cpp
// This particular file has been released in the public domain
// and is therefore free of any restriction. You are allowed
// to credit me as the original author, but this is not 
// required.
// This file was setup/modified in: 
// 2019
// If the law of your country does not support the concept
// of a product being released in the public domain, while
// the original author is still alive, or if his death was
// not longer than 70 years ago, you can deem this file
// "(c) Jeroen Broks - licensed under the CC0 License",
// with basically comes down to the same lack of
// restriction the public domain offers. (YAY!)
// *********************************************************** 
// Version 19.02.27
// EndLic
#include <iostream>
#include "JCR6.hpp"

#define MY_HEAD "JCR6 Test Creation Utility\nCoded by: Jeroen P. Broks\nReleased in the Public Domain\n\n\n"

int main(){
  using namespace std;
  using namespace jcr6;
  init_JCR6();
  std::string myhead = MY_HEAD;
  const char * chead; chead = myhead.c_str();
  char mhead[255]; strcpy(mhead,chead);
  cout << MY_HEAD;
  cout << "Creating JCR\n";
  JT_Create j{"testcr.jcr","Store"};
  j.AddComment("Test","This is just a test JCR file! Does it work?");
  cout << "Storing HEADER\n";
  j.AddBuff("HEAD","Store",mhead,myhead.size());
  cout << "Storing source\n";
  j.AddFile("testcr.cpp","testcr.cpp","Store");
  cout << "Storing random string\n";
  j.AddString("String.txt","The quick brown fox jumps over the lazy dog!");
  cout << "Alias testing\n";
  j.Alias("testcr.cpp","Alias.cpp");
  j.Alias("String.txt","LazyDog.txt");
  cout << "Finalizing\n";
  j.Close();
}
