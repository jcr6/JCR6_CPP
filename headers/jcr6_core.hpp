// Lic:
//   jcr6_core.hpp
//   JCR6 for C++ -- Core Header
//   version: 19.02.20
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

namespace jcr6{

     // const std::string &JCR_Error; The clean way can't work, eh?
     #DEFINE JCR_Error jcr6::Get_JCR_Error_Message()

     class JT_Entry {
     private:
     public:
       std::string MainFile;
       std::map <std::string,bool> CFG_Bool;
       std::map <std::string,int> CFG_Int;
       std::map <std::string,std::string> CFG_String;
     };

     class JT_Dir {
     private:
       std::map <std::string,JT_Entry> EntryMap;
     public:
       std::map <std::string,std::string> Config;
       std::map <std::string,std::string> Comments;
       // methods
       std::map <std::string,JT_Entry> &Entries(); // Get all Entries
       JT_Entry *CreateEntry(std::string Name); // Creates a new entry with 'Name' as a name. Please note as JCR6 is case INSENSITIVE, these names will always be converted into caps. A pointer to the created entry is returned.
     };

     typedef struct {
       std::string Name;
       (*bool) Recognize(std::File);
       (*JT_Dir) Dir(std::File);
     } JD_DirDriver;
     void RegisterDirDriver(JC_DirDriver Driver);

     typedef struct {
       std::string Name;
       (*int) Compress(char *Uncompressed,char *Compressed, int size_uncompressed); // This function will need to do the compression and return the size of the compressed data, or -1 if something went wrong.
       (*bool) Expand(char *Compressed, char *UnCompressed, int size_uncompressed, int size_compressed); // This function will expand. The size_compressed parameter will check if the expanded data is indeed as long as we wanted. Will return 'true' if succesful, and 'false' if failed.
     } JC_CompressDriver;
     void RegisterCompressDriver(JC_CompressDriver);

}


#endif