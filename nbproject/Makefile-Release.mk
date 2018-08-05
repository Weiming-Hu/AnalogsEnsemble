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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include AnalogsEnsemble-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AnEn.o \
	${OBJECTDIR}/AnEnIO.o \
	${OBJECTDIR}/Array4D.o \
	${OBJECTDIR}/Forecasts.o \
	${OBJECTDIR}/Observations.o \
	${OBJECTDIR}/Parameters.o \
	${OBJECTDIR}/SimilarityMatrices.o \
	${OBJECTDIR}/StandardDeviation.o \
	${OBJECTDIR}/Stations.o \
	${OBJECTDIR}/Times.o \
	${OBJECTDIR}/canalogs.o \
	${OBJECTDIR}/doxygen-mainpage.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f8 \
	${TESTDIR}/TestFiles/f4 \
	${TESTDIR}/TestFiles/f5 \
	${TESTDIR}/TestFiles/f3 \
	${TESTDIR}/TestFiles/f6 \
	${TESTDIR}/TestFiles/f7 \
	${TESTDIR}/TestFiles/f1 \
	${TESTDIR}/TestFiles/f2

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/tests/runnerAnEn.o \
	${TESTDIR}/tests/runnerAnEnIO.o \
	${TESTDIR}/tests/runnerForecastsArray.o \
	${TESTDIR}/tests/runnerObservationsArray.o \
	${TESTDIR}/tests/runnerSimilarityMatrices.o \
	${TESTDIR}/tests/runnerStandardDeviation.o \
	${TESTDIR}/tests/runnerStation.o \
	${TESTDIR}/tests/runnerStations.o \
	${TESTDIR}/tests/testAnEn.o \
	${TESTDIR}/tests/testAnEnIO.o \
	${TESTDIR}/tests/testForecastsArray.o \
	${TESTDIR}/tests/testObservationsArray.o \
	${TESTDIR}/tests/testSimilarityMatrices.o \
	${TESTDIR}/tests/testStandardDeviation.o \
	${TESTDIR}/tests/testStation.o \
	${TESTDIR}/tests/testStations.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/analogsensemble

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/analogsensemble: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/analogsensemble ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/AnEn.o: AnEn.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEn.o AnEn.cpp

${OBJECTDIR}/AnEnIO.o: AnEnIO.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEnIO.o AnEnIO.cpp

${OBJECTDIR}/Array4D.o: Array4D.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Array4D.o Array4D.cpp

${OBJECTDIR}/Forecasts.o: Forecasts.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Forecasts.o Forecasts.cpp

${OBJECTDIR}/Observations.o: Observations.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Observations.o Observations.cpp

${OBJECTDIR}/Parameters.o: Parameters.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Parameters.o Parameters.cpp

${OBJECTDIR}/SimilarityMatrices.o: SimilarityMatrices.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SimilarityMatrices.o SimilarityMatrices.cpp

${OBJECTDIR}/StandardDeviation.o: StandardDeviation.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StandardDeviation.o StandardDeviation.cpp

${OBJECTDIR}/Stations.o: Stations.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stations.o Stations.cpp

${OBJECTDIR}/Times.o: Times.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Times.o Times.cpp

${OBJECTDIR}/canalogs.o: canalogs.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/canalogs.o canalogs.cpp

${OBJECTDIR}/doxygen-mainpage.o: doxygen-mainpage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/doxygen-mainpage.o doxygen-mainpage.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f8: ${TESTDIR}/tests/runnerAnEn.o ${TESTDIR}/tests/testAnEn.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f8 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f4: ${TESTDIR}/tests/runnerAnEnIO.o ${TESTDIR}/tests/testAnEnIO.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f4 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f5: ${TESTDIR}/tests/runnerForecastsArray.o ${TESTDIR}/tests/testForecastsArray.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f5 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f3: ${TESTDIR}/tests/runnerObservationsArray.o ${TESTDIR}/tests/testObservationsArray.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f3 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f6: ${TESTDIR}/tests/runnerSimilarityMatrices.o ${TESTDIR}/tests/testSimilarityMatrices.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f6 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f7: ${TESTDIR}/tests/runnerStandardDeviation.o ${TESTDIR}/tests/testStandardDeviation.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f7 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/runnerStation.o ${TESTDIR}/tests/testStation.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   

${TESTDIR}/TestFiles/f2: ${TESTDIR}/tests/runnerStations.o ${TESTDIR}/tests/testStations.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS}   `cppunit-config --libs`   


${TESTDIR}/tests/runnerAnEn.o: tests/runnerAnEn.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerAnEn.o tests/runnerAnEn.cpp


${TESTDIR}/tests/testAnEn.o: tests/testAnEn.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testAnEn.o tests/testAnEn.cpp


${TESTDIR}/tests/runnerAnEnIO.o: tests/runnerAnEnIO.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerAnEnIO.o tests/runnerAnEnIO.cpp


${TESTDIR}/tests/testAnEnIO.o: tests/testAnEnIO.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testAnEnIO.o tests/testAnEnIO.cpp


${TESTDIR}/tests/runnerForecastsArray.o: tests/runnerForecastsArray.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerForecastsArray.o tests/runnerForecastsArray.cpp


${TESTDIR}/tests/testForecastsArray.o: tests/testForecastsArray.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testForecastsArray.o tests/testForecastsArray.cpp


${TESTDIR}/tests/runnerObservationsArray.o: tests/runnerObservationsArray.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerObservationsArray.o tests/runnerObservationsArray.cpp


${TESTDIR}/tests/testObservationsArray.o: tests/testObservationsArray.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testObservationsArray.o tests/testObservationsArray.cpp


${TESTDIR}/tests/runnerSimilarityMatrices.o: tests/runnerSimilarityMatrices.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerSimilarityMatrices.o tests/runnerSimilarityMatrices.cpp


${TESTDIR}/tests/testSimilarityMatrices.o: tests/testSimilarityMatrices.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testSimilarityMatrices.o tests/testSimilarityMatrices.cpp


${TESTDIR}/tests/runnerStandardDeviation.o: tests/runnerStandardDeviation.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerStandardDeviation.o tests/runnerStandardDeviation.cpp


${TESTDIR}/tests/testStandardDeviation.o: tests/testStandardDeviation.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testStandardDeviation.o tests/testStandardDeviation.cpp


${TESTDIR}/tests/runnerStation.o: tests/runnerStation.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerStation.o tests/runnerStation.cpp


${TESTDIR}/tests/testStation.o: tests/testStation.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testStation.o tests/testStation.cpp


${TESTDIR}/tests/runnerStations.o: tests/runnerStations.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/runnerStations.o tests/runnerStations.cpp


${TESTDIR}/tests/testStations.o: tests/testStations.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/testStations.o tests/testStations.cpp


${OBJECTDIR}/AnEn_nomain.o: ${OBJECTDIR}/AnEn.o AnEn.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/AnEn.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEn_nomain.o AnEn.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/AnEn.o ${OBJECTDIR}/AnEn_nomain.o;\
	fi

${OBJECTDIR}/AnEnIO_nomain.o: ${OBJECTDIR}/AnEnIO.o AnEnIO.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/AnEnIO.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnEnIO_nomain.o AnEnIO.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/AnEnIO.o ${OBJECTDIR}/AnEnIO_nomain.o;\
	fi

${OBJECTDIR}/Array4D_nomain.o: ${OBJECTDIR}/Array4D.o Array4D.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Array4D.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Array4D_nomain.o Array4D.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Array4D.o ${OBJECTDIR}/Array4D_nomain.o;\
	fi

${OBJECTDIR}/Forecasts_nomain.o: ${OBJECTDIR}/Forecasts.o Forecasts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Forecasts.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Forecasts_nomain.o Forecasts.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Forecasts.o ${OBJECTDIR}/Forecasts_nomain.o;\
	fi

${OBJECTDIR}/Observations_nomain.o: ${OBJECTDIR}/Observations.o Observations.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Observations.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Observations_nomain.o Observations.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Observations.o ${OBJECTDIR}/Observations_nomain.o;\
	fi

${OBJECTDIR}/Parameters_nomain.o: ${OBJECTDIR}/Parameters.o Parameters.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Parameters.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Parameters_nomain.o Parameters.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Parameters.o ${OBJECTDIR}/Parameters_nomain.o;\
	fi

${OBJECTDIR}/SimilarityMatrices_nomain.o: ${OBJECTDIR}/SimilarityMatrices.o SimilarityMatrices.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/SimilarityMatrices.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SimilarityMatrices_nomain.o SimilarityMatrices.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/SimilarityMatrices.o ${OBJECTDIR}/SimilarityMatrices_nomain.o;\
	fi

${OBJECTDIR}/StandardDeviation_nomain.o: ${OBJECTDIR}/StandardDeviation.o StandardDeviation.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/StandardDeviation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StandardDeviation_nomain.o StandardDeviation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/StandardDeviation.o ${OBJECTDIR}/StandardDeviation_nomain.o;\
	fi

${OBJECTDIR}/Stations_nomain.o: ${OBJECTDIR}/Stations.o Stations.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Stations.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stations_nomain.o Stations.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Stations.o ${OBJECTDIR}/Stations_nomain.o;\
	fi

${OBJECTDIR}/Times_nomain.o: ${OBJECTDIR}/Times.o Times.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Times.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Times_nomain.o Times.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Times.o ${OBJECTDIR}/Times_nomain.o;\
	fi

${OBJECTDIR}/canalogs_nomain.o: ${OBJECTDIR}/canalogs.o canalogs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/canalogs.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/canalogs_nomain.o canalogs.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/canalogs.o ${OBJECTDIR}/canalogs_nomain.o;\
	fi

${OBJECTDIR}/doxygen-mainpage_nomain.o: ${OBJECTDIR}/doxygen-mainpage.o doxygen-mainpage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/doxygen-mainpage.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/doxygen-mainpage_nomain.o doxygen-mainpage.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/doxygen-mainpage.o ${OBJECTDIR}/doxygen-mainpage_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f8 || true; \
	    ${TESTDIR}/TestFiles/f4 || true; \
	    ${TESTDIR}/TestFiles/f5 || true; \
	    ${TESTDIR}/TestFiles/f3 || true; \
	    ${TESTDIR}/TestFiles/f6 || true; \
	    ${TESTDIR}/TestFiles/f7 || true; \
	    ${TESTDIR}/TestFiles/f1 || true; \
	    ${TESTDIR}/TestFiles/f2 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
