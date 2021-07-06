// Lic:
// src/jcr6_jxsda.cpp
// jxsda driver for JCR6
// version: 21.07.06
// Copyright (C) 2021 Jeroen P. Broks
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