# Top-level Makefile for elfldr

TOP=$(PWD)

# configuration variables
SSX3=0
EXPERIMENTAL=0

BINDIR = bin

# TODO
ifeq ($(SSX3),1)
OBJDIR = obj_ssx3
else
OBJDIR = obj
endif

# submake macro thing
define do_submake
	$(info Submake for $(1)...)
	@$(MAKE) OBJDIR=$(TOP)/$(1)/$(OBJDIR) BINDIR=$(TOP)/$(BINDIR) TOP=$(TOP) SSX3=$(SSX3) EXPERIMENTAL=$(EXPERIMENTAL) -C $(1) $(2)
endef

.PHONY: all clean erl

all: $(BINDIR)/
	$(call do_submake,src/utils/elf)
	$(call do_submake,src/erl)
	$(call do_submake,src/)

clean:
	$(call do_submake,src/utils/elf,clean)
	$(call do_submake,src/erl,clean)
	$(call do_submake,src/,clean)
	rmdir $(BINDIR)

# temporary target to build the sample ERL
# once ERL support is actually a thing, this
# will be put into all.
erl:
	$(call do_submake,src/utils/erl)
	$(call do_submake,src/sampleerl)
	
$(BINDIR)/:
	mkdir -p $@