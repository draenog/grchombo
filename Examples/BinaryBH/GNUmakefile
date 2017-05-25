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
CATALYST_HOME := /home1/01249/gda/paraview/pv.osmesa.nox
cxxcppflags += -DUSE_CATALYST -I$(CATALYST_HOME)/include/paraview-5.3
syslibflags += -L$(CATALYST_HOME)/lib/paraview-5.3 -lvtkClientServer-pv5.3 -lvtkCommonCore-pv5.3 -lvtkCommonDataModel-pv5.3 -lvtkCommonExecutionModel-pv5.3 -lvtkCommonMath-pv5.3 -lvtkCommonMisc-pv5.3 -lvtkCommonSystem-pv5.3 -lvtkCommonTransforms-pv5.3 -lvtkFiltersCore-pv5.3 -lvtkFiltersExtraction-pv5.3 -lvtkFiltersGeneral-pv5.3 -lvtkFiltersGeometry-pv5.3 -lvtkFiltersModeling-pv5.3 -lvtkFiltersParallel-pv5.3 -lvtkFiltersProgrammable-pv5.3 -lvtkFiltersSources-pv5.3 -lvtkIOCore-pv5.3 -lvtkIOImage-pv5.3 -lvtkIOLegacy-pv5.3 -lvtkIOXMLParser-pv5.3 -lvtkParallelCore-pv5.3 -lvtkParallelCore-pv5.3 -lvtkParallelMPI-pv5.3 -lvtkParallelMPI-pv5.3 -lvtkPVCatalyst-pv5.3 -lvtkPVClientServerCoreCore-pv5.3 -lvtkPVCommon-pv5.3 -lvtkPVPythonCatalyst-pv5.3 -lvtkPVServerImplementationCore-pv5.3 -lvtkPVServerManagerApplication-pv5.3 -lvtkPVServerManagerCore-pv5.3 -lvtkPVVTKExtensionsCore-pv5.3 -lvtkPythonInterpreter-pv5.3 -lvtksys-pv5.3 -lvtkWrappingPython27Core-pv5.3
#-L/opt/apps/intel17/python/2.7.13/lib -lpython2.7
endif

include $(CHOMBO_HOME)/mk/Make.test
