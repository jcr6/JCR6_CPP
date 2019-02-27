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
  j.AddBuff("HEAD","zlib",mhead,myhead.size());
  cout << "Storing source\n";
  j.AddFile("testcr.cpp","testcr.cpp","Store");
  cout << "Finalizing\n";
  j.Close();
}
