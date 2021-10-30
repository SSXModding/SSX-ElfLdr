# Makefile for the elfldr's main product

VERSION=0.1

# Base objects all versions of
# elfldr need to link properly.
OBJS = $(OBJDIR)/main.o \
	   $(OBJDIR)/patch.o \
	   $(OBJDIR)/GameVersion.o \
	   $(OBJDIR)/ElfLoader.o
	  
# We do not use newlib file IO routines.
EXTRADEFS = -DNEWLIB_PORT_AWARE

# Use Nano newlib.
EELIBC = c_nano
	  
# Link libutils (elf version), and some other
# fancy ps2sdk libs we need
LIBS = -lutils-elf -lkernel -lps2sdkc 

BIN = ssx_elfldr

# EXPERIMENTAL should maybe produce a new filename?

ifeq ($(EXPERIMENTAL),1)
OBJS += $(OBJDIR)/patch_experimental.o
EXTRADEFS += -DEXPERIMENTAL
endif

# OG objects
OBJS += $(OBJDIR)/patch_hostfs.o \
	   $(OBJDIR)/patch_memory.o

# Include common makefile
include $(TOP)/tools.mk