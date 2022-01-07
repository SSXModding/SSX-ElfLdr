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

.PHONY: all clean

all: $(BINDIR)/
	$(call do_submake,src/utils/elf)
	$(call do_submake,src/erl)
	$(call do_submake,src)
# Build the ERL flavor of LibUtils and
# the sample ERL.
# (any custom ERLs will for now need to be in tree :()
	$(call do_submake,src/utils/erl)
	$(call do_submake,src/sampleerl)
# Run publish.sh if it exists in the repository root.
# This aforementioned script allows you to publish elfldr
# to your game roots if you're a developer and want rapid test cycles
	@[ -f "$(TOP)/publish.sh" ] && $(TOP)/publish.sh
clean:
	$(call do_submake,src/utils/elf,clean)
	$(call do_submake,src/erl,clean)
	$(call do_submake,src,clean)
	$(call do_submake,src/utils/erl,clean)
	$(call do_submake,src/sampleerl,clean)
	rmdir $(BINDIR)

$(BINDIR)/:
	mkdir -p $@