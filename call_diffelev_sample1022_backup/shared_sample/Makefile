# Output Binary Name
OUTBIN = sample_diffelev

# Commands
CC = gcc
CXX = g++
CP = cp -f
RM = rm -f

##### Objects
OBJS =	main.o \
	args.o

##### Preprocessor Defines
USR_DEF =

##### Include-path Directories
USR_INC_PATH = ./diffelev

##### Library-path Directories
USR_LIB_PATH = ./diffelev

##### Static Link Archives
USR_ARC =

##### Libraries
USR_LIBS = diffelev \
	reade57 \
	stdc++fs \
	pthread \
	xerces-c

##### Packages
USR_PKGS =

##### Other CPPFLAGS/CXXFLAGS/LDFLAGS (write as CXX argument)
USR_CPFLGS = --std=c++17
USR_CXFLGS =
USR_LDFLGS =

# add preprocessor-define to CPPFLAGS
CPPFLAGS += $(foreach DEF,$(USR_DEF),-D$(DEF))

# add other option to CPPFLAGS
CPPFLAGS += $(USR_CPFLGS)

# add include-path to CXXFLAGS
CXXFLAGS += $(foreach DIR,$(USR_INC_PATH),-I$(DIR))

# add other option to CXXFLAGS
CXXFLAGS += $(USR_CXFLGS)

# add package to CXXFLAGS
CXXFLAGS += $(foreach PKG,$(USR_PKGS),$(shell pkg-config $(PKG) --cflags))

# add other option to LDFLAGS
LDFLAGS += $(USR_LDFLGS)

# add library-path to LDFLAGS
LDFLAGS += $(foreach DIR,$(USR_LIB_PATH),-L$(DIR))

# add archive to LIBS
LIBS += $(USR_ARC)

# add library to LIBS
LIBS += $(foreach LIB,$(USR_LIBS),-l$(LIB))

# add package to LIBS
LIBS += $(foreach PKG,$(USR_PKGS),$(shell pkg-config $(PKG) --libs))

# Dependency Definition File
DEPEND_INC = depend.inc

# default
.PHONY: all
all: depend $(OUTBIN)	

########## Link
# link dependency files
LINK_DEPEND = $(OBJS)

# link command
LINK_COMMAND = $(CXX) $(LDFLAGS) -o $(OUTBIN) $(OBJS) $(LIBS);

# link rule
$(OUTBIN) : $(LINK_DEPEND)
	$(LINK_COMMAND)

# suffix rule
.SUFFIXES: .o .cpp

# create dependency file
.PHONY: depend
depend: $(OBJS:.o=.cpp)
	-@ $(RM) $(DEPEND_INC)
	-@ for i in $^; do\
		cpp $(CXXFLAGS) $(CPPFLAGS) -MM $$i | sed "s/\ [_a-zA-Z0-9][_a-zA-Z0-9]*\.cpp//g" >> $(DEPEND_INC);\
	done

-include $(DEPEND_INC)

########## Clean
CLEAN_TARGET = $(OBJS) $(OUTBIN) $(DEPEND_INC)
.PHONY: clean
clean:
	$(RM) $(CLEAN_TARGET)


