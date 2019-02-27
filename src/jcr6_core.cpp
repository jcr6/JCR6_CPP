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


#define DEBUGCHAT
static inline void chat(std::vector<std::string> args){
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

static inline bool FileExists(std::string fileName) {
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
  checked=true;
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

//  void WriteInt(std::ofstream &bt,int i){
void WriteInt(FILE * bt,int i){
    uEndianCheckUp e;
    e.ec_int = EndianConvert(i);
    for (int i=0; i<4;++i) fputc(e.ec_reverse[i],bt);//bt.write(&(e.ec_reverse[i]),1);
  }

//  void WriteLong(std::ofstream &bt,long i){
  void WriteLong(FILE*bt,long i){
    uEndianCheckUp e;
    e.ec_long = EndianConvert(i);
    for (int i=0; i<8;++i) fputc(e.ec_reverse[i],bt); //bt.write(&(e.ec_reverse[i]),1);
  }

/*
  void WriteRawString(std::ofstream &bt,std::string str){
    for (int i=0; i<str.size();i++) bt.write(&(str[i]),1);
  }
*/
  void WriteRawString(FILE * bt, std::string str){
    for (int i=0; i<str.size();i++) fputc(str[i],bt);
  }

  //void WriteString(std::ofstream &bt,std::string str){
  void WriteString(FILE *bt,std::string str){
    WriteInt(bt,str.size());
    WriteRawString(bt,str);
  }

  //void WriteByte(std::ofstream &bt, unsigned char c){
  void WriteByte(FILE * bt, unsigned char c){
    uEndianCheckUp e;
    e.ec_byte=c;
    //bt.write(&(e.ec_char),1);
    fputc(e.ec_char,bt);
  }

  //void WriteBool(std::ofstream &bt,bool b){
  void WriteBool(FILE * bt,bool b){
    if (b) WriteByte(bt,1); else WriteByte(bt,0);
  }

}



// Just to use by JCR6, as I did not want to rely on "boost".
// There is not guarantee all compilers support that, and this way I'm safe.
static std::string Upper(std::string strToConvert)
{
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

namespace jcr6 {
  static std::map<std::string,JC_CompressDriver> CompDrivers;

// I know there might be better routines for this out there, but I wanted JCR6 to be as "self-reliant" as possible.
  char *mybankstream::pointme() { return buf; }
  int mybankstream::getsize() { return bufsize; }
  bool mybankstream::eof() { return Position >= bufsize; }

  void mybankstream::newbuf(int size){
    delete buf;
    buf = new char[size];
    bufsize = size;
    Position = 0;
    chat({"Buffer in bankstream recreated: ",std::to_string(bufsize)," bytes"});
  }

  unsigned char mybankstream::ReadByte() {
    assert((!eof() && "End of buffer reached!"));
    uEndianCheckUp c;
    c.ec_char = buf[Position];
    #ifdef DEBUGCHAT
    std::cout << "Read byte " << (int)c.ec_byte << " from position " << Position << '\n';
    #endif
    Position++;
    return c.ec_byte;
  }

  char mybankstream::ReadChar() {
    assert((!eof() && "End of buffer reached!"));
    char c = buf[Position];
    #ifdef DEBUGCHAT
    std::cout << "Read char " << (int)c << " from position " << Position << '\n';
    #endif
    Position++;
    return c;

  }

  bool mybankstream::ReadBool() { return ReadByte()!=0; }

  int mybankstream::ReadInt() {
    uEndianCheckUp ret;
    for (int i=0; i<4; i++) ret.ec_reverse[i] = ReadByte();
    #ifdef DEBUGCHAT
    std::cout << "Got int " << ret.ec_int << '\n';
    #endif
    return EndianConvert(ret.ec_int);
  }

  long mybankstream::ReadLong() {
    uEndianCheckUp ret;
    for (int i=0; i<8; i++) ret.ec_reverse[i] = ReadByte();
    return EndianConvert(ret.ec_long);
  }

  std::string mybankstream::ReadString(int l){
    int l2{l};
    std::string ret = "";
    if (!l2) l2 = ReadInt();
    for (int i=0;i<l2;i++) ret+=ReadChar();
    #ifdef DEBUGCHAT
    std::cout << "Got string \"" << ret << "\" (" << l2 << " bytes";
    if (l) std::cout << ", " << l << "requested";
    std::cout << ")\n";
    #endif
    return ret;
  }

  std::string mybankstream::ReadLine(){
    std::string ret{""};
    do{
      char c = ReadChar();
      if (c=='\n' || c=='\r') break;
      ret += c;
    } while (!eof());
    if (!eof() && (buf[Position]=='\r' || buf[Position]=='\n')) Position++; // Make sure both Windows and Unix are supported.
    return ret;
  }


  mybankstream::mybankstream(int size){ buf = new char[size]; bufsize=size; }
  mybankstream::~mybankstream() { delete buf;}





   // Prologue: Declarations
   static std::string JAMJCR_Error;
   static bool JCR_ErrorCrash { false };
   void SetErrorCrash(bool value) { JCR_ErrorCrash=value;}


   std::string Get_JCR_Error_Message() { return JAMJCR_Error; }

   void JamError(std::string errormessage){
     #ifdef DEBUGCHAT
     std::cout << "JCR6 ERROR: " << errormessage <<"\n";
     #endif

     JAMJCR_Error = errormessage;
     if (JCR_ErrorCrash) {
       std::cout << "JCR6 ERROR: " << errormessage<<"\n";
       exit(1);
     }
   }

   // Chapter 1: Reading
   std::map <std::string,JT_Entry> &JT_Dir::Entries(){
     // code comes later
     return EntryMap;
   }


   void JT_Dir::PatchDir(JT_Dir &dir){
     auto ent = dir.Entries();
     for (auto& kv : ent ) AddEntry(kv.first,kv.second);
     for (auto& kv : dir.Comments ) Comments[kv.first] = kv.second;
   }

   void JT_Dir::PatchFile(std::string file){
     JT_Dir need = Dir(file);
     PatchDir(need);
   }

   JT_Entry &JT_Dir::Entry(std::string entry){
     static JT_Entry nothing;
     std::string capentry = Upper(entry);
     if (!EntryMap.count(capentry)) {
       std::string e = "Entry \""; e+=entry; e+="\" not found";
       JamError(e);
       return nothing;
     }
     return (EntryMap[capentry]);
   }

   void JT_Dir::B(std::string entry,mybankstream & data){
     chat({"B: Requested: ",entry});
     static mybankstream nothing(1);
     JAMJCR_Error = "Ok";
     JT_Entry &E = Entry(entry);
     if (JAMJCR_Error != "" && JAMJCR_Error != "Ok") return ;
     std::string storage{E.dataString["__Storage"]};
     if (!CompDrivers.count(storage)){
       std::string e = "Unknown compression method: "; e+=storage;
       JamError(e);
       return ;
     }
     std::ifstream bt ;
     bt.open (E.MainFile, std::ios::binary);
     bt.seekg(E.Offset(),std::ios::beg);
     mybankstream comp(E.CompressedSize());
     data.newbuf(E.RealSize());
     bt.read(comp.pointme(),E.CompressedSize());
     CompDrivers[storage].Expand(comp.pointme(),data.pointme(),comp.getsize(),data.getsize());
     return ;
   }

   std::vector<std::string> JT_Dir::Lines(std::string entry) {
     std::vector<std::string> ret;
     mybankstream bt;
     B(entry,bt);
     if (JAMJCR_Error!="" && JAMJCR_Error!="Ok") return ret;
     while(!bt.eof()) ret.push_back(bt.ReadLine());
     return ret;
   }
   std::string JT_Dir::String(std::string entry) {
     std::string ret;
     mybankstream bt;
     B(entry,bt);
     if (JAMJCR_Error!="" && JAMJCR_Error!="Ok") return "";
     return bt.pointme();
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

   void JT_Dir::AddEntry(std::string name,JT_Entry Entry){
     EntryMap[Upper(name)] = Entry;
     chat({"Entry added as: ",Upper(name)});
   }

   std::string JT_Entry::Entry() { return dataString["__Entry"]; }
   int JT_Entry::CompressedSize() { return dataInt["__CSize"]; }
   int JT_Entry::RealSize() { return dataInt["__Size"]; }
   int JT_Entry::Offset() { return dataInt["__Offset"]; }


   void RegisterCompressDriver(JC_CompressDriver Driver){
     JAMJCR_Error = "Ok";
     if (CompDrivers.count(Driver.Name)) { JamError("Duplicate storage driver!"); return; }
     CompDrivers[Driver.Name]=Driver;
   }

   static std::map<std::string,JD_DirDriver> DirDrivers;
   void RegisterDirDriver(JD_DirDriver Driver){
     JAMJCR_Error = "Ok";
     if (DirDrivers.count(Driver.Name)) { JamError("Duplicate directory driver!"); return; }
     DirDrivers[Driver.Name]=Driver;
   }

   static int FakeStore(char * ibuffer,char * obuffer, int size){
      for (int i=0;i<size;++i) {
        obuffer[i]=ibuffer[i]; // Copying the buffer as a whole in stead of just copying the pointer is essential to make sure still will not get deleted when it shouldn't be!
        //chat({"Position:",std::to_string(i),"/",std::to_string(size)," >> ",std::to_string(obuffer[i])}); // debug mode or not, always comment out when not in use!
      }
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
     char *cmpbuf   = cmpbank.pointme();
     char *dirbuf   = dirbank.pointme();
     bt.read(cmpbuf,ret.FT_csize);
     bt.close();
     if (!CompDrivers.count(ret.FT_storage)) {
       std::string e = "Unknown File Table Compression Method: "; e+=ret.FT_storage;
       JamError(e);
       return ret;
     }
     if (!CompDrivers[ret.FT_storage].Expand(cmpbuf,dirbuf,ret.FT_csize,ret.FT_size)){ return ret; };
     // Time to actually READ everything
     // This code too was originally written in C# and manually converted to C++
     while ((!dirbank.eof()) && (!theend)) {
       auto mtag = dirbank.ReadByte();
       auto ppp  = dirbank.Position;
       std::string tag;
       chat({"What to do with main tag: ",std::to_string(mtag)});
       switch (mtag) {
         case 0xff:
              theend = true;
              break;

         case 0x01:
         tag = Upper(dirbank.ReadString());
         /*switch (tag)*/ {    // Unbelievably primitive... sigh! C++ does not suppor this.... What year is this? 1832?

           //case "FILE":
           if (tag=="FILE") {
             JT_Entry newentry;
             newentry.MainFile = file;
             auto ftag = dirbank.ReadByte();
             while (ftag != 255) {
               switch (ftag) {

                 case 1: { // string
                   auto k = dirbank.ReadString();
                   auto v = dirbank.ReadString();
                   newentry.dataString[k] = v;
                 } break;

                 case 2: {// Boolean
                   auto kb = dirbank.ReadString();
                   auto vb = dirbank.ReadBool();
                   newentry.dataBool[kb] = vb;
                 } break;

                 case 3: {// Integer
                   auto ki = dirbank.ReadString();
                   auto vi = dirbank.ReadInt();
                   newentry.dataInt[ki] = vi;
                 } break;

                 case 255: // the end
                 break;

                 default: {// error
                   std::string er = "Illegal tag in FILE part "; er += std::to_string(ftag); er += " on fatpos "; er += std::to_string(dirbank.Position);
                   JamError (er);
                   return ret;
                 }
               }
               ftag = dirbank.ReadByte();
             }
             auto centry = Upper(newentry.Entry());
             ret.AddEntry(centry,newentry); //ret.Entries[centry] = newentry;
           } //break;

           else if (tag=="COMMENT") { //case "COMMENT":
             std::string commentname = dirbank.ReadString();
             ret.Comments[commentname] = dirbank.ReadString();
           } //break;

           //case "IMPORT":
           //case "REQUIRE":
           else if (tag=="IMPORT" || tag=="REQUIRE") {
             auto deptag = dirbank.ReadByte();
             std::string depk{""};
             std::string depv{""};
             std::map<std::string,std::string> depm; //= new Dictionary<string, string>();
             while (deptag != 255) {
               depk = dirbank.ReadString();
               depv = dirbank.ReadString();
               depm[depk] = depv;
               deptag = dirbank.ReadByte();
             }
             auto depfile = depm["File"];
             //depsig   := depm["Signature"]
             auto deppatha = depm.count("AllowPath") && depm["AllowPath"] == "TRUE";
             std::string depcall = "";
             // var depgetpaths[2][] string
             std::vector<std::string> depgetpaths[2]; // List<string>[] depgetpaths = new List<string>[2];
             // not needed in C++                    depgetpaths[0] = new List<string>();
             // not needed in C++                    depgetpaths[1] = new List<string>();
             auto owndir = ExtractDir(file); //Path.GetDirectoryName(file);
             int deppath = 0;
             if (deppatha) { deppath = 1; }
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
                 JERROR += depcall;
                 JERROR +=") could not imported! Importer reported: ";
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
           }//           break;

           else {
             std::string e = "Unknown instruction tag: "; e+=tag;
             JamError(e);
             return ret;
           }
         }
         break;
         default:
         {
             std::string JERROR = "Unknown main tag ";
             JERROR += std::to_string(mtag);
             JERROR += ", at file table position ";
             JERROR += std::to_string(dirbank.Position);
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
   JT_Create::JT_Create(std::string file, std::string storage){
     using namespace jcr6is;
     //bt.open(file,std::ios::binary|std::ios::trunc);
     chat({"Creating:",file});
     MainFile=file;
     bt = fopen(file.c_str(),"wb");
     //if (!bt.is_open()){
     if (bt==nullptr){
       closed=true;
       JamError("File could not be written");
       return;
     }
     chat({"Success"});
     FT_storage=storage;
     WriteRawString(bt,"JCR6\032");;
     offsetoffset=ftell(bt); //bt.tellp();
     if (offsetoffset!=5) std::cout << "WARNING! Offset was not 5 ("<<offsetoffset<<")\n";
     WriteInt(bt,0);
   }
   JT_Create::~JT_Create(){ Close(); }

   void JT_Create::Close(){
     using namespace jcr6is;
     if (closed) return;
     // write out (and compress) file table
     if (!CompDrivers.count(FT_storage)) {
       JamError("Unknown compression method for File Table storage");
       return;
     }
     //std::ofstream ft;
     FILE * ft;
     std::ifstream it;
     int start = ftell(bt); //bt.tellp();
     int eind{0};
     int csize{0};
     char * buf;
     char * cbuf;
     chat({"File Table at:",std::to_string(start)});
     std::string FTFile = MainFile ; FTFile += ".$$DIRTEMP$$" ;
     //ft.open(FTFile,std::ios::binary|std::ios::trunc);
     ft = fopen(FTFile.c_str(),"wb");
     for (auto comment : Comments){
       WriteByte(ft,1);
       WriteString(ft,"COMMENT");
       WriteString(ft,comment.first);
       WriteString(ft,comment.second);
     }
     for (auto ent : Entries){
       WriteByte(ft,1);
       WriteString(ft,"FILE");
       for (auto data : ent.second.dataString ){ jcr6is::WriteByte(ft,1); jcr6is::WriteString(ft,data.first); jcr6is::WriteString(ft,data.second); }
       for (auto data : ent.second.dataBool   ){ jcr6is::WriteByte(ft,2); jcr6is::WriteString(ft,data.first); jcr6is::WriteBool  (ft,data.second); }
       for (auto data : ent.second.dataInt    ){ jcr6is::WriteByte(ft,3); jcr6is::WriteString(ft,data.first); jcr6is::WriteInt   (ft,data.second); }
       WriteByte(ft,255);
     }
     eind=ftell(ft); //ft.tellp();
     fclose(ft); //ft.close();

     it.open(FTFile,std::ios::binary);
     buf = new char[eind];
     cbuf = new char[eind + ((eind*4)/3)];
     it.read(buf,eind);
     it.close();
     csize = CompDrivers[FT_storage].Compress(buf,cbuf,eind);
     if (csize>=0 && csize<eind){
       WriteInt(bt,eind);
       WriteInt(bt,csize);
       WriteString(bt,FT_storage);
       //bt.write(cbuf,csize);
       fwrite(cbuf,1,csize,bt);
     } else {
       WriteInt(bt,eind);
       WriteInt(bt,eind);
       WriteString(bt,"Store");
       //bt.write(buf,eind);
       fwrite(buf,1,eind,bt);
     }
     delete buf;
     delete cbuf;

     // This footer is new in JCR6, and this C++ library is the first to
     // include it. All still functional JCR6 libraries will get this footer
     // and it's meant for a future feature of JCR6.
     int footer = ftell(bt)+8; //(int)(bt.tellp()) + 8;
     WriteRawString(bt,"JCR6");
     WriteInt(bt,footer);

     // Put the fat offset on the offset spot
     fseek(bt,offsetoffset,SEEK_SET); //bt.seekp(offsetoffset);
     WriteInt(bt,start);

     // closure
     //bt.close();
     fclose(bt);
     closed=true;
   }

   #define loc_configout JAMJCR_Error = "Ok"; if (entryadded) { JamError("You cannot change the global function once an entry has been added!"); return; }
   void JT_Create::AddConfig(std::string key,std::string value) { loc_configout ConfigString[key]=value; }
   void JT_Create::AddConfig(std::string key,int value)         { loc_configout ConfigInt   [key]=value; }
   void JT_Create::AddConfig(std::string key,bool value)        { loc_configout ConfigBool  [key]=value; }
   #undef loc_configout

   JT_Entry JT_Create::AddBuff(std::string entryname,std::string storage,char * buffer,int size, bool dataclearnext){
     using namespace jcr6is;
     JT_Entry entry;
     JAMJCR_Error = "Ok";
     if (!entryadded) {
       chat({"First entry is now being added, let's safe the global config first!"});
       for (auto kv : ConfigString ) { WriteByte(bt,1); WriteString(bt,kv.first); WriteString(bt,kv.second); }
       for (auto kv : ConfigBool)    { WriteByte(bt,2); WriteString(bt,kv.first); WriteBool  (bt,kv.second); }
       for (auto kv : ConfigInt )    { WriteByte(bt,3); WriteString(bt,kv.first); WriteInt   (bt,kv.second); }
       WriteByte(bt,255);
     }
     entryadded=true;
     // add entry itself
     chat({"Compressing with:",storage});
     if (!CompDrivers.count(storage)){
       #ifdef DEBUGCHAT
       for (auto&k:CompDrivers) std::cout << "DEBUG: " << "Compression driver found: " << k.first << "\n";
       #endif
       JamError("Unknown compression method");
       return entry;
     }
     char * compressed = new char[size+((size/4)*3)];
     int csize = CompDrivers[storage].Compress(buffer,compressed,size);
     int offset = ftell(bt); //bt.tellp();
     if (csize<0) {
       if (JAMJCR_Error=="Ok" || JAMJCR_Error=="") JamError("Compression failure");
       return entry;
     }
     fwrite(compressed,1,csize,bt); //bt.write(compressed,csize);
     delete[] compressed;

     for(auto kv:nDataString) entry.dataString[kv.first]=kv.second;
     for(auto kv:nDataInt)    entry.dataInt   [kv.first]=kv.second;
     for(auto kv:nDataBool)   entry.dataBool  [kv.first]=kv.second;
     entry.dataString["__Entry"]    = entryname;
     entry.dataInt   ["__Offset"]   = offset;
     entry.dataInt   ["__Size"]     = size;
     entry.dataInt   ["__CSize"]    = csize;
     entry.dataString["__Storage"]  = storage;
     Entries[Upper(entryname)]=entry;

     // clear the next data if needed
     if (dataclearnext) {
       nDataString.clear();
       nDataBool.clear();
       nDataInt.clear();
     }
     return entry;
   }

   JT_Entry JT_Create::AddFile(std::string filename, std::string entryname, std::string storage,bool dataclearnext){
     JT_Entry e;
     JAMJCR_Error = "Ok";
     char * buf1;
     std::ifstream ib; ib.open(filename,std::ios::binary);
     if (!ib.is_open()) { JamError("Input file could not be read!"); return e;}
     ib.seekg(0,std::ios::end);
     int size = ib.tellg();
     ib.seekg(0,std::ios::beg);
     buf1 = new char[size];
     ib.read(buf1,size);
     ib.close();
     e = AddBuff(entryname,storage,buf1,size,dataclearnext);
     delete[] buf1;
     return e;
   }

   void JT_Create::Import(std::string dependency){}
   void JT_Create::Require(std::string dependency){}
   void JT_Create::AddComment(std::string namecomment,std::string contentcomment){
     JAMJCR_Error = "Ok";
     Comments[namecomment] = contentcomment;
   }


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
