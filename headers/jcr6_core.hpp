// Lic:
//   jcr6_core.hpp
//   JCR6 for C++ -- Core Header
//   version: 19.02.22
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



namespace jcr6{

     // const std::string &JCR_Error; The clean way can't work, eh?
     #define JCR_Error jcr6::Get_JCR_Error_Message()
     std::string Get_JCR_Error_Message();
     void JamError(std::string errormessage);
     void SetErrorCrash(bool value);

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
     };

     class mybankstream {
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
       long ReadLong();
       bool ReadBool();
       std::string ReadLine();
       std::string ReadString(int l=0);
       bool eof();
       void newbuf(int size);
       mybankstream(int size=1);
       ~mybankstream();
     };


     class JT_Dir {
     private:
       std::map <std::string,JT_Entry> EntryMap;
     public:
       int FT_offset{0};
       int FT_size{0};
       int FT_csize{0};
       std::string FT_storage{""};
       std::map <std::string,std::string> Config_string;
       std::map <std::string,int>         Config_int;
       std::map <std::string,bool>        Config_bool;
       std::map <std::string,std::string> Comments;
       // methods
       std::map <std::string,JT_Entry> &Entries(); // Get all Entries
       //JT_Entry *CreateEntry(std::string Name); // Creates a new entry with 'Name' as a name. Please note as JCR6 is case INSENSITIVE, these names will always be converted into caps. A pointer to the created entry is returned.
       void PatchFile(std::string file);
       void PatchDir(JT_Dir &dir);
       void AddEntry(std::string name,JT_Entry Entry);
       JT_Entry &Entry(std::string entry);
       void B(std::string entry,mybankstream &data); // Reads an entry from a JCR file and returns it as a bankstream.
       std::vector<std::string> Lines(std::string entry);
       std::string String(std::string entry);
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
       JT_Entry AddBuff(std::string entryname,std::string storage,char * buffer,int size, bool dataclearnext=true);
       JT_Entry AddFile(std::string filename, std::string entryname, std::string storage="Store",bool dataclearnext=true);
       void Import(std::string dependency);
       void Require(std::string dependency);
       void AddComment(std::string namecomment,std::string contentcomment);
     };




     typedef struct {
       std::string Name;
       bool (* Recognize)(std::string File);
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
