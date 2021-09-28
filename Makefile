# Top-level Makefile for elfldr

TOP=$(PWD)

SSX3=0
EXPERIMENTAL=0

ifeq ($(SSX3),1)
OBJDIR = obj_ssx3
else
OBJDIR = obj
endif

all: $(OBJDIR)/
	$(MAKE) OBJDIR=$(TOP)/$(OBJDIR) TOP=$(TOP) -C src/utils
	$(MAKE) OBJDIR=$(TOP)/$(OBJDIR) TOP=$(TOP) SSX3=$(SSX3) EXPERIMENTAL=$(EXPERIMENTAL) -C src/

clean:
	$(MAKE) OBJDIR=$(TOP)/$(OBJDIR) TOP=$(TOP) -C src/utils clean
	$(MAKE) OBJDIR=$(TOP)/$(OBJDIR) TOP=$(TOP) SSX3=$(SSX3) EXPERIMENTAL=$(EXPERIMENTAL) -C src/ clean
	rmdir $(OBJDIR)


$(OBJDIR)/:
	mkdir -p $@