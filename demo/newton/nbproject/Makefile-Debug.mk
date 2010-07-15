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
	${OBJECTDIR}/_ext/539585854/NewtonParallelScene.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/_ext/1756136190/SDLDevice.o \
	${OBJECTDIR}/_ext/539585854/NewtonNode.o


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
LDLIBSOPTIONS=../../dist/Debug/MinGW-Windows/libtbe.a /D/librairies/c++/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib /D/librairies/c++/freetype/freetype-2.3.9/objs/.libs/libfreetype.a /D/librairies/c++/glee/GLee5_4/GLee.lib /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDLmain.a /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a /D/librairies/c++/newton/NewtonWin-2.22/sdk/x32/dll_vs7/newton.lib -lmingw32 -lopengl32 -lglu32

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../bin/newton.exe

../bin/newton.exe: ../../dist/Debug/MinGW-Windows/libtbe.a

../bin/newton.exe: /D/librairies/c++/devil/DevIL-SDK-x86-1.7.8/lib/DevIL.lib

../bin/newton.exe: /D/librairies/c++/freetype/freetype-2.3.9/objs/.libs/libfreetype.a

../bin/newton.exe: /D/librairies/c++/glee/GLee5_4/GLee.lib

../bin/newton.exe: /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDLmain.a

../bin/newton.exe: /D/librairies/c++/sdl/SDL/SDL-1.2.14/lib/libSDL.dll.a

../bin/newton.exe: /D/librairies/c++/newton/NewtonWin-2.22/sdk/x32/dll_vs7/newton.lib

../bin/newton.exe: ${OBJECTFILES}
	${MKDIR} -p ../bin
	${LINK.cc} -o ../bin/newton ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/539585854/NewtonParallelScene.o: ../../plugins/Newton/NewtonParallelScene.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/539585854
	${RM} $@.d
	$(COMPILE.cc) -g -I../../sources -I../../plugins -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/librairies/c++/newton/NewtonWin-2.22/sdk -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/539585854/NewtonParallelScene.o ../../plugins/Newton/NewtonParallelScene.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../../sources -I../../plugins -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/librairies/c++/newton/NewtonWin-2.22/sdk -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/_ext/1756136190/SDLDevice.o: ../../plugins/SDLDevice/SDLDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1756136190
	${RM} $@.d
	$(COMPILE.cc) -g -I../../sources -I../../plugins -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/librairies/c++/newton/NewtonWin-2.22/sdk -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1756136190/SDLDevice.o ../../plugins/SDLDevice/SDLDevice.cpp

${OBJECTDIR}/_ext/539585854/NewtonNode.o: ../../plugins/Newton/NewtonNode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/539585854
	${RM} $@.d
	$(COMPILE.cc) -g -I../../sources -I../../plugins -I/D/librairies/c++/sdl/SDL/SDL-1.2.14/include/SDL -I/D/librairies/c++/newton/NewtonWin-2.22/sdk -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/539585854/NewtonNode.o ../../plugins/Newton/NewtonNode.cpp

# Subprojects
.build-subprojects:
	cd ../.. && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../bin/newton.exe

# Subprojects
.clean-subprojects:
	cd ../.. && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
