#
# Release: This file is part of the openkim-api.git repository.
#
# Copyright 2011 Ellad B. Tadmor, Ryan S. Elliott, and James P. Sethna
# All rights reserved.
#
# Authors: Valeriu Smirichinski, Ryan S. Elliott, Ellad B. Tadmor
#
# This make file needs to be included by the makefiles in
# the KIM_API, MODELs/*/, MODEL_DRIVERs/*/ and TESTs/*/ directories.
# It contains definitions for the GNU and Intel compiler sets.
# It also contains definitions for patern compilation rules.
#

ifndef KIM_DIR
   $(error The environment variable KIM_DIR is not defined.)
endif

# The openkim-api subdirectories:
ifndef KIM_API_DIR
   KIM_API_DIR :=$(KIM_DIR)KIM_API/
endif
ifndef KIM_MODELS_DIR
   KIM_MODELS_DIR:=$(KIM_DIR)MODELs/
endif
ifndef KIM_MODEL_DRIVERS_DIR
   KIM_MODEL_DRIVERS_DIR:=$(KIM_DIR)MODEL_DRIVERs/
endif
ifndef KIM_TESTS_DIR
   KIM_TESTS_DIR:=$(KIM_DIR)TESTs/
endif


# All Model .o files and library definitions
# (In the future, this will be generated by the processing pipeline)

# setup list of available models
MODELLST = $(patsubst .%.make-temp,%,$(notdir $(wildcard $(KIM_API_DIR).*.make-temp)))
MODELDRIVERLST = $(notdir $(filter-out .%,$(shell find $(KIM_MODEL_DRIVERS_DIR) -maxdepth 1 -mindepth 1 -type d -exec basename {} \;)))
MODELOBJ = $(addprefix $(KIM_MODELS_DIR), $(join $(addsuffix /,$(MODELLST)), $(addsuffix .a, $(MODELLST)))) \
           $(addprefix $(KIM_MODEL_DRIVERS_DIR), $(join $(addsuffix /,$(MODELDRIVERLST)), $(addsuffix .a, $(MODELDRIVERLST))))

# Determine whether a 32 bit or 64 bit compile should be use
ifdef KIM_SYSTEM32
   MACHINESYSTEM=SYSTEM32
else
   MACHINESYSTEM=SYSTEM64
endif

ifdef KIM_INTEL
   # Define Intel compiler switches
   OBJONLY=-c
   OUTPUTIN=-o
   FORTRANFLAG =-I $(KIM_API_DIR) -D $(MACHINESYSTEM)                     \
                                  -D KIM_DIR_API=\"$(KIM_API_DIR)\"       \
                                  -D KIM_DIR_MODELS=\"$(KIM_MODELS_DIR)\" \
                                  -D KIM_DIR_TESTS=\"$(KIM_TESTS_DIR)\"   \
                                  -D KIM_DIR_MODEL_DRIVERS=\"$(KIM_MODEL_DRIVERS_DIR)\"
   CCOMPILER = icc
   CPPCOMPILER = icpc
   CPPFLAG = -O3 -I$(KIM_API_DIR) -D KIM_DIR_API=\"$(KIM_API_DIR)\"       \
                                  -D KIM_DIR_MODELS=\"$(KIM_MODELS_DIR)\" \
                                  -D KIM_DIR_TESTS=\"$(KIM_TESTS_DIR)\"   \
                                  -D KIM_DIR_MODEL_DRIVERS=\"$(KIM_MODEL_DRIVERS_DIR)\"
   CPPLIBFLAG = -nofor_main -cxxlib
   FORTRANLIBFLAG = -cxxlib
   FORTRANCOMPILER = ifort
   LINKCOMPILER = $(FORTRANCOMPILER)
else
   # Define GNU compiler switches
   OBJONLY=-c
   OUTPUTIN=-o
   FORTRANFLAG = -fcray-pointer -O3 -I$(KIM_API_DIR) -D $(MACHINESYSTEM)                     \
                                                     -D KIM_DIR_API=\"$(KIM_API_DIR)\"       \
                                                     -D KIM_DIR_MODELS=\"$(KIM_MODELS_DIR)\" \
                                                     -D KIM_DIR_TESTS=\"$(KIM_TESTS_DIR)\"   \
                                                     -D KIM_DIR_MODEL_DRIVERS=\"$(KIM_MODEL_DRIVERS_DIR)\"
   CCOMPILER   = gcc
   CPPCOMPILER = g++
   #CCOMPILER   = mpicc
   #CPPCOMPILER = mpiCC
   #CCOMPILER   = gcc-fsf-4.4  # for OS X using fink compilers
   #CPPCOMPILER = g++-fsf-4.4  # for OS X using fink compilers
   CPPFLAG = -O3 -I$(KIM_API_DIR) -Wno-write-strings -D KIM_DIR_MODELS=\"$(KIM_MODELS_DIR)\" \
                                                     -D KIM_DIR_API=\"$(KIM_API_DIR)\"       \
                                                     -D KIM_DIR_TESTS=\"$(KIM_TESTS_DIR)\"   \
                                                     -D KIM_DIR_MODEL_DRIVERS=\"$(KIM_MODEL_DRIVERS_DIR)\"
   FORTRANCOMPILER = gfortran
   #FORTRANCOMPILER = mpif90
   #CPPLIBFLAG = -lgfortran           #if GNU version 4.5 and up. tested on suse 
   #LINKCOMPILER = $(CPPCOMPILER)     #if GNU version 4.5 and up. tested on suse
   CPPLIBFLAG = -lstdc++             #if GNU version 4.4.1. tested on suse
   LINKCOMPILER = $(FORTRANCOMPILER) #if GNU version 4.4.1. tested on suse
   ifdef KIM_DYNAMIC
      CPPLIBFLAG += -ldl
   endif
endif

# directory where the kim.log file should be created
# CPPFLAG += -D KIM_DIR=\"$(KIM_DIR)\"

# Set max neighbors
ifdef KIM_API_MAX_NEIGHBORS #MAX NEIGHBORS FOR AN ATOM default is 512
   CPPFLAG += -D KIM_API_MAX_NEIGHBORS=$(KIM_API_MAX_NEIGHBORS)
endif

# Set common compiler flags for dynamic linking
ifdef KIM_DYNAMIC
   CPPFLAG += -D KIM_DYNAMIC=\"$(KIM_DYNAMIC)\" -fPIC
   FORTRANFLAG += -fPIC
endif

KIM_LIB_FILE = $(KIM_API_DIR)/libkim.a
KIM_LIB = -L$(KIM_API_DIR) -lkim

#set default goals allways all
.DEFAULT_GOAL := all

#build target .a or .so for models
ifdef KIM_DYNAMIC
   MODEL_BUILD_TARGET += $(patsubst %.a,%.so, $(MODEL_BUILD_TARGET))
   MODEL_DRIVER_BUILD_TARGET += $(patsubst %.a,%.so, $(MODEL_DRIVER_BUILD_TARGET))
   SHARED_LIB_FLAG = -shared
   ifeq ($(OSTYPE),FreeBSD)
      SHARED_LIB_FLAG = -dynamiclib
   endif
endif


# Definition of c and fortran .o file list
OBJC = KIMservice.o KIMserviceC.o  
OBJF90 = KIMserviceF.o

#fortran on/of
ifdef KIM_NO_FORTRAN
   ALLOBJ = $(OBJC)
else
   ALLOBJ = $(OBJC) $(OBJF90)
endif


# C/C++ Compiler pattern rules
%.o:%.c    # C with preprocessing
	$(CCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.i    # C without preprocessing
	$(CCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.cpp  # C++ with preprocessing
	$(CPPCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.ii   # C++ without preprocessing
	$(CPPCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.cc   # C++ with preprocessing
	$(CPPCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.cxx  # C++ with preprocessing
	$(CPPCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.cpp  # C++ with preprocessing
	$(CPPCOMPILER) $(CPPFLAG) $(OBJONLY) $<
%.o:%.C    # C++ with preprocessing
	$(CPPCOMPILER) $(CPPFLAG) $(OBJONLY) $<

# Fortran Compiler pattern rules
# Fixed form code
%.o:%.f    # FORTRAN 77 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.for  # FORTRAN 77 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.ftn  # FORTRAN 77 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.fpp  # FORTRAN 77 with preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.F    # FORTRAN 77 with preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.FOR  # FORTRAN 77 with preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.FTN  # FORTRAN 77 with preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.FPP  # FORTRAN 77 with preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
# Free form code
%.o:%.f90  # Fortran 90 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.f95  # Fortran 95 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.f03  # Fortran 2003 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.f08  # Fortran 2008 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.F90  # Fortran 90 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.F95  # Fortran 95 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.F03  # Fortran 2003 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<
%.o:%.F08  # Fortran 2008 without preprocessing
	$(FORTRANCOMPILER) $(FORTRANFLAG) $(OBJONLY) $<


#
MODEL_NAME_KIM_STR_H = char* $(MODEL_NAME)_kim_str'('')'';'
MODEL_NAME_KIM_STR_CPP = char* $(MODEL_NAME)_kim_str'('')''{' 
%_kim_str.cpp: %.kim
	echo "extern \"C\" {"           > $*_kim_str.cpp
	echo $(MODEL_NAME_KIM_STR_H)   >> $*_kim_str.cpp
	echo "}"                       >> $*_kim_str.cpp
	echo $(MODEL_NAME_KIM_STR_CPP) >> $*_kim_str.cpp
	echo "static char kimstr[] ="  >> $*_kim_str.cpp
	cat $(MODEL_NAME).kim | \
	sed -e 's,\\,\\\\,g'     \
            -e 's,",\\",g'       \
            -e 's,^,      ",g'   \
            -e 's,$$,\\n",g'           >> $*_kim_str.cpp
	echo "   ;"                    >> $*_kim_str.cpp
	echo "return &kimstr[0];"      >> $*_kim_str.cpp
	echo ""                        >> $*_kim_str.cpp
	echo "}"                       >> $*_kim_str.cpp

# Library pattern rule
%.so: %.a $(KIM_LIB_FILE) 
	$(LINKCOMPILER) $(SHARED_LIB_FLAG)  $(CPPLIBFLAG) -o $@  *.o $(addprefix $(KIM_API_DIR),$(ALLOBJ))
