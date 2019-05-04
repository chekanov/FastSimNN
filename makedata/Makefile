# S.Chekanov


ifndef ROOTSYS 
$(error ROOTSYS env variable is not set. Install ROOT first)
endif

ifndef DELPHES
$(error DELPHES env variable is not set. Install DELPHES first)
endif

ifndef LIBCONFIG
$(error LIBCONFIG env variable is not set. Install LIBCONFIG first)
endif


include ${ROOTSYS}/etc/Makefile.arch

# Root variables
ROOTCFLAGS    = $(shell root-config --nonew --cflags)
ROOTLIBS      = $(shell root-config --nonew --libs)
ROOTGTTLIBS   = $(shell root-config --nonew --glibs)
# Assign or add variables
CXXFLAGS     += $(ROOTCFLAGS)
LIBS         += $(ROOTLIBS) -lEG
LIBS         += -Wl,-rpath,$(DELPHES) -L$(DELPHES) -lDelphes
LIBS         +=  -L$(LIBCONFIG)/lib -lconfig -lconfig++ 

INCLUDE1= -I./inc -I. -I$(DELPHES) -I$(DELPHES)/external
INCLUDE2= -I./src -I$(FANN)/include -I$(LIBCONFIG)/include
 

Tasks:     clean mydict analysis 

SOURCE_FILES := $(shell ls -1 *.cxx src/*.cxx)
objects       = $(patsubst %.cxx,%.o,$(SOURCE_FILES))


mydict: inc/LParticle.h
	@rm -f src/CParticle_dict*
	@rm -f inc/CParticle_dict*
	@echo "Generating dictionary for CParticle"
	@rootcint src/CParticle_dict.cxx -c inc/CParticle.h
	@rm -f src/LParticle_dict*
	@rm -f inc/LParticle_dict*
	@echo "Generating dictionary for LParticle"
	@rootcint src/LParticle_dict.cxx -c inc/LParticle.h


%.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

analysis: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
clean:
	        @rm -f *.o *~ analysis; rm -f src/*.o;  echo "Clear.." 
