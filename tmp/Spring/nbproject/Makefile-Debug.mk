#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AnEn.o \
	${OBJECTDIR}/AnEnRead.o \
	${OBJECTDIR}/AnEnReadNcdf.o \
	${OBJECTDIR}/AnEnWriteNcdf.o \
	${OBJECTDIR}/Analogs.o \
	${OBJECTDIR}/BasicData.o \
	${OBJECTDIR}/Forecasts.o \
	${OBJECTDIR}/ForecastsArray.o \
	${OBJECTDIR}/Functions.o \
	${OBJECTDIR}/Ncdf.o \
	${OBJECTDIR}/Observations.o \
	${OBJECTDIR}/ObservationsArray.o \
	${OBJECTDIR}/Parameters.o \
	${OBJECTDIR}/SimilarityMatrices.o \
	${OBJECTDIR}/StandardDeviation.o \
	${OBJECTDIR}/Stations.o \
	${OBJECTDIR}/Times.o \
	${OBJECTDIR}/Txt.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++11
CXXFLAGS=-std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spring

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spring: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spring ${OBJECTFILES} ${LDLIBSOPTIONS} -lboost_system -lboost_filesystem -lnetcdf-cxx4

${OBJECTDIR}/AnEn.o: AnEn.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEn.o AnEn.cpp

${OBJECTDIR}/AnEnRead.o: AnEnRead.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEnRead.o AnEnRead.cpp

${OBJECTDIR}/AnEnReadNcdf.o: AnEnReadNcdf.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEnReadNcdf.o AnEnReadNcdf.cpp

${OBJECTDIR}/AnEnWriteNcdf.o: AnEnWriteNcdf.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEnWriteNcdf.o AnEnWriteNcdf.cpp

${OBJECTDIR}/Analogs.o: Analogs.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Analogs.o Analogs.cpp

${OBJECTDIR}/BasicData.o: BasicData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BasicData.o BasicData.cpp

${OBJECTDIR}/Forecasts.o: Forecasts.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Forecasts.o Forecasts.cpp

${OBJECTDIR}/ForecastsArray.o: ForecastsArray.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ForecastsArray.o ForecastsArray.cpp

${OBJECTDIR}/Functions.o: Functions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions.o Functions.cpp

${OBJECTDIR}/Ncdf.o: Ncdf.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Ncdf.o Ncdf.cpp

${OBJECTDIR}/Observations.o: Observations.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Observations.o Observations.cpp

${OBJECTDIR}/ObservationsArray.o: ObservationsArray.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ObservationsArray.o ObservationsArray.cpp

${OBJECTDIR}/Parameters.o: Parameters.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Parameters.o Parameters.cpp

${OBJECTDIR}/SimilarityMatrices.o: SimilarityMatrices.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SimilarityMatrices.o SimilarityMatrices.cpp

${OBJECTDIR}/StandardDeviation.o: StandardDeviation.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StandardDeviation.o StandardDeviation.cpp

${OBJECTDIR}/Stations.o: Stations.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stations.o Stations.cpp

${OBJECTDIR}/Times.o: Times.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Times.o Times.cpp

${OBJECTDIR}/Txt.o: Txt.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Txt.o Txt.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
