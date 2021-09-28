# Top-level Makefile for elfldr

TOP=$(PWD)

# configuration variables
SSX3=0
EXPERIMENTAL=0

BINDIR = bin

ifeq ($(SSX3),1)
OBJDIR = obj_ssx3
else
OBJDIR = obj
endif

PASSFLAGS=OBJDIR=$(TOP)/$(OBJDIR) BINDIR=$(TOP)/$(BINDIR) TOP=$(TOP) SSX3=$(SSX3) EXPERIMENTAL=$(EXPERIMENTAL)

all: $(BINDIR)/ $(OBJDIR)/
	$(MAKE) $(PASSFLAGS) -C src/utils
	$(MAKE) $(PASSFLAGS) -C src/

clean:
	$(MAKE) $(PASSFLAGS) -C src/utils clean
	$(MAKE) $(PASSFLAGS) -C src/ clean
	rmdir $(OBJDIR) $(BINDIR)

# temporary target to build the sample ERL
# once ERL support is actually a thing, this
# will be put into all.
erl:
	$(MAKE) $(PASSFLAGS) -C src/sampleerl

$(OBJDIR)/:
	mkdir -p $@
	
$(BINDIR)/:
	mkdir -p $@