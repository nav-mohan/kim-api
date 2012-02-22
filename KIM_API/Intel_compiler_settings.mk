#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the Common Development
# and Distribution License Version 1.0 (the "License").
#
# You can obtain a copy of the license at
# http://www.opensource.org/licenses/CDDL-1.0.  See the License for the
# specific language governing permissions and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each file and
# include the License file in a prominent location with the name LICENSE.CDDL.
# If applicable, add the following below this CDDL HEADER, with the fields
# enclosed by brackets "[]" replaced with your own identifying information:
#
# Portions Copyright (c) [yyyy] [name of copyright owner]. All rights reserved.
#
# CDDL HEADER END
#

#
# Copyright (c) 2012, Regents of the University of Minnesota.  All rights reserved.
#
# Contributors:
#    Valeriu Smirichinski
#    Ryan S. Elliott
#    Ellad B. Tadmor
#

#
# Release: This file is part of the openkim-api.git repository.
#


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

