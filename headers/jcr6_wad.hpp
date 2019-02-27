// Lic:
//   jcr6_wad.hpp
//   JCR6 for C++ -- WAD driver
//   version: 19.02.27
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

    The WAD file system is a data-resource format developed by id-Software for
    the DOOM project. WAD stands for "Where is All the Data?"

    This system has been used in:
    - DOOM
    - The Ultimate DOOM
    - DOOM II
    - Evilution
    - The Plutonia Experiment
    - Heretic
    - HeXen: Beyond Heretic
    - Rise of the Triad
    - And a few more.

    This driver allows you to view a WAD file as it were a JCR6 file.
    A few notes.
    - A WAD entry's name can only have 8 letters. Traditionally all in CAPS.
      This poses no issues in JCR6 as JCR6 only can do better than that.
    - WAD files never have any folders
    - The way DOOM, Heretic and HeXen, all files using the (modified) DOOM
      engine store their maps is something JCR6 really can't handle, due to
      each map containing multiple files all with the same name. This driver
      automatically converts map into a folder bearing the proper files.
    - WAD requires a LittleEndian CPU environment. This driver does have the
      possibility to auto-reverse integers in case this code runs on a BigEndian
      CPU, however as I do not have a BigEndian machine at my disposal right
      now, I could not test this.
    - Keep in mind that the content within WAD files can be protected by
      copyright. No law can stop you from viewing the content by means of this
      driver, but when you extract and distribute stuff, there can be a problem!
      You are using this driver on your own accord and you and you alone bear
      responsibility of what you do with the data you can get by that.

*/

#include "jcr6_core.hpp"

namespace jcr6{
  void InitWAD(); 
}
