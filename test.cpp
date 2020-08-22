// Lic:
// ***********************************************************
// test.cpp
// This particular file has been released in the public domain
// and is therefore free of any restriction. You are allowed
// to credit me as the original author, but this is not
// required.
// This file was setup/modified in:
// 2019, 2020
// If the law of your country does not support the concept
// of a product being released in the public domain, while
// the original author is still alive, or if his death was
// not longer than 70 years ago, you can deem this file
// "(c) Jeroen Broks - licensed under the CC0 License",
// with basically comes down to the same lack of
// restriction the public domain offers. (YAY!)
// ***********************************************************
// Version 20.08.22
// EndLic
#include <iostream>
#include <string>
#include "JCR6.hpp"
#include "headers/jcr6_wad.hpp"
#include "headers/jcr6_quakepak.hpp"

// #define testjcr "packdatastorefat.jcr"
// #define testjcr "testbin/fullzlib.jcr"
// #define testentry "jcr6_zlib.hpp"

// Only for testing the WAD Driver
//#define testjcr "/Volumes/Scyndi/Games4Emulators/Doomsday/Doom/DOOM.WAD"

// Testing Quake (Mac)
// #define testjcr "/Volumes/Scyndi/DOS/QUAKE_SW/ID1/PAK0.PAK"
// Testing Quake (Windows)
#define testjcr "E:/DOS/QUAKE_SW/ID1/PAK0.PAK"


int main(){
	std::cout << "Testing utility for JCR6 in C++\nCreated by Jeroen P. Broks\nPlease use as you see fit!\n\n";
	// Of course (this should go without saying), when you modify this file keep in mind you can "spook" up your repository folder
	// causing git to malfunction when you try to update your clone from the actual repository.  Perhaps it's better to copy this file first
	// before you make any modifications, in order to keep git happy :-P

	jcr6::init_JCR6();
	// JCR_InitZlib();
	jcr6::InitWAD();
	jcr6::init_quakepak();

	std::cout << testjcr << " recognized as " << jcr6::Recognize(testjcr) << "; " << JCR_Error << "\n";
	auto jcr = jcr6::Dir(testjcr);
	std::cout << "Return status:      " << JCR_Error << '\n';
	std::cout << "File Table offset:  " << jcr.FT_offset << '\n';
	std::cout << "File Table size:    " << jcr.FT_size << '\n';
	std::cout << "File Table csize:   " << jcr.FT_csize << '\n';
	std::cout << "File Table storage: " << jcr.FT_storage << '\n';
	auto ent = jcr.Entries();
	for (auto& kv : ent ){
		std::cout << "\nEntry Key:" << kv.first;
		std::cout << "\tMainfile: " << kv.second.MainFile << "\n";
		for(auto &skv : kv.second.dataString ) { std::cout << "\t string " << skv.first << " = \"" << skv.second << "\"\n"; }
		for(auto &bkv : kv.second.dataBool   ) { std::cout << "\t bool   " << bkv.first << " = " << bkv.second << "\n"; }
		for(auto &ikv : kv.second.dataInt    ) { std::cout << "\t int    " << ikv.first << " = " << ikv.second << "\n"; }
	}
	#ifdef testentry
	std::cout << "\n\nLet's take a look at file: "<<testentry<<"\n";
	jcr6::mybankstream bt;
	jcr.B(testentry,bt);
	std::cout << "Report status: " << JCR_Error    << "\n";
	std::cout << "Buffer size:   " << bt.getsize() << "\n";
	std::cout << "\n";
	std::cout << "Byte by byte test:\n";
	while(!bt.eof()) std::cout << bt.ReadChar();
	bt.Position=0;
	std::cout << "\n\nLine by line test:\n";
	while(!bt.eof()) std::cout << "- " << bt.ReadLine() << "\n";
	std::cout << "\n\nLines test:\n";
	auto lines = jcr.Lines(testentry);
	for(int i=0; i<lines.size(); i++) std::cout << i << "\t" << lines[i] << "\n";
	std::cout << "\n\nEntire string in once test:\n" << jcr.String(testentry) << "\n\n";
	#endif
	return 0;
}