# Makefile for elfldr

# EE tools
EEPREFIX=mips64r5900el-ps2-elf
EECC=$(EEPREFIX)-gcc
EECXX=$(EEPREFIX)-g++
EESTRIP=$(EEPREFIX)-strip

# remove libgcc dependency
EENOLIBGCC = -fno-stack-protector -fno-ident -fno-unwind-tables -fno-asynchronous-unwind-tables

# Reasons for disabling warnings:
#  -Wno-stringop-overflow: gcc throws out nonsensical warnings because it doesn't know what I'm doing
#  -Wno-array-bounds: ditto
EEWFLAGS = -Wall -Wno-stringop-overflow -Wno-array-bounds

EECCFLAGS = $(EEWFLAGS) -D_EE -Os -G0 $(EENOLIBGCC) -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include

# C++20 (without RTTI/exceptions) or Riot
EECXXFLAGS = -std=c++20 -fno-rtti -fno-exceptions $(EECCFLAGS)

EELDFLAGS := -L$(PS2SDK)/ee/lib -Wl,-zmax-page-size=128

SSX3=0
EXPERIMENTAL=0

ifeq ($(SSX3),1)
BIN = ssx_elfldr_ssx3.elf
OBJDIR = obj_ssx3
else
BIN = ssx_elfldr.elf
OBJDIR = obj
endif

# Base framework objects
OBJS = $(OBJDIR)/main.o \
	   $(OBJDIR)/codeutils.o \
	   $(OBJDIR)/utils.o \
	   $(OBJDIR)/ElfLoader.o \
	   $(OBJDIR)/patch.o

# Game/configuration specific objects + ccflags

ifeq ($(EXPERIMENTAL),1)
OBJS += $(OBJDIR)/patch_experimental.o
EECCFLAGS += -DEXPERIMENTAL
FEATURES += experimental
endif

ifeq ($(SSX3),1)
OBJS += $(OBJDIR)/patch_hostfs_ssx3.o
EECCFLAGS += -DSSX3
else
# OG objects
OBJS += \
	   $(OBJDIR)/patch_hostfs.o \
	   $(OBJDIR)/patch_memory.o
endif

# Set to `c` to use regular newlib.
# not that you'd really need to, but the option is there!
EELIBC = c_nano
EEPS2SDK_LIBS = -lcdvd -lkernel -lps2sdkc

# This complicated fun makes it so that I only include the libs I exactly need.
LIBS = -nodefaultlibs -Wl,--start-group -l$(EELIBC) $(EEPS2SDK_LIBS) -Wl,--end-group

all: $(BIN)

clean:
	rm -rf $(BIN) $(OBJDIR)

# compile rules	

# unused
#$(OBJDIR)/%.o: src/%.c
#	$(EECC) $(EECFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.cpp
	$(EECXX) $(EECXXFLAGS) -c $< -o $@

$(OBJDIR)/:
	mkdir -p $@

# Link rule
$(BIN): $(OBJDIR)/ $(OBJS)
	$(EECC) -T./ld/linkfile -o $@ $(OBJS) $(EELDFLAGS) $(LIBS)
	$(EESTRIP) --strip-all $@
