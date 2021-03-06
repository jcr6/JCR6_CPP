# This makefile is just a way to create a quick utility testing an existing JCR6 file
# All the .o files created in the process will live in mkobjects/


# change this to your machine
PLATFORM=mac


all:mkobjects/jcr6_core.$(PLATFORM).o mkobjects/jcr6_zlib.$(PLATFORM).o  mkobjects/jcr6_wad.$(PLATFORM).o mkobjects/jcr6_quakepak.$(PLATFORM).o mkobjects/testme.$(PLATFORM).o test$(PLATFORM) mkobjects/testcr.$(PLATFORM).o testcr$(PLATFORM) 
#mkobjects/*.$(PLATFORM).o:mkobjects/jcr6_core.$(PLATFORM).o mkobjects/jcr6_zlib.$(PLATFORM).o mkobjects/testme.$(PLATFORM).o test$(PLATFORM)

mkobjects/jcr6_core.$(PLATFORM).o:src/jcr6_core* headers/jcr6_core*
	@echo Compiling Core
	@g++ -std=c++11 -o mkobjects/jcr6_core.$(PLATFORM).o -pedantic-errors -c src/jcr6_core.cpp
mkobjects/jcr6_zlib.$(PLATFORM).o:src/jcr6_zlib* headers/jcr6_zlib*
	@echo Compiling ZLib Driver
	@g++ -std=c++11 -o mkobjects/jcr6_zlib.$(PLATFORM).o -pedantic-errors -c src/jcr6_zlib.cpp
mkobjects/jcr6_wad.$(PLATFORM).o:src/jcr6_wad.cpp headers/jcr6_wad.hpp
	@echo Compiling WAD Driver -- Where is All the Data?
	@g++ -std=c++11 -o mkobjects/jcr6_wad.$(PLATFORM).o -pedantic-errors -c src/jcr6_wad.cpp
mkobjects/jcr6_quakepak.$(PLATFORM).o:src/jcr6_quakepak.cpp headers/jcr6_quakepak.hpp
	@echo Compiling Quake Pack Driver
	@g++ -std=c++11 -o mkobjects/jcr6_quakepak.$(PLATFORM).o -pedantic-errors -c src/jcr6_quakepak.cpp
mkobjects/testme.$(PLATFORM).o:test.cpp mkobjects/jcr6_core.$(PLATFORM).o mkobjects/jcr6_zlib.$(PLATFORM).o
	@echo Compiling Test utility
	@g++ -std=c++11 -o mkobjects/testme.$(PLATFORM).o -pedantic-errors -c test.cpp
mkobjects/testcr.$(PLATFORM).o:testcr.cpp mkobjects/jcr6_core.$(PLATFORM).o mkobjects/jcr6_zlib.$(PLATFORM).o
	@echo Compiling Test Creation utility
	@g++ -std=c++11 -o mkobjects/testcr.$(PLATFORM).o -pedantic-errors -c testcr.cpp
test$(PLATFORM):mkobjects/jcr6_core.$(PLATFORM).o mkobjects/jcr6_zlib.$(PLATFORM).o mkobjects/testme.$(PLATFORM).o mkobjects/jcr6_wad.$(PLATFORM).o
	@echo Creating Test utility
	@g++ -std=c++11 -o test$(PLATFORM) mkobjects/jcr6*.$(PLATFORM).o /Volumes/Scyndi/C-Libs/zlib/libz.a mkobjects/testme.$(PLATFORM).o
testcr$(PLATFORM):mkobjects/jcr6_core.$(PLATFORM).o mkobjects/jcr6_zlib.$(PLATFORM).o mkobjects/testcr.$(PLATFORM).o
	@echo Creating Test Creation utility
	@g++ -std=c++11 -o testcr$(PLATFORM) mkobjects/jcr6*.$(PLATFORM).o /Volumes/Scyndi/C-Libs/zlib/libz.a mkobjects/testcr.$(PLATFORM).o
	@echo Done
