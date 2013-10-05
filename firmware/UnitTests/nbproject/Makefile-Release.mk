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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/DCC_Decoder.o \
	${OBJECTDIR}/_ext/147789263/reset.o \
	${OBJECTDIR}/FX.o \
	${OBJECTDIR}/DCC_Hardware.o \
	${OBJECTDIR}/AVR/eeprom.o \
	${OBJECTDIR}/DCC_Config_Test.o \
	${OBJECTDIR}/AVR/io.o \
	${OBJECTDIR}/AVR/interrupt.o \
	${OBJECTDIR}/FX_Test.o \
	${OBJECTDIR}/RunAllTests.o \
	${OBJECTDIR}/DCC_Config.o \
	${OBJECTDIR}/DCC_Packet_Test.o \
	${OBJECTDIR}/Sanity_Test.o \
	${OBJECTDIR}/Aegaeon-L.o \
	${OBJECTDIR}/AVR/pgmspace.o \
	${OBJECTDIR}/DCC_Packet.o \
	${OBJECTDIR}/DCC_Decoder_Test.o \
	${OBJECTDIR}/util/atomic.o \
	${OBJECTDIR}/NMRAMotorDecoder_Test.o \
	${OBJECTDIR}/Motor_Test.o \
	${OBJECTDIR}/Motor.o \
	${OBJECTDIR}/DCC_Hardware_Test.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dcc_decoder

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dcc_decoder: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dcc_decoder ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/DCC_Decoder.o: DCC_Decoder.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Decoder.o DCC_Decoder.c

${OBJECTDIR}/_ext/147789263/reset.o: /Users/dgoodman/Documents/NetBeansProjects/DCC_Decoder/reset.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/147789263
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/147789263/reset.o /Users/dgoodman/Documents/NetBeansProjects/DCC_Decoder/reset.c

${OBJECTDIR}/FX.o: FX.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/FX.o FX.c

${OBJECTDIR}/DCC_Hardware.o: DCC_Hardware.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Hardware.o DCC_Hardware.c

${OBJECTDIR}/AVR/eeprom.o: AVR/eeprom.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/eeprom.o AVR/eeprom.c

${OBJECTDIR}/DCC_Config_Test.o: DCC_Config_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Config_Test.o DCC_Config_Test.cpp

${OBJECTDIR}/AVR/io.o: AVR/io.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/io.o AVR/io.c

${OBJECTDIR}/AVR/interrupt.o: AVR/interrupt.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/interrupt.o AVR/interrupt.c

${OBJECTDIR}/FX_Test.o: FX_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/FX_Test.o FX_Test.cpp

${OBJECTDIR}/RunAllTests.o: RunAllTests.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RunAllTests.o RunAllTests.cpp

${OBJECTDIR}/DCC_Config.o: DCC_Config.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Config.o DCC_Config.c

${OBJECTDIR}/DCC_Packet_Test.o: DCC_Packet_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Packet_Test.o DCC_Packet_Test.cpp

${OBJECTDIR}/Sanity_Test.o: Sanity_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Sanity_Test.o Sanity_Test.cpp

${OBJECTDIR}/Aegaeon-L.o: Aegaeon-L.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Aegaeon-L.o Aegaeon-L.c

${OBJECTDIR}/AVR/pgmspace.o: AVR/pgmspace.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/pgmspace.o AVR/pgmspace.c

${OBJECTDIR}/DCC_Packet.o: DCC_Packet.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Packet.o DCC_Packet.c

${OBJECTDIR}/DCC_Decoder_Test.o: DCC_Decoder_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Decoder_Test.o DCC_Decoder_Test.cpp

${OBJECTDIR}/util/atomic.o: util/atomic.c 
	${MKDIR} -p ${OBJECTDIR}/util
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/util/atomic.o util/atomic.c

${OBJECTDIR}/NMRAMotorDecoder_Test.o: NMRAMotorDecoder_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/NMRAMotorDecoder_Test.o NMRAMotorDecoder_Test.cpp

${OBJECTDIR}/Motor_Test.o: Motor_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Motor_Test.o Motor_Test.cpp

${OBJECTDIR}/Motor.o: Motor.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Motor.o Motor.c

${OBJECTDIR}/DCC_Hardware_Test.o: DCC_Hardware_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Hardware_Test.o DCC_Hardware_Test.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dcc_decoder

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
