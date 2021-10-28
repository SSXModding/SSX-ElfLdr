# EE tool definitions and common Make rules,
# used to simplify the build.

# EE target prefix
EEPREFIX=mips64r5900el-ps2-elf

# EE GNU tools:
# - C Compiler
# - C++ compiler
# - Librarian
# - Strip

EECC=$(EEPREFIX)-gcc
EECXX=$(EEPREFIX)-g++
EEAR=$(EEPREFIX)-ar
EESTRIP=$(EEPREFIX)-strip

# remove libgcc dependency
EENOLIBGCC = -fno-stack-protector -fno-ident -fno-unwind-tables -fno-asynchronous-unwind-tables

# Reasons for disabling warnings:
#  -Wno-stringop-overflow: gcc throws out nonsensical warnings because it doesn't know what I'm doing
#  -Wno-array-bounds: ditto
#
# all other warnings are fair game and should not be ignored,
# unless they are actually meaningless (in many cases, they are not.)
EEWFLAGS = -Wall -Wno-stringop-overflow -Wno-array-bounds

EECCFLAGS = $(EEWFLAGS) -D_EE $(EXTRADEFS) -G0 $(EENOLIBGCC) -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(TOP)/include

# GCC options to generate dependency information
EEDEPGEN = -MT $@ -MD -MP -MF $(OBJDIR)/$*.d

# C++20 (without RTTI/exceptions) or Riot
EECXXFLAGS = -std=c++20 -fno-rtti -fno-exceptions -fno-threadsafe-statics $(EECCFLAGS)

# ld.bfd defaults to -z norelro
EELDFLAGS = -G0 -L$(PS2SDK)/ee/lib -L$(BINDIR) -Wl,-zmax-page-size=128,--build-id=none

# If debug build is enabled,
# emit debugging info and remove size optimization flags.

ifeq ($(DEBUG),1)
BUILDSUFFIX = _debug
EECCFLAGS += -g -DDEBUG
else

# If we're building an ERL, these size pushing options actually break some stuff.
# Keep some of it off for ERL's.

ifeq ($(ERL),)
EECCFLAGS += -fomit-frame-pointer -ffunction-sections -fdata-sections -Os
EELDFLAGS += -Wl,--gc-sections
else
EECCFLAGS += -fomit-frame-pointer -Os
endif

endif

# Rules to build source files.

# unused by my code
$(OBJDIR)/%.o: %.c
	$(info Compiling C source file $<)
	$(EECC) $(EECFLAGS) $(EEDEPGEN) -c $< -o $@

# TODO: generate .d files in $(OBJDIR)/ and include them here

$(OBJDIR)/%.o: %.cpp
	$(info Compiling C++ source file $<)
	$(EECXX) $(EECXXFLAGS) $(EEDEPGEN) -c $< -o $@
	
# mark our psuedotargets *as* psuedotargets
.PHONY: all clean

# These ifneq's seem weird, but.. this make include file actually provides
# the targets to build multiple products, so the Makefiles don't have to.
#
# These are:
#  - an static library
#  - a PS2 ELF (minified, of course :P)
#  - a "custom" ERL ("custom" i.e no version/copyright/ident/depends info.).
#
# Having these rules be standardized is actually quite a good thing
# and.. while I could have just used the ps2sdk makefiles to do so,
# I like this setup a bit more, because it's a lot better and, most importantly,
# *one* include file.

ifneq ($(LIB),)
# include rules for building a static library
# TODO: LIB should only contain a name,
# we should gnuize/compose a filename here.

all: $(OBJDIR)/ $(BINDIR)/$(LIB)

clean:
	$(info Cleaning build products..)
	rm $(BINDIR)/$(LIB) $(OBJS) $(OBJDIR)/*.d
	rmdir $(OBJDIR)

$(BINDIR)/$(LIB): $(OBJS)
	$(info Creating static library $@)
	$(EEAR) r $@ $(OBJS)
endif

ifneq ($(BIN),)
# include rules for building standard PS2 ELF

# This complicated fun makes it so that I only include the libs I exactly need.
# LIBS specifies any and all libraries the ELF *NEEDS*.
# For simplicity, and, to avoid any *more* pain, we DO assume a libc,
# which needs to be specified in the product's Makefile.
EEBIN_LIBS = -nodefaultlibs -Wl,--start-group -l$(EELIBC) $(LIBS) -Wl,--end-group

all: $(OBJDIR)/ $(BINDIR)/$(BIN)$(BUILDSUFFIX).elf

clean:
	$(info Cleaning build products..)
	rm $(BINDIR)/$(BIN)$(BUILDSUFFIX).elf $(OBJS) $(OBJDIR)/*.d
	rmdir $(OBJDIR)

$(BINDIR)/$(BIN)$(BUILDSUFFIX).elf: $(OBJS)
	$(info Linking PS2 ELF $@)
	$(EECXX) $(EECXXFLAGS) $(EELDFLAGS) -T$(TOP)/ld/linkfile -o $@ $(OBJS) $(EEBIN_LIBS)
ifneq ($(DEBUG),1)
	$(info Stripping ELF $@ since this is a release build)
	$(EESTRIP) --strip-all $@
endif
endif

ifneq ($(ERL),)
# include rules for building PS2 ERL file

all: $(OBJDIR)/ $(BINDIR)/$(ERL)$(BUILDSUFFIX).erl

clean:
	$(info Cleaning build products..)
	rm $(BINDIR)/$(ERL)$(BUILDSUFFIX).erl $(OBJS) $(OBJDIR)/*.d
	rmdir $(OBJDIR)
	
$(BINDIR)/$(ERL)$(BUILDSUFFIX).erl: $(OBJS)
	$(info Linking ERL $@)
	$(EECC) $(EECCFLAGS) $(EELDFLAGS) -nostartfiles -nodefaultlibs -o $@ $(OBJS) $(LIBS) -Wl,-r
ifneq ($(DEBUG),1)
	$(info Stripping ERL $@ since this is a release build)
	$(EESTRIP) --strip-unneeded -R .mdebug.eabi64 -R .reginfo -R .comment $@
endif
endif

# fun make tricks, episode # 27,402
$V.SILENT:

# needs to be below all these other rules,
# otherwise it physically will not fire.
# just make things
$(OBJDIR)/:
	mkdir -p $@

# include .d files generated by gcc
-include $(wildcard $(OBJDIR)/*.d)