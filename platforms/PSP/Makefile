# Makefile for libe3d.a (E3D Simple Cross-Platform 3D library)
# For PSPSDK dev environment
# James Higgs 2006

TARGET_LIB = libe3d.a
OBJS = E3D_Camera.o E3D_Light.o E3D_Model.o E3D_Object.o \
			E3D_Scene.o E3D_Utility.o E3D_PlatformSpecific.o

INCDIR =
CFLAGS = -G0 -Wall -I$(PSPSDK)/samples/gu/common
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lm -lstdc++

E3D_SRC_DIR = ../../src

EXTRA_TARGETS = 
PSP_EBOOT_TITLE = 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

E3D_Camera.o : $(E3D_SRC_DIR)/E3D_Camera.cpp
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Camera.cpp -o $@

E3D_Light.o : $(E3D_SRC_DIR)/E3D_Light.cpp
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Light.cpp -o $@
	
E3D_Model.o : $(E3D_SRC_DIR)/E3D_Model.cpp
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Model.cpp -o $@

E3D_Object.o : $(E3D_SRC_DIR)/E3D_Object.cpp
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Object.cpp -o $@

E3D_Scene.o : $(E3D_SRC_DIR)/E3D_Scene.cpp
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Scene.cpp -o $@

E3D_Utility.o : $(E3D_SRC_DIR)/E3D_Utility.cpp
	$(CC) $(CFLAGS) -c $(E3D_SRC_DIR)/E3D_Utility.cpp -o $@

E3D_PlatformSpecific.o : E3D_PlatformSpecific.cpp
	$(CC) $(CFLAGS) -c E3D_PlatformSpecific.cpp -o $@


cleanlib:
	$(RM) -f libe3d.a $(E3DOBJS)

