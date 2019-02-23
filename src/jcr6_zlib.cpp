#include <string>
#include "../headers/jcr6_zlib.hpp"


#define JCR6ZLIBWARN
#ifdef JCR6ZLIBWARN
#include <iostream>
#endif

int JZL_Compress(char *Uncompressed,char *Compressed, int size_uncompressed){
    // code comes later
    return size_uncompressed*2; // Force store as long as this is not fully set up!
}

bool JZL_Expand(char *Compressed, char *UnCompressed, int size_compressed, int size_uncompressed){
  uLongf szc = size_uncompressed;
  int err = uncompress ((Bytef*)UnCompressed,   &szc, (Bytef*)Compressed, size_compressed);
  if (err!=Z_OK) {
    std::string e{"Error by zlib! "}; e+=std::to_string(err);
    #ifdef JCR6ZLIBWARN
    std::cout << "DEBUG: size_compressed = " << szc << "; size_uncompressed = " << size_uncompressed << "\n";
    std::cout << "ERROR!\t" << e << "\n";
    #endif
    jcr6::JamError(e);
  }
  return err==Z_OK;
}


void JCR_InitZlib(){
  using namespace jcr6;
  JC_CompressDriver Driver;
  Driver.Compress = JZL_Compress;
  Driver.Expand   = JZL_Expand;
  Driver.Name     = "zlib";
  RegisterCompressDriver(Driver);
}
