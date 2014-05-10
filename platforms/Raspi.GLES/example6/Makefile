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
LDFLAGS += -lmikmod -lGL -lGLU -lm -lstdc++ 

TARGET = webfest
OBJS = webfest.o behaviours.o loadweb.o hiscore.o

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) ../libe3d.a -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)



