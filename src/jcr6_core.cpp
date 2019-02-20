// Lic:
//   jcr6_core.cpp
//   JCR6 for C++ -- Code code
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

#include <iostream>
#include "../headers/jcr6_core.hpp"


// Just to use by JCR6, as I did not want to rely on "boost".
// There is not guarantee all compilers support that, and this way I'm safe.
static std::string Upper(std::string strToConvert)
{
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}


namespace jcr6 {

   // Prologue: Declarations
   static std::string JAMJCR_Error;

   std::string Get_JCR_Error_Message() { return JAMJCR_Error; }

   void JamError(std::string errormessage){
     JAMJCR_Error = errormessage;
     if (JCR_ErrorCrash) {
       std::cout << "JCR6 ERROR: " << errormessage;
       exit(1);
     }
   }

   // Chapter 1: Reading
   std::map <std::string,JT_Entry> &JT_Dir::Entries(){
     // code comes later
     return EntryMap;
   }

   JT_Entry *JT_Dir::CreateEntry(std::string Name){
     JAMJCR_Error = "Ok";
     std::string id = Upper(Name);
     // Is there a more elegant solution for this one?
     JT_Entry cr;
     EntryMap[id] = cr;
     // Create pointer
     JT_Entry *ret = &cr;
     // Base config
     ret->CFG_String["__ENTRY"] = Name;
     // return
     return ret;
   }




   static std::map<std::string,JC_CompressDriver> CompDrivers;
   void RegisterCompressDriver(JC_CompressDriver){
     JAMJCR_Error = "Ok";
     // code comes later
   }

   static std::map<std::string,JD_DirDriver> DirDrivers;
   void RegisterDirDriver(JD_DirDriver Driver){
     JAMJCR_Error = "Ok";
     if (DirDrivers.count(Driver.Name)) { JamError("Duplicate directory driver!"); return; }
     DirDrivers[Driver.Name]=Driver;
   }

   static int FakeStore(char * ibuffer,char *obuffer, int size){
      for (int i=0;i<size;++i) obuffer[i]=ibuffer[i]; // Copying the buffer as a whole in stead of just copying the pointer is essential to make sure still will not get deleted when it shouldn't be!
      return size;
   }

   static bool FakeRestore(char *ibuffer, char *obuffer, int size1, int size2){
     FakeStore(ibuffer,obuffer,size2);
     return true;
   }


   // Chapter 2: Writing

   // Epilogue: InitJCR6
   void init_JCR6(){
     JC_CompressDriver comp;
     comp.Compress=FakeStore;
     comp.Expand=FakeRestore;
     comp.Name="Store";
   }
}
