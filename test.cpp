// Lic:
// ***********************************************************
// test.cpp
// This particular file has been released in the public domain
// and is therefore free of any restriction. You are allowed
// to credit me as the original author, but this is not
// required.
// This file was setup/modified in:
// 2019
// If the law of your country does not support the concept
// of a product being released in the public domain, while
// the original author is still alive, or if his death was
// not longer than 70 years ago, you can deem this file
// "(c) Jeroen Broks - licensed under the CC0 License",
// with basically comes down to the same lack of
// restriction the public domain offers. (YAY!)
// ***********************************************************
// Version 19.02.22
// EndLic
#include <iostream>
#include <string>
#include "JCR6.hpp"

// #define testjcr "packdatastorefat.jcr"
#define testjcr "test.jcr"
#define testentry "0011_reftest/reftest.cpp"

int main(){
	std::cout << "Testing utility for JCR6 in C++\nCreated by Jeroen P. Broks\nPlease use as you see fit!\n\n";
	// Of course (this should go without saying), when you modify this file keep in mind you can "spook" up your repository folder
	// causing git to malfunction when you try to update your clone from the actual repository.  Perhaps it's better to copy this file first
	// before you make any modifications, in order to keep git happy :-P

	jcr6::init_JCR6();

	std::cout << "testjcr recognized as " << jcr6::Recognize(testjcr) << "; " << JCR_Error << "\n";
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
	std::cout << "Byte by byte test:";
	while(!bt.eof()) std::cout << bt.ReadChar();
	bt.Position=0;
	std::cout << "Line by line test:";
	while(!bt.eof()) std::cout << "- " << bt.ReadLine() << "\n";
	#endif
	return 0;
}
