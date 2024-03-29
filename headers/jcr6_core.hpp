// Lic:
// headers/jcr6_core.hpp
// JCR6
// version: 23.09.04
// Copyright (C) 2019, 2020, 2021, 2023 Jeroen P. Broks
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

/*

	 This file contains all the basic functions and classes and structs and
	 whatever to access a JCR6 file. When only this header is included (with
	 the corresponding jcr6_core.cpp file in the src folder of course) JCR6
	 will then support non-compressed JCR6 files only. Support for any sort of
	 compression support seperate driver files will need to be included as well.
	 The JCR6.hpp header, this this project's main directory will select the
	 most obvious ones for you.

	 When it comes to reading alternate file formats, those are not included by
	 JCR6.hpp, but should always be included separately.

*/

#ifndef DEFINED_JCR6CORE
#define DEFINED_JCR6CORE



#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <memory>


namespace jcr6{

	 // const std::string &JCR_Error; The clean way can't work, eh?
	 #define JCR_Error jcr6::Get_JCR_Error_Message()
	 std::string Get_JCR_Error_Message();
	 void JamError(std::string errormessage);
	 void SetErrorCrash(bool value);
	 extern void (*JCRPanic)(std::string errormessage);

	 class JT_Block {
	 private:
	 public:
		 int ID{ 0 };
		 std::string MainFile;
		 std::map <std::string, bool> dataBool;
		 std::map <std::string, int> dataInt;
		 std::map <std::string, std::string> dataString;
		 int CompressedSize();
		 int RealSize();
		 int Offset();
	 };

	 class JT_Entry {
	 private:
	 public:
	   std::string MainFile;
	   std::map <std::string,bool> dataBool;
	   std::map <std::string,int> dataInt;
	   std::map <std::string,std::string> dataString;
	   std::string Entry();
	   int CompressedSize();
	   int RealSize();
	   int Offset();
	   int Block();
	   void SAN(std::string author, std::string Notes = "");
	 };

	 class JT_EntryReader {
	 private:
	   char *buf;
	   int bufsize;
	 public:
	   int Position = 0;
	   char *pointme();
	   int getsize();
	   unsigned char ReadByte();
	   char ReadChar();
	   int ReadInt();
	   int ReadInt16();
	   long ReadLong();
	   long long ReadLong64();
	   bool ReadBool();
	   std::string ReadLine();
	   std::string ReadString(int l=0);
	   unsigned int ReadUInt32();
	   unsigned short ReadUInt16();
	   bool eof();
	   void newbuf(int size);
	   std::string BufAsString(bool safe = true);
	   JT_EntryReader(int size=1);
	   ~JT_EntryReader();
	 };


	 class JT_Dir {
	 private:
		 std::map <std::string, JT_Entry> EntryMap;
		 std::map <std::string, JT_Block> BlockMap;
		 static char* _LastBlockBuf;
		 static std::string _LastBlock;
	 public:
		 static void FlushBlock();
		 int FT_offset{ 0 };
		 int FT_size{ 0 };
		 int FT_csize{ 0 };
		 std::string FT_storage{ "" };
		 std::map <std::string, std::string> Config_string;
		 std::map <std::string, int>         Config_int;
		 std::map <std::string, bool>        Config_bool;
		 std::map <std::string, std::string> Comments;
		 // methods
		 std::map <std::string, JT_Entry>& Entries(); // Get all Entries
		 //JT_Entry *CreateEntry(std::string Name); // Creates a new entry with 'Name' as a name. Please note as JCR6 is case INSENSITIVE, these names will always be converted into caps. A pointer to the created entry is returned.
		 void PatchFile(std::string file);
		 void PatchDir(JT_Dir& dir);
		 void PatchDir(JT_Dir& dir, std::string prefix);
		 void AddEntry(std::string name, JT_Entry Entry);
		 void AddBlock(int a, JT_Block b);
		 JT_Entry& Entry(std::string entry);
		 void B(std::string entry, JT_EntryReader& data); // Reads an entry from a JCR file and returns it as a bankstream.
		 std::shared_ptr<JT_EntryReader> B(std::string entry);
		 std::vector<std::string> Lines(std::string entry);
		 std::string String(std::string entry);
		 std::map<std::string, std::string> StringMap(std::string entry);
		 std::vector<char> Characters(std::string entry);
		 bool EntryExists(std::string name);
		 bool DirectoryExists(std::string dir);
	 };

	 class JT_Create;

	 class JT_CreateBuf {
	 private: 
		 JT_Create* parent;
		 std::vector<char> buf;
		 std::string EntryName;
		 std::string Storage;
		 bool closed = false;
	 public:
		 JT_CreateBuf(JT_Create* parent, std::string Entry,std::string aStorage="Store");
		 void Write(char C);
		 void Write(unsigned char C);
		 void Write(unsigned short C);
		 void Write(int C);
		 void Write(unsigned int C);
		 void Write(long long C);
		 void Write(std::string S,bool raw=false);
		 JT_Entry Close(bool autodelete=false);
		 //std::vector<char> *GetBuf(); // Only use this when you know what you are doing!
		 std::string BufAsString(bool safe=true);
		 ~JT_CreateBuf();
	 };

	 class JT_Create{
	 private:
	   std::map<std::string,std::string> Comments;
	   std::map<std::string,JT_Entry>    Entries;
	   std::map<std::string,std::string> nDataString;
	   std::map<std::string,int>         nDataInt;
	   std::map<std::string,bool>        nDataBool;
	   std::string FT_storage;
	   std::string MainFile;
	   std::map<std::string,std::string> ConfigString;
	   std::map<std::string,int>         ConfigInt;
	   std::map<std::string,bool>        ConfigBool;
	   int offsetoffset; // Sounds silly, but it's just a precaution to make sure the offset integer is written on the right spot.
	   bool closed{false};
	   bool entryadded{false}; // Once true the config add routines should no longer work!
	   // std::ofstream bt;
	   FILE * bt; // Otherwise the compiler bugs me!
				  // As the C++ devs why, as it doesn't make sense to me either!

	 public:
	   std::map <std::string,std::string> LastResult;
	   JT_Create(std::string file,std::string storage="Store");
	   ~JT_Create();
	   void Close();
	   void AddConfig(std::string key,std::string value);
	   void AddConfig(std::string key,int value);
	   void AddConfig(std::string key,bool value);
	   JT_Entry AddBuff(std::string entryname,std::string storage,char * buffer,int size, std::string author="",std::string notes="", bool dataclearnext = true);
	   JT_Entry AddFile(std::string filename, std::string entryname, std::string storage="Store", std::string author="", std::string notes = "", bool dataclearnext = true);
	   JT_Entry AddString(std::string entryname,std::string str,std::string storage="Store", std::string author = "", std::string notes = "", bool dataclearnext=true);
	   JT_Entry AddStringMap(std::string entryname, std::map<std::string, std::string> map, std::string storage = "Store", bool dataclearnext = true);
	   JT_Entry AddCharacters(std::string entryname, std::vector<char> chars, std::string storage = "Store", bool dataclearnext = true);
	   JT_CreateBuf* StartEntry(std::string entry, std::string storage = "Store");

	   void Import(std::string dependency);
	   void Require(std::string dependency);
	   void AddComment(std::string namecomment,std::string contentcomment);
	   void Alias(std::string original,std::string target);
	 };




	 typedef struct {
	   std::string Name;
	   bool (*Recognize)(std::string File);
	   JT_Dir (*Dir)(std::string File);
	 } JD_DirDriver;
	 void RegisterDirDriver(JD_DirDriver Driver);

	 typedef struct {
	   std::string Name;
	   int ( * Compress ) (char *Uncompressed,char *Compressed, int size_uncompressed); // This function will need to do the compression and return the size of the compressed data, or -1 if something went wrong.
	   bool ( * Expand )(char *Compressed, char *UnCompressed, int size_compressed, int size_uncompressed); // This function will expand. The size_compressed parameter will check if the expanded data is indeed as long as we wanted. Will return 'true' if succesful, and 'false' if failed.
	 } JC_CompressDriver;
	 void RegisterCompressDriver(JC_CompressDriver Driver);

	 std::string Recognize(std::string file);

	 void init_JCR6();
	 JT_Dir Dir(std::string file);


}


#endif