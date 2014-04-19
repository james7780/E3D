# compile SDL project from cli like so:
#gcc -o test test.c `sdl-config --cflags --libs`

#CC = gcc
CC = g++
CXX = g++

# get the proper CFLAGS and LDFLAGS for SDL:
#SDL_CFLAGS := $(shell sdl-config --cflags)
#SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS := $(shell sdl-config --cflags)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := $(shell sdl-config --libs)
LDFLAGS += -lmikmod -lGL -lGLU -lm -lstdc++ 



all: webfest

webfest : webfest.o behaviours.o loadweb.o hiscore.o ../libe3d.a
	$(CXX) $(LDFLAGS) webfest.o behaviours.o loadweb.o hiscore.o ../libe3d.a -o webfest



