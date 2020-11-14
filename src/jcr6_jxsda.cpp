#include <jcr6_core.hpp>
#include <iostream>
#include <JXSDA.hpp>
#include "..\headers\jcr6_jxsda.hpp"
// Jeroen's eXtremely Simplistic Dictionary Algorithm

// Driver for JCR6

using namespace TrickyUnits;
using std::cout;

int JXSDA_Compress(char* Uncompressed, char* Compressed, int size_uncompressed) {
    cout << "Compressing not yet supported in JXSDA! Please come back later!\n";
    return size_uncompressed * 2; // Make sure system will always deem this too big!
}

bool JXSDA_Expand(char* Compressed, char* UnCompressed, int size_compressed, int size_uncompressed) {
    auto e = jxsda_unpack(Compressed, size_compressed, UnCompressed, size_uncompressed);
    return e==jxda_error::Ok;
}

namespace jcr6 {
    void init_jxsda() {
        using namespace jcr6;
        JC_CompressDriver Driver;
        Driver.Compress = JXSDA_Compress;
        Driver.Expand = JXSDA_Expand;
        Driver.Name = "jxsda";
        RegisterCompressDriver(Driver);
    }
}

