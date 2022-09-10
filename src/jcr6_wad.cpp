// Lic:
// src/jcr6_wad.cpp
// JCR6
// version: 20.08.22
// Copyright (C) 2019, 2020 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iostream>
#include <algorithm>
#include "../headers/jcr6_wad.hpp"

#define WAD_DEBUGCHAT
#ifdef WAD_DEBUGCHAT
#define chat(message) std::cout << "WAD: " << message << "\n"
#endif
#ifndef WAD_DEBUGCHAT
#define chat(message) //
#endif

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
#define WLittleEndian CHK_E()

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
  if (!WLittleEndian) {
	TBUF ret2;
	for (int i=0;i<4;++i) ret2.buf[3-i]=ret.buf[i];
	//chat("Big Endian:");
	//chat(ret2.toint);
	return ret2.toint;
  }
  //chat(ret.toint);
  return ret.toint;
}


bool RecognizeWAD(std::string file){
  char head[5]; head[4]='\0';
  std::ifstream bt;
  bt.open (file, std::ios::binary);
  bt.read(head,4);
  bt.close();
  if (strcmp(head,"IWAD")==0 || strcmp(head,"PWAD")==0) return true;
  return false;
}

jcr6::JT_Dir DirWAD(std::string file){
  using namespace jcr6;
  char head[5]; head[4]='\0';
  char filename[9]; filename[8]=0; // Never forget that strings in C are null-terminated, so an extra char is needed!
  int fsize,foffset;
  JT_Dir ret;
  std::string folderpref{""};
  int aantal{0},offset{0};
  std::map<std::string,std::string> authors;
  const std::string LevelFiles[] = { "THINGS", "LINEDEFS", "SIDEDEFS", "VERTEXES", "SEGS", "SSECTORS", "NODES", "SECTORS", "REJECT", "BLOCKMAP", "BEHAVIOR" };
  authors["DOOM.WAD"]    = "id Software";
  authors["DOOM2.WAD"]   = "id Software";
  authors["HERETIC.WAD"] = "Raven Software";
  authors["HEXEN.WAD"]   = "Raven Software";
  authors["DARKWAR.WAD"] = "Apogee's Developers of Incredible Power";
  std::string sfile{""}; // stripping dir comes later!
  std::ifstream bt;
  {
	  int found=file.find_last_of("/\\");
	  sfile=file.substr(found+1);
	  std::transform(sfile.begin(), sfile.end(), sfile.begin(), ::toupper);
  }
  bt.open (file, std::ios::binary);
  bt.read(head,4);
  if (strcmp(head,"IWAD")!=0 && strcmp(head,"PWAD")!=0) {
	JamError("Not a WAD file!"); // Safety precaution as this should NEVER happen!
	return ret;
  }
  if (strcmp(head,"IWAD")==0) ret.Comments["Internal WAD"] = "Please note that this is an IWAD file\nJCR6 will allow you to view all contents, but always respect the copyrights of the original developers of this game!";
  ret.FT_storage="Store"; // WAD does not support compression, so 'Store' will always have to be the compression method.
						  // Not that it truly matters of course, as I will just do a 'direct read' here, but let's keep it
						  // a convention to work this way, for mess prevention's sake, and also to 'satisfy' listout utilities
						  // which actually show this.
  chat(head);
  aantal=rint(bt);
  offset=rint(bt); ret.FT_offset=offset;
  bt.seekg(offset,std::ios::beg);
  for (int i=0;i<aantal;++i){
	JT_Entry entry;
	foffset = rint(bt);
	fsize = rint(bt);
	bt.read(filename,8);
	entry.MainFile = file;
	if (folderpref!="") {
	  bool found{false};
	  for (auto mentry:LevelFiles) {
		found = found || mentry == filename;
	  }
	  if (!found) folderpref="";
	}
	if ( (filename[0]=='E' && filename[2]=='M' && filename[4]==0) || (filename[0]=='M' && filename[1]=='A' && filename[2]=='P' && filename[5]==0)) {
	  folderpref  = "MAP_";
	  folderpref += filename;
	  folderpref += "/";
	}
	entry.dataInt   ["__Offset"]  = foffset;
	entry.dataInt   ["__CSize"]   = fsize;
	entry.dataInt   ["__Size"]    = fsize;
	entry.dataString["__Entry"]   = folderpref + filename;
	entry.dataString["__Notes"]   = folderpref;
	entry.dataString["__Storage"] = "Store"; // Must be set or bad stuff will happen!
	entry.dataString["__Author"]  = "";
	if (authors.count(sfile)) entry.dataString["__Author"] = authors[sfile];
	ret.AddEntry(entry.dataString["__Entry"],entry);
  }
  bt.close();
  return ret;
}

void jcr6::InitWAD(){
  JD_DirDriver Dir;
  Dir.Recognize=RecognizeWAD;
  Dir.Dir = DirWAD;
  Dir.Name="Where is All the Data?";
  RegisterDirDriver(Dir);
}