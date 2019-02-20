# Let's be frank
# This make file should NOT be used, unless you know what *I* am doing.
# I only created this file for testing if compiling works....
# I would not really recommend using it for building the library for your own uses (yet).


# change this to your machine
PLATFORM=mac


all:mkobjects/*.$(PLATFORM).o

mkobjects/*.$(PLATFORM).o:src/* headers/*
	@echo Compiling JCR6
	@g++ -c -std=c++11 -o mkobjects/jcr6.$(PLATFORM).o -pedantic-errors src/*.cpp
	@echo Done
