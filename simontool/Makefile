# Copyright (C) 2015  Brian Degnan (http://users.ece.gatech.edu/~degs)
# The Georgia Institute of Technology
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 

#DEBUG_MODE = DEBUG_MODE
#DEBUG_TARGET = DEBUG_ARBREG



#-------------------------------------------------------------------------------
#		Configuration
#-------------------------------------------------------------------------------

OPTIMIZATION = -g
OUTPUT=simontool
ARCH=x86


ifeq ($(OS),Windows_NT)
  BIN = bin
  OBJ = obj
  SRC = src
  
  SVNREVRAW := $(shell svnversion -n .)
  SVNREV := $(shell svnstrip.bat $(SVNREVRAW))

else
  BIN = bin
  OBJ = obj
  SRC = src
  
  SVNREV := $(shell svnversion -n . | sed -e "s/.*://" -e "s/\([0-9]*\).*/\1/" | grep "[0-9]" )
  
endif



#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------

#no prefix for non-cross compile
CROSS_COMPILE =
CC = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy

#-------------------------------------------------------------------------------
#		Configuration
#-------------------------------------------------------------------------------

CFLAGS = -W -Wall 
ifeq ($(DEBUG_MODE),DEBUG_MODE)
	CFLAGS += -D$(DEBUG_MODE) -D$(DEBUG_TARGET)
endif

#-mlong-calls -ffunction-sections
LDFLAGS =  $(OPTIMIZATION)

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------

C_OBJECTS = main.o simon.o
#C_OBJECTS = main_lfsr.o
C_OBJECTS += arbitrary_register.o utilities.o 
# This is for my internal version control, the external version may be different
ifdef SVNREV
$(warning )
$(warning SVN revision is $(SVNREV), which will be out of date without a svn update)
$(warning )
CFLAGS += -DSVNREVISION=$(SVNREV)
endif

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

OUTPUTDIR := $(BIN)/$(OUTPUT)

all: clean $(BIN) $(OBJ) $(ARCH)

cad: clean cadlogs $(BIN) $(OBJ) $(ARCH)

$(BIN) $(OBJ):
	mkdir $@

define RULES
C_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(C_OBJECTS))

$(1): $$(C_OBJECTS_$(1))
	$(CC) $(LDFLAGS) -o $(OUTPUTDIR).elf $$^
#	$(CC) $(LDFLAGS) -o $(OUTPUTDIR)-$$@.elf $$^
#	$(OBJCOPY) -O binary $(OUTPUTDIR)-$$@.elf $(OUTPUTDIR)-$$@.bin
#	$(SIZE) $$^ $(OUTPUTDIR)-$$@.elf

$$(C_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.c Makefile $(OBJ) $(BIN)
	$(CC) $(CFLAGS) -D$(1) -c -o $$@ $$<

$$(ASM_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.S Makefile $(OBJ) $(BIN)
	$(CC) $(ASFLAGS) -D$(1) -c -o $$@ $$<

endef
$(foreach ARCHCOMPILE, $(ARCH), $(eval $(call RULES,$(ARCHCOMPILE))))

cadlogs: 
     CFLAGS += -DCADLOGS


clean:
	-rm -f $(SRC)/*.s $(SRC)/*.i $(OBJ)/*.o $(BIN)/*.bin $(BIN)/*.elf
	-rm -f *.s
	-rm -f *.i
	-rm -f log*
	-rm -f *.tex

