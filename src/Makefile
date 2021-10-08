# Makefile for the elfldr's main product

VERSION=0.1

# Base objects all versions of
# elfldr need to link properly.
OBJS = $(OBJDIR)/main.o \
	   $(OBJDIR)/patch.o \
	   $(OBJDIR)/ElfLoader.o
	  
# Use Nano newlib.
EELIBC = c_nano
	  
# Link libutils (elf version), and some other
# fancy ps2sdk libs we need
LIBS = -lutils-elf -lerl-loader -lkernel -lps2sdkc 

ifeq ($(SSX3),1)
BIN = ssx_elfldr_ssx3
else
BIN = ssx_elfldr
endif

# EXPERIMENTAL should maybe produce a new filename?

# ifeq's for including patches

ifeq ($(EXPERIMENTAL),1)
OBJS += $(OBJDIR)/patch_experimental.o
EXTRADEFS += -DEXPERIMENTAL
FEATURES += experimental
endif

ifeq ($(SSX3),1)
OBJS += $(OBJDIR)/patch_hostfs_ssx3.o
EXTRADEFS += -DSSX3
else
# OG objects
OBJS += $(OBJDIR)/patch_hostfs.o \
	   $(OBJDIR)/patch_memory.o
endif


# Include common makefile
include $(TOP)/tools.mk