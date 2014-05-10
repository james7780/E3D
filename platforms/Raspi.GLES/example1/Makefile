# compile SDL project from cli like so:
#gcc -o test test.c `sdl-config --cflags --libs`

#CC = gcc
CC = g++
CXX = g++

# get the proper CFLAGS and LDFLAGS for SDL:
#SDL_CFLAGS := $(shell sdl-config --cflags)
#SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS := $(shell sdl-config --cflags) -D_RASPI
CXXFLAGS := $(CFLAGS) $(CXXFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := $(shell sdl-config --libs)
LDFLAGS += -lGL -lGLU -lm -lstdc++ 



all: example1

example1 : example1.o
	$(CXX) $(LDFLAGS) example1.o ../libe3d.a -o example1



