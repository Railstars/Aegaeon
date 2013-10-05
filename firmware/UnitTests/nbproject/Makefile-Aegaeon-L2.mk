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
CND_CONF=Aegaeon-L2
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1472/Aegaeon.o \
	${OBJECTDIR}/_ext/1472/DCC_Config.o \
	${OBJECTDIR}/_ext/1472/DCC_Decoder.o \
	${OBJECTDIR}/_ext/1472/DCC_Hardware.o \
	${OBJECTDIR}/_ext/1472/DCC_Packet.o \
	${OBJECTDIR}/_ext/1472/FX.o \
	${OBJECTDIR}/_ext/1472/Motor.o \
	${OBJECTDIR}/_ext/1472/reset.o \
	${OBJECTDIR}/AVR/eeprom.o \
	${OBJECTDIR}/AVR/interrupt.o \
	${OBJECTDIR}/AVR/io.o \
	${OBJECTDIR}/AVR/pgmspace.o \
	${OBJECTDIR}/DCC_Config_Test.o \
	${OBJECTDIR}/DCC_Decoder_Test.o \
	${OBJECTDIR}/DCC_Hardware_Test.o \
	${OBJECTDIR}/DCC_Packet_Test.o \
	${OBJECTDIR}/FX_Test.o \
	${OBJECTDIR}/Motor_Test.o \
	${OBJECTDIR}/NMRAMotorDecoder_Test.o \
	${OBJECTDIR}/RunAllTests.o \
	${OBJECTDIR}/Sanity_Test.o \
	${OBJECTDIR}/util/atomic.o


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
LDLIBSOPTIONS=-lCppUTest

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1472/Aegaeon.o: ../Aegaeon.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/Aegaeon.o ../Aegaeon.c

${OBJECTDIR}/_ext/1472/DCC_Config.o: ../DCC_Config.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/DCC_Config.o ../DCC_Config.c

${OBJECTDIR}/_ext/1472/DCC_Decoder.o: ../DCC_Decoder.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/DCC_Decoder.o ../DCC_Decoder.c

${OBJECTDIR}/_ext/1472/DCC_Hardware.o: ../DCC_Hardware.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/DCC_Hardware.o ../DCC_Hardware.c

${OBJECTDIR}/_ext/1472/DCC_Packet.o: ../DCC_Packet.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/DCC_Packet.o ../DCC_Packet.c

${OBJECTDIR}/_ext/1472/FX.o: ../FX.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/FX.o ../FX.c

${OBJECTDIR}/_ext/1472/Motor.o: ../Motor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/Motor.o ../Motor.c

${OBJECTDIR}/_ext/1472/reset.o: ../reset.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/reset.o ../reset.c

${OBJECTDIR}/AVR/eeprom.o: AVR/eeprom.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/eeprom.o AVR/eeprom.c

${OBJECTDIR}/AVR/interrupt.o: AVR/interrupt.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/interrupt.o AVR/interrupt.c

${OBJECTDIR}/AVR/io.o: AVR/io.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/io.o AVR/io.c

${OBJECTDIR}/AVR/pgmspace.o: AVR/pgmspace.c 
	${MKDIR} -p ${OBJECTDIR}/AVR
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AVR/pgmspace.o AVR/pgmspace.c

${OBJECTDIR}/DCC_Config_Test.o: DCC_Config_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Config_Test.o DCC_Config_Test.cpp

${OBJECTDIR}/DCC_Decoder_Test.o: DCC_Decoder_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Decoder_Test.o DCC_Decoder_Test.cpp

${OBJECTDIR}/DCC_Hardware_Test.o: DCC_Hardware_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Hardware_Test.o DCC_Hardware_Test.cpp

${OBJECTDIR}/DCC_Packet_Test.o: DCC_Packet_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DCC_Packet_Test.o DCC_Packet_Test.cpp

${OBJECTDIR}/FX_Test.o: FX_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/FX_Test.o FX_Test.cpp

${OBJECTDIR}/Motor_Test.o: Motor_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Motor_Test.o Motor_Test.cpp

${OBJECTDIR}/NMRAMotorDecoder_Test.o: NMRAMotorDecoder_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/NMRAMotorDecoder_Test.o NMRAMotorDecoder_Test.cpp

${OBJECTDIR}/RunAllTests.o: RunAllTests.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RunAllTests.o RunAllTests.cpp

${OBJECTDIR}/Sanity_Test.o: Sanity_Test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/share/CppUTest/include -I.. -I. -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Sanity_Test.o Sanity_Test.cpp

${OBJECTDIR}/util/atomic.o: util/atomic.c 
	${MKDIR} -p ${OBJECTDIR}/util
	${RM} $@.d
	$(COMPILE.c) -g -DTESTING -D__AEGAEON_L2 -I/usr/local/include/CppUTest -I. -I.. -std=c89 -MMD -MP -MF $@.d -o ${OBJECTDIR}/util/atomic.o util/atomic.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
