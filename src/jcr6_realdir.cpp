// Lic:
// src/jcr6_realdir.cpp
// "Real Dir" driver for JCR6
// version: 21.04.04
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
// Core
#include "jcr6_core.hpp"

// Tricky's Units
#include <FileList.hpp>
#include <QuickStream.hpp>
#include <QuickString.hpp>

using namespace std; // I am EVIL! I know!

namespace jcr6 {


	static bool RDRec(string pth) {
		return TrickyUnits::IsDir(pth);
	}

	static JT_Dir RDGet(string pth) {
		using namespace TrickyUnits;
        JT_Dir ret;
        pth = TReplace(pth, '\\', '/');
		auto Lijst = GetTree(pth);
		for (auto file : Lijst) {
            JT_Entry entry;
            //foffset = rint(bt);
            auto fsize = FileSize(pth+"/"+file);
            entry.MainFile = pth + "/" + file;
            entry.dataInt["__Offset"] = 0;
            entry.dataInt["__CSize"] = fsize;
            entry.dataInt["__Size"] = fsize;
            entry.dataString["__Entry"] = file;
            entry.dataString["__Notes"] = "";
            entry.dataString["__Storage"] = "Store"; // Must be set or bad stuff will happen!
            entry.dataString["__Author"] = "";            
            ret.AddEntry(entry.dataString["__Entry"], entry);        
		}
        return ret;
	}


	void InitRealDir() {
        JD_DirDriver Dir;
        Dir.Recognize = RDRec;
        Dir.Dir = RDGet;
        Dir.Name = "RealDir";
        RegisterDirDriver(Dir);
	}

}