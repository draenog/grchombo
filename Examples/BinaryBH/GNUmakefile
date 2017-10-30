# -*- Mode: Makefile -*-

### This makefile produces an executable for each name in the `ebase'
###  variable using the libraries named in the `LibNames' variable.

# Included makefiles need an absolute path to the Chombo installation
# CHOMBO_HOME := Please set the CHOMBO_HOME locally (e.g. export CHOMBO_HOME=... in bash)

ebase := GRChombo_BinaryBH

LibNames := AMRTimeDependent AMRTools BoxTools

src_dirs := ../../utils ../../simd ../../CCZ4 ../../InitialConditions/BlackHoles ../../FABDriver ../../GRChomboCore
ifdef USE_CATALYST
src_dirs += ../../GRChomboCore/Insitu
CATALYST_HOME := /nfs/software/apps/paraview/5.2.0-catalyst
cxxcppflags += -DUSE_CATALYST $(shell paraview-config  --include)
syslibflags += $(shell paraview-config --libs vtkClientServer) $(shell paraview-config --libs vtkCommonCore) $(shell paraview-config --libs vtkIOAMR) $(shell paraview-config --libs vtkPVPythonCatalyst)
#-L/opt/apps/intel17/python/2.7.13/lib -lpython2.7
endif

include $(CHOMBO_HOME)/mk/Make.test
