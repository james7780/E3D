# Makefile for libe3d.a (E3D Simple Cross-Platform 3D library)
# For PS2SDK dev environment
# James Higgs 2006

# path to E3D folder
E3D_PATH = ../..

EE_LIB = libe3d.a
EE_OBJS = E3D_Camera.o E3D_Light.o E3D_Model.o E3D_Object.o \
			E3D_Scene.o E3D_Utility.o E3D_PlatformSpecific.o

EE_LIBS =  -lsbv_patches -lmc -lpad -ldebug -lc  -lm

EE_INCS= -I$(PS2GCC)/ee/include \
	-I$(PS2DEV)/gsKit/ee/gs/include \
	-I$(PS2DEV)/gsKit/ee/dma/include \
	-I$(PS2DEV)/sbv-1.0-lite/include

EE_LDFLAGS = -L. -L$(PS2DEV)/gsKit/lib -L$(PS2DEV)/sbv-1.0-lite/lib
EE_CFLAGS =  -DPS2_DEBUG

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp

all: $(EE_LIB)

E3D_Camera.o : 
	$(EE_CC) $(CFLAGS) -c $(E3D_PATH)/src/E3D_Camera.cpp -o $@

E3D_Light.o : 
	$(EE_CC) $(CFLAGS) -c $(E3D_PATH)/src/E3D_Light.cpp -o $@
	
E3D_Model.o : 
	$(EE_CC) $(CFLAGS) -c $(E3D_PATH)/src/E3D_Model.cpp -o $@

E3D_Object.o : 
	$(EE_CC) $(CFLAGS) -c $(E3D_PATH)/src/E3D_Object.cpp -o $@

E3D_Scene.o : 
	$(EE_CC) $(CFLAGS) -c $(E3D_PATH)/src/E3D_Scene.cpp -o $@

E3D_Utility.o : 
	$(EE_CC) $(CFLAGS) -c $(E3D_PATH)/src/E3D_Utility.cpp -o $@

E3D_PlatformSpecific.o : 
	$(EE_CC) $(CFLAGS) -c E3D_PlatformSpecific.cpp -o $@


clean:
	$(RM) $(EE_LIB) $(EE_OBJS)

