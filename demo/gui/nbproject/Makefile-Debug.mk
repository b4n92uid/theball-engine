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
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/_ext/1756136190/SDLDevice.o


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
LDLIBSOPTIONS=/D/projets/c++/tbe/dist/Debug/MinGW-Windows/libtbe.a /D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib /D/Librairies/C++/Freetype/freetype-2.3.9/objs/.libs/libfreetype.a /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDLmain.a /D/Librairies/C++/Glee/GLee5_4/GLee.lib -lmingw32 -lopengl32 -lglu32

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../bin/gui.exe

../bin/gui.exe: /D/projets/c++/tbe/dist/Debug/MinGW-Windows/libtbe.a

../bin/gui.exe: /D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib

../bin/gui.exe: /D/Librairies/C++/Freetype/freetype-2.3.9/objs/.libs/libfreetype.a

../bin/gui.exe: /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a

../bin/gui.exe: /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDLmain.a

../bin/gui.exe: /D/Librairies/C++/Glee/GLee5_4/GLee.lib

../bin/gui.exe: ${OBJECTFILES}
	${MKDIR} -p ../bin
	${LINK.cc} -o ../bin/gui ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../../plugins -I../../sources -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/_ext/1756136190/SDLDevice.o: ../../plugins/SDLDevice/SDLDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1756136190
	${RM} $@.d
	$(COMPILE.cc) -g -I../../plugins -I../../sources -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1756136190/SDLDevice.o ../../plugins/SDLDevice/SDLDevice.cpp

# Subprojects
.build-subprojects:
	cd D\:/projets/c++/tbe && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../bin/gui.exe

# Subprojects
.clean-subprojects:
	cd D\:/projets/c++/tbe && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
