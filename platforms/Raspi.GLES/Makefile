# Makefile for libe3d.a (E3D Simple Cross-Platform 3D library)
# For Raspberry Pi + GLES dev environment
# James Higgs 2014

#CC = gcc
CC = g++
CXX = g++
AR = ar
RANLIB = ranlib
STRIP = strip

# get the proper CFLAGS and LDFLAGS for SDL:
#SDL_CFLAGS := $(shell sdl-config --cflags)
#SDL_LDFLAGS := $(shell sdl-config --libs)

#CFLAGS := $(shell sdl-config --cflags) -D_LINUX -D_RASPI
#CXXFLAGS := $(CFLAGS) $(CXXFLAGS) -fno-exceptions -fno-rtti
#LDFLAGS := $(shell sdl-config --libs)
#LDFLAGS += -lGL -lGLU -lm -lstdc++

# For Broadcom / GLES stuff
CFLAGS += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

LDFLAGS += -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont

INCLUDES += -I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont


TARGET_LIB = libe3d.a
OBJS = E3D_Camera.o E3D_Light.o E3D_Model.o E3D_Object.o \
			E3D_Scene.o E3D_Utility.o E3D_PlatformSpecific.o

# path to E3D folder
E3D_PATH = ../..
E3D_INC_DIR = ../../include
E3D_SRC_DIR = ../../src

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

E3D_Camera.o : $(E3D_SRC_DIR)/E3D_Camera.cpp $(E3D_INC_DIR)/E3D_Camera.h $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Camera.cpp -o $@

E3D_Light.o : $(E3D_SRC_DIR)/E3D_Light.cpp $(E3D_INC_DIR)/E3D_Light.h $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Light.cpp -o $@
	
E3D_Model.o : $(E3D_SRC_DIR)/E3D_Model.cpp $(E3D_INC_DIR)/E3D_Model.h $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Model.cpp -o $@

E3D_Object.o : $(E3D_SRC_DIR)/E3D_Object.cpp $(E3D_INC_DIR)/E3D_Object.h $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Object.cpp -o $@

E3D_Scene.o : $(E3D_SRC_DIR)/E3D_Scene.cpp $(E3D_INC_DIR)/E3D_Scene.h $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Scene.cpp -o $@

E3D_Utility.o : $(E3D_SRC_DIR)/E3D_Utility.cpp $(E3D_INC_DIR)/E3D_Utility.h $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Utility.cpp -o $@

E3D_PlatformSpecific.o : E3D_PlatformSpecific.cpp $(E3D_INC_DIR)/E3D.h
	$(CXX) $(CFLAGS) $(INCLUDES) -c E3D_PlatformSpecific.cpp -o $@


clean:
	$(RM) -f $(TARGET_LIB) $(OBJS)

