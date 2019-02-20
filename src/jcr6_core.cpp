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

   static std::map<std::string,JD_DirDriver> JC_DirDrivers;
   void RegisterDirDriver(JC_DirDriver Driver){
     JAMJCR_Error = "Ok";
     // code comes later
   }

   // Chapter 2: Writing

}
