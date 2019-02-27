// Lic:
//   jcr6_quakepak.cpp
//   JCR6 for C++ -- QuakePack Driver
//   version: 19.02.27
//   Copyright (C) 2019 Jeroen P. Broks
//   This software is provided 'as-is', without any express or implied
//   warranty.  In no event will the authors be held liable for any damages
//   arising from the use of this software.
//   Permission is granted to anyone to use this software for any purpose,
//   including commercial applications, and to alter it and redistribute it
//   freely, subject to the following restrictions:
//   1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//   2. Altered source versions must be plainly marked as such, and must not be
//      misrepresented as being the original software.
//   3. This notice may not be removed or altered from any source distribution.
// EndLic

#include <string>
#include <fstream>
#include <iostream>
#include "../headers/jcr6_quakepak.hpp"

namespace jcrquake{
typedef union {
  char buf[5];
  unsigned char ch[5];
  int  toint;
} TBUF;

inline bool CHK_E(){
  static bool checked=false;
  static bool LittleEndian=false;
  if (checked) return LittleEndian;
  TBUF a;
  a.toint=20;
  LittleEndian=a.buf[0]>0;
  if (!LittleEndian) std::cout<< "WAD: WARNING! Big endian system detected! Proper working cannot be guaranteed!\n";
  return LittleEndian;
}
#define QLittleEndian CHK_E()

int rint(std::ifstream &bt){
  TBUF ret;
  //bt.read(ret.buf,4);
  for (int j=0; j<4; ++j) {
    char x;
    bt.read(&x,1); //&(ret.buf[j]),1);
    ret.buf[j]=x;
    //chat(j);
    //chat(x);
    //chat(std::to_string(ret.ch[j]));
  }
  if (!QLittleEndian) {
    TBUF ret2;
    for (int i=0;i<4;++i) ret2.buf[3-i]=ret.buf[i];
    //chat("Big Endian:");
    //chat(ret2.toint);
    return ret2.toint;
  }
  //chat(ret.toint);
  return ret.toint;
}
}

bool QuakeRec(std::string file){
  char chead[5] = "PACK";
  char rhead[5] = "....";
  bool ret{true};
  std::ifstream bt;
  bt.open(file,std::ios::binary);
  bt.read(rhead,4);
  for (char i=1;i<5;i++) ret=ret&&chead[i]==rhead[i];
  bt.close();
  return ret;
}

jcr6::JT_Dir QuakeDir(std::string file){
  using namespace jcr6;
  using namespace std;
  using namespace jcrquake;
  char chead[5] = "PACK";
  char rhead[5] = "....";
  bool chk{true};
  ifstream bt;
  JT_Dir ret;
  bt.open(file,std::ios::binary);
  bt.read(rhead,4);
  //for (char i=1;i<5;i++) ret=ret&&chead[i]==rhead[i];
  auto DirOffset = rint(bt);
  auto DirSize = rint(bt);
  ret.FT_offset=DirOffset;
  ret.FT_csize=DirSize;
  ret.FT_size=DirSize;
  ret.FT_storage="Store";
  bt.seekg(DirOffset);
  for (int Ak = 0; Ak < DirSize; Ak += 64){
    JT_Entry e;
    e.MainFile=file;
    char EntryName[57];
    bt.read(EntryName,56);
    e.dataString["__Entry"]   = EntryName;
    e.dataInt   ["__Offset"]  = rint(bt);
    e.dataInt   ["__CSize"]   = rint(bt);
    e.dataInt   ["__Size"]    = e.dataInt["__CSize"];
    e.dataString["__Storage"] = "Store";
    e.dataString["__Notes"]   = "";
    e.dataString["__Author"]  = "";
    ret.AddEntry(EntryName,e);
  }
  bt.close();
  return ret;
}

namespace jcr6{
  void init_quakepak(){
    JD_DirDriver Q;
    Q.Recognize = QuakeRec;
    Q.Dir = QuakeDir;
    Q.Name = "Quake Pack";
    jcr6::RegisterDirDriver(Q);
  }
}
