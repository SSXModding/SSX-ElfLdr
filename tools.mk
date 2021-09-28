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

# I'd like to add -flto, but unfortunately,
# doing so breaks libutils for some reason????????
# ah.. ps2sdk binutils can't do lto objects. cool.

# remove libgcc dependency
EENOLIBGCC = -fno-stack-protector -fno-ident -fno-unwind-tables -fno-asynchronous-unwind-tables

# Reasons for disabling warnings:
#  -Wno-stringop-overflow: gcc throws out nonsensical warnings because it doesn't know what I'm doing
#  -Wno-array-bounds: ditto
EEWFLAGS = -Wall -Wno-stringop-overflow -Wno-array-bounds

EECCFLAGS = $(EEWFLAGS) -D_EE $(EXTRADEFS) -G0 $(EENOLIBGCC) -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(TOP)/include

# C++20 (without RTTI/exceptions) or Riot
EECXXFLAGS = -std=c++20 -fno-rtti -fno-exceptions -fno-threadsafe-statics $(EECCFLAGS)

# ld.bfd defaults to -z norelro
EELDFLAGS = -G0 -L$(PS2SDK)/ee/lib -L$(OBJDIR) -Wl,-zmax-page-size=128,--build-id=none

# If debug build is enabled,
# emit debugging info and remove size optimization flags.

ifeq ($(DEBUG),1)
BUILDSUFFIX = _debug
EECCFLAGS += -g -DDEBUG
else

ifeq ($(ERL),)
EECCFLAGS += -fomit-frame-pointer -ffunction-sections -fdata-sections -Os
EELDFLAGS += -Wl,--gc-sections
else
# if we're building an ERL, the size pushing options actually break some stuff.
EECCFLAGS += -fomit-frame-pointer -Os
endif
endif

# unused by my code
$(OBJDIR)/%.o: %.c
	$(EECC) $(EECFLAGS) -c $< -o $@

# TODO: generate .d files in $(OBJDIR)/ and include them here

$(OBJDIR)/%.o: %.cpp
	$(EECXX) $(EECXXFLAGS) -c $< -o $@
	
# mark our psuedotargets *as* psuedotargets
.PHONY: all clean

# These ifneq's seem weird, but.. this file is intended to be used
# to build multiple products:
# - a static library
# - a PS2 ELF (minified, of course :P)
# - a "custom" ERL.
#
# Having these rules be standardized is actually quite a good thing
# and.. while I could have just used the ps2sdk makefiles to do so,
# I like this setup a bit more.

ifneq ($(LIB),)
# include rules for building a static library

all: $(OBJDIR)/$(LIB)

clean:
	rm $(OBJDIR)/$(LIB) $(OBJS)

$(OBJDIR)/$(LIB): $(OBJS)
	$(EEAR) r $@ $(OBJS)
endif

ifneq ($(BIN),)
# include rules for building standard PS2 ELF

# This complicated fun makes it so that I only include the libs I exactly need.
# LIBS specifies any and all libraries the ELF *NEEDS*.
EEBIN_LIBS = -nodefaultlibs -Wl,--start-group -l$(EELIBC) $(LIBS) -Wl,--end-group

all: $(BINDIR)/$(BIN)$(BUILDSUFFIX).elf

clean:
	rm $(BINDIR)/$(BIN)$(BUILDSUFFIX).elf $(OBJS)

$(BINDIR)/$(BIN)$(BUILDSUFFIX).elf: $(OBJS)
	$(EECXX) $(EECXXFLAGS) $(EELDFLAGS) -T$(TOP)/ld/linkfile -o $@ $(OBJS) $(EEBIN_LIBS)
ifneq ($(DEBUG),1)
	$(EESTRIP) --strip-all $@
endif
endif

ifneq ($(ERL),)
# include rules for building PS2 ERL file

all: $(BINDIR)/$(ERL)$(BUILDSUFFIX).erl

clean:
	rm $(BINDIR)/$(ERL)$(BUILDSUFFIX).erl
	
$(BINDIR)/$(ERL)$(BUILDSUFFIX).erl: $(OBJS)
	$(EECC) $(EECCFLAGS) $(EELDFLAGS) -nostartfiles -nodefaultlibs -o $@ $(OBJS) $(LIBS) -Wl,-r -Wl,-d
ifneq ($(DEBUG),1)
	$(EESTRIP) --strip-unneeded -R .mdebug.eabi64 -R .reginfo -R .comment $@
endif
endif