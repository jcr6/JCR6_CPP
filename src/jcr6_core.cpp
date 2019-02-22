// Lic:
//   jcr6_core.cpp
//   JCR6 for C++ -- Code code
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

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include "../headers/jcr6_core.hpp"


#undef DEBUGCHAT
static void chat(std::vector<std::string> args){
  #ifdef DEBUGCHAT
  std::cout << "DEBUG: ";
  for (std::string a:args) std::cout << a;
  std::cout << "\n";
  #endif
}

#undef JCRDEBUGPATHSPLIT
static std::string ExtractDir(const std::string& str)
{
  size_t found;
  #ifdef JCRDEBUGPATHSPLIT
  cout << "Splitting: " << str << endl;
  #endif
  found=str.find_last_of("/\\");
  #ifdef JCRDEBUGPATHSPLIT
  cout << " folder: " << str.substr(0,found) << endl;
  cout << " file: " << str.substr(found+1) << endl;
  #endif
  return str.substr(0,found);
}

static inline bool FileExists(const char *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

static std::string Left(std::string mstr,int pos=1){ return mstr.substr(0,pos); } // BASIC style Left function


typedef union {
  unsigned char ec_byte;
  char ec_char;
  int ec_int;
  long ec_long;
  char ec_reverse[10];
} uEndianCheckUp;
bool LittleEndian(){
  static bool checked{false};
  static bool ret{false};
  if (checked) return ret;
  uEndianCheckUp e;
  e.ec_int = 20;
  ret = e.ec_byte>0; // In big Endian the Byte would always be 0
  #ifdef DEBUGCHAT
  if (ret) { chat({"System has been detected as: LittleEndian"});} else {chat({"System has been detected as BigEndian"});}
  #endif
  return ret;
}

// Please note! I only use LittleEndian computers myself, so I really don't
// know if this works, as I have no way to test this.
template <typename ecconv> ecconv EndianConvert(ecconv num,bool force=false){
  if (LittleEndian() && !force) return num;
  uEndianCheckUp a1;
  uEndianCheckUp a2;
  switch (sizeof(ecconv)){
    case 4:
      a1.ec_int = num; break;
    case 8:
      a1.ec_long = num; break;
    default:
      std::cout << "FATAL ERROR! I do not know how to handle size " << sizeof(ecconv) << "!\n";
      exit(2);
  }

  for (int i=0;i<sizeof(ecconv);++i ){
    a2.ec_reverse[sizeof(ecconv)-(i+1)] = a1.ec_reverse[i];
  }
  switch (sizeof(ecconv)){
    case 4: return a2.ec_int;
    case 8: return a2.ec_long;
    default: return num;
  }

}

namespace jcr6is{ // JCR6 internal stream routines.
  int ReadInt(std::ifstream &bt){
    uEndianCheckUp i;
    for (int j=0; j<4; ++j) bt.read(&(i.ec_reverse[j]),1);
    return EndianConvert(i.ec_int);
  }
  long ReadLong(std::ifstream &bt){
    uEndianCheckUp i;
    for (int j=0; j<8; ++j) bt.read(&(i.ec_reverse[j]),1);
    return EndianConvert(i.ec_long);
  }
  unsigned char ReadByte(std::ifstream &bt){
    uEndianCheckUp c;
    bt.read(&c.ec_char,1);
    return c.ec_byte;
  }
  // Please note, JCR6 does not care about null-termination, yet C++ does.
  // Normally this should not lead to trouble, but in theory it can.
  std::string ReadString(std::ifstream &bt,int length=0){
    char c{0};
    std::string ret = "";
    int l=length;
    if (!l) l = ReadInt(bt);
    for (int i=0;i<l;++i){
      bt.read(&c,1);
      ret += c;
    }
    return ret;
  }

}



// Just to use by JCR6, as I did not want to rely on "boost".
// There is not guarantee all compilers support that, and this way I'm safe.
static std::string Upper(std::string strToConvert)
{
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

// I know there might be better routines for this out there, but I wanted JCR6 to be as "self-reliant" as possible.
class mybankstream {
private:
  char *buf;
  int bufsize;
public:
  int Position = 0;
  char *pointme() { return buf; }
  int getsize() { return bufsize; }
  bool eof() { return Position >= bufsize; }

  unsigned char ReadByte() {
    assert((!eof() && "End of buffer reached!"));
    uEndianCheckUp c;
    c.ec_char = buf[Position];
    Position++;
    return c.ec_byte;
  }

  char ReadChar() {
    assert((!eof() && "End of buffer reached!"));
    char c = buf[Position];
    Position++;
    return c;

  }

  bool ReadBool() { return ReadByte()!=0; }

  int ReadInt() {
    uEndianCheckUp ret;
    for (int i=0; i<4; i++) ret.ec_reverse[i] = ReadByte();
    return EndianConvert(ret.ec_int);
  }

  long ReadLong() {
    uEndianCheckUp ret;
    for (int i=0; i<8; i++) ret.ec_reverse[i] = ReadByte();
    return EndianConvert(ret.ec_long);
  }

  std::string ReadString(int l=0){
    int l2{l};
    std::string ret = "";
    if (!l2) l2 = ReadInt();
    for (int i=0;i<l2;i++) ret+=ReadChar();
    return ret;
  }


  mybankstream(int size){ buf = new unsigned char[size]; bufsize=size; }
  ~mybankstream() { delete buf;}
};


namespace jcr6 {

   // Prologue: Declarations
   static std::string JAMJCR_Error;
   static bool JCR_ErrorCrash { false };
   void SetErrorCrash(bool value) { JCR_ErrorCrash=value;}


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

   void JT_Dir::PatchFile(std::string file){
     // code comes later!
   }


   /*
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
   */




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

   static bool J6_Recognize(std::string file){
     bool ret{true};
     char x;
     const char head[6] = "JCR6\032";
     std::ifstream bt;
     bt.open (file, std::ios::binary);
     for (int i=0;i<5;i++) {
         bt.read(&x,1);
         ret = ret && x==head[i];
         chat({"Reading file;  pos",std::to_string(i)," char",std::to_string((int)x)," result",std::to_string(ret)});
     }
     bt.close();
     return ret;
   }

   static JT_Dir J6_Dir(std::string file){
     const char head[6] = "JCR6\032";
     JT_Dir ret;
     std::ifstream bt;
     bt.open (file, std::ios::binary);
     {
       char x;
       bool isj = true;
       for (int i=0;i<5;i++) {
         bt.read(&x,1);
         isj = isj && x==head[i];
       }
       if (!isj) { JamError("JCR6 Header error"); bt.close(); return ret; } // Now this is a safety precaution, as it should never be possible this error pops up.
     }
     ret.FT_offset = jcr6is::ReadInt(bt);
     // Read Startup Config
     // Please note I just copied this code from the C# version, and I
     // manually converted it to C++, so this may not be the most beautiful code. :P
     if (ret.FT_offset <= 0) {
       JamError("Invalid FAT offset. Maybe you are trying to read a JCR6 file that has never been properly finalized");
       bt.close();
       return ret;
     }
     unsigned char TTag = 0;
     std::string Tag = "";
     do {
       TTag = jcr6is::ReadByte(bt);
       if (TTag != 255) { Tag = jcr6is::ReadString(bt); }
       switch (TTag) {
         case 1:
          ret.Config_string[Tag] = jcr6is::ReadString(bt);
          break;
         case 2:
          ret.Config_bool[Tag] = jcr6is::ReadByte(bt) == 1;
          break;
         case 3:
          ret.Config_int[Tag] = jcr6is::ReadInt(bt);
          break;
         case 255:
          break;
         default:
          std::string e = "Invalid config tag ("; e+=std::to_string(TTag); e+")"; e+=file;
          bt.close();
         return ret;
       }
     } while (TTag != 255);
     if (ret.Config_bool.count("_CaseSensitive") && ret.Config_bool["_CaseSensitive"]) {
       JamError("Case Sensitive dir support was already deprecated and removed from JCR6 before it went to the Go language. It's only obvious that support for this was never implemented in C++ in the first place.");
       bt.close();
       return ret;
     }
     // read File Table Header (how big is it, and how is it compress (if in fact it is)).
     bt.seekg( ret.FT_offset, bt.beg );
     bool theend    = false;
     ret.FT_size    = jcr6is::ReadInt(bt);
     ret.FT_csize   = jcr6is::ReadInt(bt);
     ret.FT_storage = jcr6is::ReadString(bt);
     mybankstream dirbank(ret.FT_size);
     mybankstream cmpbank(ret.FT_csize);
     char *dirbuf   = dirbank.pointme();
     bt.read(dirbuf,ret_FT_csize);
     bt.close();
     if (!CompDrivers.count(ret.FT_storage)) {
       std::string e = "Unknown File Table Compression Method: "; e+=ret.FT_storage;
       JamError();
       return ret;
     }
     CompDrivers[ret.FT_storage].Expand(cmpbank,dirbank,ret.FT_csize,ret.FT_size);
     // Time to actually READ everything
     // This code too was originally written in C# and manually converted to C++
     while ((!dirbank.eof()) && (!theend)) {
       auto mtag = dirbank.ReadByte();
       auto ppp  = dirbank.Position;
       switch (mtag) {
         case 0xff:
              theend = true;
              break;
         case 0x01:
         std::string tag = Upper(dirbank.ReadString());
         switch (tag) {

           case "FILE":    // Atom's identing is horrible, but as I'm still on Mac, this was the quickest way to get on the road (Maybe I should just have used Geanny) :-/
           JT_Entry newentry;
           newentry.MainFile = file;
           auto ftag = dirbank.ReadByte();
           while (ftag != 255) {
             switch (ftag) {

               case 1: // string
               auto k = dirbank.ReadString();
               auto v = dirbank.ReadString();
               newentry.dataString[k] = v;
               break;

               case 2: // Boolean
               auto kb = dirbank.ReadString();
               auto vb = dirbank.ReadBoolean();
               newentry.databool[kb] = vb;
               break;

               case 3: // Integer
               auto ki = dirbank.ReadString();
               auto vi = dirbank.ReadInt();
               newentry.dataint[ki] = vi;
               break;

               case 255: // the end
               break;

               default: // error
               std::string er = "Illegal tag in FILE part "; er += std::to_string(ftag); er += "on fatpos "; er += std::to_string(dirbank.Position);
               JamError (er);
               return ret;
             }
             ftag = dirbank.ReadByte();
           }
           auto centry = newentry.Entry.ToUpper();
           ret.Entries[centry] = newentry;
           break;

           case "COMMENT":
           std::string commentname = dirbank.ReadString();
           ret.Comments[commentname] = dirbank.ReadString();
           break;

           case "IMPORT":
           case "REQUIRE":
           auto deptag = dirbank.ReadByte();
           std::string depk;
           std::string depv;
           std::map<std::string,std::string> depm //= new Dictionary<string, string>();
           while (deptag != 255) {
             depk = dirbank.ReadString();
             depv = dirbank.ReadString();
             depm[depk] = depv;
             deptag = dirbank.ReadByte();
           }
           auto depfile = depm["File"];
           //depsig   := depm["Signature"]
           auto deppatha = depm.count("AllowPath") && depm["AllowPath"] == "TRUE";
           auto depcall = "";
           // var depgetpaths[2][] string
           vector<string> depgetpaths[2]; // List<string>[] depgetpaths = new List<string>[2];
           // not needed in C++                    depgetpaths[0] = new List<string>();
           // not needed in C++                    depgetpaths[1] = new List<string>();
           auto owndir = ExtractDir(file); //Path.GetDirectoryName(file);
           int deppath = 0;
           if (deppatha) {
             deppath = 1;
           }
           if (owndir != "") { owndir += "/"; }
           depgetpaths[0].push_back(owndir);
           depgetpaths[1].push_back(owndir);
           // TODO: JCR6: depgetpaths[1] = append(depgetpaths[1], dirry.Dirry("$AppData$/JCR6/Dependencies/") )
           if (Left(depfile, 1) != "/" && Left(depfile, 2) != ":") {
             for (std::string depdir : depgetpaths[deppath]) {
               if ((depcall == "") && FileExists(depdir + depfile)) {
                 depcall = depdir + depfile;
               }
             }
           } else {
             if (FileExists(depfile)) {
               depcall = depfile;
             }
           }
           if (depcall != "") {
             ret.PatchFile(depcall);
             if (JAMJCR_Error != "" && JAMJCR_Error != "Ok" && tag == "REQUIRE") { //((!ret.PatchFile(depcall)) && tag=="REQUIRE"){
               std::string JERROR = "Required JCR6 addon file (";
               JERROR += depcall
               JERROR +=") could not imported! Importer reported: "
               JERROR += JAMJCR_Error; //,fil,"N/A","JCR 6 Driver: Dir()")
               JamError(JERROR);
               //bt.Close();
               return ret;
             } else if (tag == "REQUIRE") {
               std::string JERROR = "Required JCR6 addon file (";
               JERROR += depcall ;
               JERROR += ") could not found!"; //,fil,"N/A","JCR 6 Driver: Dir()")
               // bt.Close();
               return ret;
             }
           }
           break;
         }
         break;
         default:
         {
             std::string JERROR = "Unknown main tag ";
             JERROR += mtag;
             JERROR += ", at file table position ";
             JERROR += to_string(dirbank.Position);
             JamError(JERROR);
             //bt.Close();
             return ret;
           }
         }
       }
       return ret;
     }

   std::string Recognize(std::string file){
     JAMJCR_Error = "Ok";
     for (auto& kv : DirDrivers) { // I'm still keeping to C++ 11 for now...
           //std::cout << kv.first << " has value " << kv.second << std::endl;
           chat({"Checking: ",kv.second.Name,"/",kv.first,"; "});
           if (kv.second.Recognize(file)) {
             return kv.second.Name;
             chat({"Cool!"});
           } else {
             chat({"Nope, not correct!"});
           }
     }
     return ""; // Empty string means unrecognized.... BOO!
   }

   JT_Dir Dir(std::string file){
     JT_Dir retD; // return Dir
     std::string rec = Recognize(file);
     if (rec=="") { JamError("File not recognized by any Dir Drivers."); return retD; }
     retD = DirDrivers[rec].Dir(file);
     return retD;
   }


   // Chapter 2: Writing

   // Epilogue: InitJCR6
   void init_JCR6(){
     chat({"Init JCR6"});
     JC_CompressDriver comp;
     comp.Compress=FakeStore;
     comp.Expand=FakeRestore;
     comp.Name="Store";
     JD_DirDriver dir;
     dir.Recognize = J6_Recognize;
     dir.Dir = J6_Dir;
     dir.Name="JCR6";
     RegisterDirDriver (dir);
     RegisterCompressDriver (comp);
   }
}
