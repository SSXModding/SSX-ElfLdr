# Makefile for libutils

# TODO: allow ERL LibUtils to use allocation facilities?

# libutils objects
OBJS = $(OBJDIR)/codeutils.o \
	   $(OBJDIR)/assert.o \
	   $(OBJDIR)/fnv.o \
	   $(OBJDIR)/debugout.o

# Define a custom vpath to make this build still work.
# I am SO goddamn sorry.
VPATH = ..

# define ERL so the ERL version builds using game api stuff.
EXTRADEFS = -DERL

# This makefile builds the "libutils" library
LIB = libutils-erl.a

# Include common tools
include $(TOP)/tools.mk