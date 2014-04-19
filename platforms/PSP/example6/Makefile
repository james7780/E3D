# Makefile for E3D example6 test program

TARGET = webfest
OBJS = webfest.o behaviours.o hiscore.o loadweb.o $(PSPSDK)/samples/gu/common/callbacks.o $(PSPSDK)/samples/gu/common/vram.o

INCDIR =
CFLAGS = -G0 -Wall -I$(PSPSDK)/samples/gu/common
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../
LDFLAGS =
LIBS= -le3d -lpsppower -lpspgum -lpspgu -lpspaudio -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = WebFest 001
PSP_EBOOT_ICON = icon.png
PSP_EBOOT_ICON1 = icon1.png
PSP_EBOOT_PIC1 = pic1.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

install : EBOOT.PBP
	xcopy /Y EBOOT.PBP E:\PSP\GAME150\webfest\EBOOT.PBP
	xcopy /Y PARAM.SFO E:\PSP\GAME150\webfest\PARAM.SFO


