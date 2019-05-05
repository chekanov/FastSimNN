# Makefile example for Promc+ROOT+fastjet
# S.Chekanov (ANL) 

ifndef ROOTSYS 
$(error ROOTSYS env variable is not set. Install ROOT first)
endif

ifndef FANN
$(error FANN env variable is not set. Install FANN first)
endif


include ${ROOTSYS}/etc/Makefile.arch

# Root variables
ROOTCFLAGS    = $(shell root-config --nonew --cflags)
ROOTLIBS      = $(shell root-config --nonew --libs)
ROOTGTTLIBS   = $(shell root-config --nonew --glibs)
# Assign or add variables
CXXFLAGS     += $(ROOTCFLAGS)
LIBS         += $(ROOTLIBS)
LIBS    += -L$(FANN)/  -lfloatfann -lfixedfann -ldoublefann -lfann 


INCLUDE1= -I./inc -I./
INCLUDE2= -I./src -I./lib/inc/
INCLUDE3= -I$(FANN)/include


Tasks:     clean ana

SOURCE_FILES := $(shell ls -1 ana.cc)
SOURCE_FILES += $(shell ls -1 src/*.cc)


# build object files 
objects       = $(patsubst %.cc,%.o,$(SOURCE_FILES))


%.o: %.cc
	$(CXX) $(OPT) -Wno-unused-variable  -Wunused-but-set-variable $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

LIBOBJS = $(patsubst %.cc,%.o,$(SOURCE_FILES))

ana: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
clean:
	        @rm -f *.o ana *~ src/*.o;  echo "Clear.." 
