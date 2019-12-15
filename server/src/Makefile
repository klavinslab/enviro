#Compilers
CC          := g++ -std=c++14 -Wno-psabi
DGEN        := doxygen

#The Target Binary Program
TARGET      := enviro

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := .
INCDIR      := ../include
BUILDDIR    := .
TARGETDIR   := ../bin
SRCEXT      := cc
CHIPDIR     := /usr/local/src/Chipmunk2D
ELMADIR     := /development/elma

#Flags, Libraries and Includes
CFLAGS      := -ggdb
LIB         := -lgtest -lpthread -lelma -lchipmunk -lssl -lcrypto 
INC         := -I $(INCDIR) -I $(CHIPDIR)/include/chipmunk -I $(ELMADIR)/include
LIBDIR      := -L $(CHIPDIR)/build/src -L $(ELMADIR)/lib
INCDEP      := -I$(INCDIR)/build/src/ $(CHIPDIR)/

#Files
DGENCONFIG  := docs.config
HEADERS     := $(wildcard $(INCDIR)/*.h)
SOURCES     := $(wildcard $(SRCDIR)/*.cc)
OBJECTS     := $(patsubst %.cc, $(BUILDDIR)/%.o, $(notdir $(SOURCES)))

#Defauilt Make
all: $(TARGETDIR)/$(TARGET) 
	echo $(SOURCES) $(HEADERS)

#Remake
remake: cleaner all

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)/*.o

#Full Clean, Objects and Binaries
spotless: clean
	@$(RM) -rf $(TARGETDIR)/$(TARGET) 

#Link
$(TARGETDIR)/$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGETDIR)/$(TARGET) $(LIBDIR) $(OBJECTS) $(LIB) 
	
#Compile
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(HEADERS)
	$(CC) $(CFLAGS) $(INC) -c -fPIC -o $@ $<
