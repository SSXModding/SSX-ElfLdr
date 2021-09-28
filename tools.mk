# EE tool definitions and common Make rules

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

EECCFLAGS = $(EEWFLAGS) -D_EE $(EXTRADEFS) -Os -G0 -fomit-frame-pointer -ffunction-sections -fdata-sections $(EENOLIBGCC) -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(TOP)/include

# C++20 (without RTTI/exceptions) or Riot
EECXXFLAGS = -std=c++20 -fno-rtti -fno-exceptions -fno-threadsafe-statics $(EECCFLAGS)

# ld.bfd defaults to -z norelro
EELDFLAGS := -G0 -L$(PS2SDK)/ee/lib -L$(OBJDIR) -Wl,-zmax-page-size=128,--gc-sections,--build-id=none

# unused by my code
$(OBJDIR)/%.o: %.c
	$(EECC) $(EECFLAGS) -c $< -o $@

# TODO: generate .d files in $(OBJDIR)/ and include em here

$(OBJDIR)/%.o: %.cpp
	$(EECXX) $(EECXXFLAGS) -c $< -o $@
	
# mark our psuedotargets *as* psuedotargets
.PHONY: all clean

# These ifneq's seem weird, but.. this file is intended to be used
# to build multiple products:
# - a static library
# - a PS2 ELF (minified, of course :P)
# - (TODO) a "custom" ERL.
#
# Having these rules be standardized is actually quite a good thing
# and.. while I could have just used the ps2sdk makefiles to do so,
# I like this setup a bit more.

ifneq ($(LIB),)
# include rules for building static library

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

all: $(TOP)/$(BIN)

clean:
	rm $(TOP)/$(BIN) $(OBJS)

$(TOP)/$(BIN): $(OBJS)
	$(EECXX) $(EECXXFLAGS) $(EELDFLAGS) -T$(TOP)/ld/linkfile -o $@ $(OBJS) $(EEBIN_LIBS)
	$(EESTRIP) --strip-all $@
endif

# TODO: ERL