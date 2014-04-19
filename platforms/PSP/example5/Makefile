# Makefile for E3D example/test program

TARGET = example5
OBJS = example5.o $(PSPSDK)/samples/gu/common/callbacks.o $(PSPSDK)/samples/gu/common/vram.o

INCDIR =
CFLAGS = -G0 -Wall -I$(PSPSDK)/samples/gu/common
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../
LDFLAGS =
LIBS= -le3d -lpspgum -lpspgu -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = E3D Example 5 - Tank Game

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


