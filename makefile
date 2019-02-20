# change this to your machine
PLATFORM=mac


all:src/* header/*

mkobjects/*.$(PLATFORM).o:src/* header/*
	@echo Compiling JCR6
	@g++ --std:c++11 -o mkobjects/jcr6.$(PLATFORM).o src/*.cpp
	@echo Done
