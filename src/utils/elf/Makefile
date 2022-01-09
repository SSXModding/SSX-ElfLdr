# Makefile for libutils (ELF variant)

# libutils objects
OBJS = $(OBJDIR)/codeutils.o \
	   $(OBJDIR)/assert.o \
	   $(OBJDIR)/Fnv1a.o \
	   $(OBJDIR)/debugout.o \
	   $(OBJDIR)/FioFile.o \
	   $(OBJDIR)/Allocator.o \

# We do not use newlib file IO routines.
EXTRADEFS = -DNEWLIB_PORT_AWARE

# Define a custom vpath to make this build still work.
# I am SO goddamn sorry.
VPATH = ..

# This makefile builds the "libutils" library
LIB = libutils-elf.a

# Include common tools
include $(TOP)/tools.mk