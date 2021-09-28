# Top-level Makefile for elfldr

TOP=$(PWD)

# configuration variables
SSX3=0
EXPERIMENTAL=0

ifeq ($(SSX3),1)
OBJDIR = obj_ssx3
else
OBJDIR = obj
endif

PASSFLAGS=OBJDIR=$(TOP)/$(OBJDIR) TOP=$(TOP) SSX3=$(SSX3) EXPERIMENTAL=$(EXPERIMENTAL)

all: $(OBJDIR)/
	$(MAKE) $(PASSFLAGS) -C src/utils
	$(MAKE) $(PASSFLAGS) -C src/

clean:
	$(MAKE) $(PASSFLAGS) -C src/utils clean
	$(MAKE) $(PASSFLAGS) -C src/ clean
	rmdir $(OBJDIR)


$(OBJDIR)/:
	mkdir -p $@