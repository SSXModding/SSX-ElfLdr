# Makefile for libutils

# libutils objects
OBJS = $(OBJDIR)/codeutils.o \
	   $(OBJDIR)/assert.o \
	   $(OBJDIR)/debugout.o

# Define a custom vpath to make this build still work.
# I am SO goddamn sorry.
VPATH = ..

# This makefile builds the "libutils" library
LIB = libutils-elf.a

# Include common tools
include $(TOP)/tools.mk