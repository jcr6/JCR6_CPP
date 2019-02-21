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
// Version 19.02.20
// EndLic
#include <iostream>
#include <string>
#include "JCR6.hpp"

int main(){
	std::cout << "Testing utility for JCR6 in C++\nCreated by Jeroen P. Broks\nPlease use as you see fit!\n\n";
	// Of course (this should go without saying), when you modify this file keep in mind you can "spook" up your repository folder
	// causing git to malfunction when you try to update your clone from the actual repository.  Perhaps it's better to copy this file first
	// before you make any modifications, in order to keep git happy :-P

	std::cout << "test.jcr recognized as " << jcr6::Recognize("test.jcr") << "; " << JCR_Error << "\n";
	return 0;
}
