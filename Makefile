# Top-level Makefile for elfldr

TOP=$(PWD)

EXPERIMENTAL=0

BINDIR = bin
OBJDIR = obj

# submake macro thing
#$(info Submake for $(1)...)
define do_submake

	+@$(MAKE) OBJDIR=$(TOP)/$(1)/$(OBJDIR) BINDIR=$(TOP)/$(BINDIR) TOP=$(TOP) EXPERIMENTAL=$(EXPERIMENTAL) -C $(1) $(2)
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