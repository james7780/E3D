# Makefile for libe3d.a (E3D Simple Cross-Platform 3D library)
# For Linux dev environment
# James Higgs 2006

# compile SDL project from cli like so:
#gcc -o test test.c `sdl-config --cflags --libs`

#CC = gcc
CC = g++
CXX = g++
AR = ar
RANLIB = ranlib
STRIP = strip

# get the proper CFLAGS and LDFLAGS for SDL:
#SDL_CFLAGS := $(shell sdl-config --cflags)
#SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS := $(shell sdl-config --cflags)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := $(shell sdl-config --libs)
LDFLAGS += -lGL -lGLU -lm -lstdc++


TARGET_LIB = libe3d.a
OBJS = E3D_Camera.o E3D_Light.o E3D_Model.o E3D_Object.o \
			E3D_Scene.o E3D_Utility.o E3D_PlatformSpecific.o

#LIBS= -lpspgum -lpspgu -lm -lstdc++

# path to E3D folder
E3D_PATH = ../..
E3D_INC_DIR = ../../include
E3D_SRC_DIR = ../../src

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

E3D_Camera.o : $(E3D_SRC_DIR)/E3D_Camera.cpp $(E3D_INC_DIR)/E3D_Camera.h $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Camera.cpp -o $@

E3D_Light.o : $(E3D_SRC_DIR)/E3D_Light.cpp $(E3D_INC_DIR)/E3D_Light.h $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Light.cpp -o $@
	
E3D_Model.o : $(E3D_SRC_DIR)/E3D_Model.cpp $(E3D_INC_DIR)/E3D_Model.h $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Model.cpp -o $@

E3D_Object.o : $(E3D_SRC_DIR)/E3D_Object.cpp $(E3D_INC_DIR)/E3D_Object.h $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Object.cpp -o $@

E3D_Scene.o : $(E3D_SRC_DIR)/E3D_Scene.cpp $(E3D_INC_DIR)/E3D_Scene.h $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Scene.cpp -o $@

E3D_Utility.o : $(E3D_SRC_DIR)/E3D_Utility.cpp $(E3D_INC_DIR)/E3D_Utility.h $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Utility.cpp -o $@

E3D_PlatformSpecific.o : E3D_PlatformSpecific.cpp $(E3D_INC_DIR)/E3D.h
	$(CC) $(CFLAGS) -c E3D_PlatformSpecific.cpp -o $@


cleanlib:
	$(RM) -f $(TARGET_LIB) $(OBJS)

