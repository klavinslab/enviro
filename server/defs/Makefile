#Architecture
ARCH := $(shell uname -m)

#Compilers
CC          := g++ -std=c++17 -Wno-psabi

#The Target Library

#The Directories, Source, Includes, Objects, Binary and Resources
SRCEXT      := cc

# Directories
ELMADIR     := /development/elma
CHIPDIR     := /usr/local/src/Chipmunk2D

#Flags, Libraries and Includes
CFLAGS      := -ggdb  -shared -fPIC
INCLUDE		:= -I../include -I$(ELMADIR)/include -I $(CHIPDIR)/include/chipmunk 

#Files

TARGETDIR	 := .
SOURCES      := $(wildcard *.cc)
HEADERS      := $(wildcard *.h)
TARGETS		 := $(patsubst %.cc,%.so,$(wildcard *.cc))
FULL_TARGETS := $(addprefix $(TARGETDIR)/, $(TARGETS))

#Default Make
all: $(FULL_TARGETS)

#Clean only Objects
clean:
	@$(RM) -rf $(TARGETS)

# Compile
$(TARGETDIR)/%.so: %.cc %.h ../include/*.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

