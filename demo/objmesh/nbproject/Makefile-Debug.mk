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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/plugins/SDLDevice/SDLDevice.o \
	${OBJECTDIR}/main.o

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
LDLIBSOPTIONS=../../dist/Debug/MinGW-Windows/libtbe.a D:/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDLmain.a D:/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a D:/librairies/c++/glee/GLee5_4/GLee.lib D:/librairies/c++/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib D:/librairies/c++/freetype/freetype-2.3.9/objs/.libs/libfreetype.a -lopengl32 -lglu32

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk ../bin/objmesh.exe

../bin/objmesh.exe: ../../dist/Debug/MinGW-Windows/libtbe.a

../bin/objmesh.exe: /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDLmain.a

../bin/objmesh.exe: /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a

../bin/objmesh.exe: /D/librairies/c++/glee/GLee5_4/GLee.lib

../bin/objmesh.exe: /D/librairies/c++/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib

../bin/objmesh.exe: /D/librairies/c++/freetype/freetype-2.3.9/objs/.libs/libfreetype.a

../bin/objmesh.exe: ${OBJECTFILES}
	${MKDIR} -p ../bin
	${LINK.cc} -o ../bin/objmesh ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/plugins/SDLDevice/SDLDevice.o: nbproject/Makefile-${CND_CONF}.mk ../../plugins/SDLDevice/SDLDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/plugins/SDLDevice
	${RM} $@.d
	$(COMPILE.cc) -g -I../../sources -I../../plugins -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/plugins/SDLDevice/SDLDevice.o ../../plugins/SDLDevice/SDLDevice.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../../sources -I../../plugins -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../.. && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../bin/objmesh.exe

# Subprojects
.clean-subprojects:
	cd ../.. && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
