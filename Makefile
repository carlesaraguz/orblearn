#####################################################################################################
# ENERGY MANAGER MAKEFILE                                                      v1.5.5 (17-nov-2014) #
#####################################################################################################

# Application that you want to compile and link. Must have the same name as C file containing
# the 'main()' function.
APPLICATION = orbprop

# Source files (including the main C file)
SOURCES = orbprop.cpp \
          cOrbit.cpp \
          cEci.cpp \
          cJulian.cpp \
          cNoradBase.cpp \
          cNoradSDP4.cpp \
          cNoradSGP4.cpp \
          coord.cpp \
          cSite.cpp \
          cTle.cpp \
          cVector.cpp \
          globals.cpp


# Paths to search the sources (separated by a colon ':')
VPATH = orbitTools/core:orbitTools/orbit

# Extra Compiler and Linker Flags:
EXTRACFLAGS = -I./orbitTools/core -I./orbitTools/orbit
EXTRALDFLAGS =

# Default configuration:
CC_HOST=g++#        # Compiler for your architecture (usually: gcc)


#####################################################################################################
#####################################################################################################
# Makefile configuration. Do not modify.
MV            := 1.5.5
TOOLCHAIN     := $(CC_HOST)
BINDIR        := .
OBJDIR        := obj
OBJS          := $(addprefix $(OBJDIR)/,$(SOURCES:%.cpp=%.o))
OBJSLF         = $(addprefix '\n------------:',$(OBJS)])
CC_BASE_DIR   := $(subst -g++,,$(TOOLCHAIN))
CC_BASE_DIR   := $(subst g++,,$(CC_BASE_DIR))
ifneq (,$(CC_BASE_DIR))
    CC_BASE_DIR := $(CC_BASE_DIR)/
endif
BASIC_CFLAGS   = -Wall -Wno-reorder -std=c++11


ifneq ($(CONF),quiet)
all: show_config | $(APPLICATION)
else
all: $(APPLICATION)
endif

$(OBJDIR)/%.o : %.cpp
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@$(TOOLCHAIN) -c $< -o $@ $(BASIC_CFLAGS) $(EXTRACFLAGS) && echo 'done.'

$(APPLICATION) : $(OBJS) | $(BINDIR) $(OBJDIR)
	@echo -n -e '---------: LINKING : '
	@$(TOOLCHAIN) $(OBJS) -o $@ $(BASIC_LDFLAGS) $(EXTRALDFLAGS) && echo 'done.'

$(OBJS): | $(BINDIR) $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

show_config:
	@echo '---------: APPLICATION  : $(APPLICATION)'
	@echo '---------: OBJS         : $(SOURCES:%.c=%.o)'
	@echo '---------: TOOLCHAIN    : $(TOOLCHAIN)'
	@echo '---------: EXTRACFLAGS  : $(EXTRACFLAGS)'
	@echo '---------: EXTRALDFLAGS : $(EXTRALDFLAGS)'
	@echo '---------: CONFIG.      : $(CONF)'
	@echo '---------: MAKEFILE VER.: $(MV)'
	@echo -n '---------: ' && date +"%Y/%m/%d   : %T"

clean:
	@echo -n '---------: REMOVING $(BINDIR)/$(APPLICATION)...' && rm $(BINDIR)/$(APPLICATION) -f && echo 'done.'
	@echo -n '---------: REMOVING $(OBJDIR)...' && rm $(OBJDIR) -r -f && echo 'done.'

cleanall: | clean
	@echo -n '---------: REMOVING old propagations...' && rm propagations -rf && echo 'done.'
