# Makefile for E3D example/test program

TARGET = example1
OBJS = example1.o $(PSPSDK)/samples/gu/common/callbacks.o $(PSPSDK)/samples/gu/common/vram.o

INCDIR =
CFLAGS = -G0 -Wall -I$(PSPSDK)/samples/gu/common
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../
LDFLAGS =
LIBS= -le3d -lpspgum -lpspgu -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = E3D Example1 by Jum

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


#logo.o: logo.raw
#	bin2o -i logo.raw logo.o logo
