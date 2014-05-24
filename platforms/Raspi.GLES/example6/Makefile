# compile SDL project from cli like so:
#gcc -o test test.c `sdl-config --cflags --libs`

#CC = gcc
CC = g++
CXX = g++

# get the proper CFLAGS and LDFLAGS for SDL:
#SDL_CFLAGS := $(shell sdl-config --cflags)
#SDL_LDFLAGS := $(shell sdl-config --libs)
CFLAGS := $(shell sdl-config --cflags) -D_RASPI
#CXXFLAGS := $(CFLAGS) $(CXXFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := $(shell sdl-config --libs) -lmikmod

# For Broadcom / GLES stuff
CFLAGS += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

LDFLAGS += -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont

INCLUDES += -I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

TARGET = webfest
OBJS = webfest.o behaviours.o loadweb.o hiscore.o

all: $(TARGET)

$(TARGET) : $(OBJS) ../libe3d.a
	$(CXX) $(LDFLAGS) $(OBJS)  ../libe3d.a -o $@

%.o: %.cpp
	@rm -f $@ 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)





