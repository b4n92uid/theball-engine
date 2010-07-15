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
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/sources/MotionPath.o \
	${OBJECTDIR}/sources/Heightmap.o \
	${OBJECTDIR}/sources/ListBox.o \
	${OBJECTDIR}/sources/BloomEffect.o \
	${OBJECTDIR}/sources/MeshParallelScene.o \
	${OBJECTDIR}/sources/GLee.o \
	${OBJECTDIR}/sources/Rtt.o \
	${OBJECTDIR}/sources/Shader.o \
	${OBJECTDIR}/sources/DofEffect.o \
	${OBJECTDIR}/sources/PostProcessManager.o \
	${OBJECTDIR}/sources/ColorEffect.o \
	${OBJECTDIR}/sources/Fog.o \
	${OBJECTDIR}/sources/BlurEffect.o \
	${OBJECTDIR}/sources/GuiManager.o \
	${OBJECTDIR}/sources/ObjMesh.o \
	${OBJECTDIR}/sources/Node.o \
	${OBJECTDIR}/sources/Skin.o \
	${OBJECTDIR}/sources/ParticlesParallelScene.o \
	${OBJECTDIR}/sources/Pencil.o \
	${OBJECTDIR}/sources/Light.o \
	${OBJECTDIR}/sources/StateShow.o \
	${OBJECTDIR}/sources/Primitives.o \
	${OBJECTDIR}/sources/Switch.o \
	${OBJECTDIR}/sources/Gauge.o \
	${OBJECTDIR}/sources/HardwareBuffer.o \
	${OBJECTDIR}/sources/KeyConfig.o \
	${OBJECTDIR}/sources/FrameBufferObject.o \
	${OBJECTDIR}/sources/Skybox.o \
	${OBJECTDIR}/sources/Device.o \
	${OBJECTDIR}/sources/AABB.o \
	${OBJECTDIR}/sources/Water.o \
	${OBJECTDIR}/sources/ParallelScene.o \
	${OBJECTDIR}/sources/Particles.o \
	${OBJECTDIR}/sources/Mesh.o \
	${OBJECTDIR}/sources/TextBox.o \
	${OBJECTDIR}/sources/PosMap.o \
	${OBJECTDIR}/sources/Texture.o \
	${OBJECTDIR}/sources/FPSManager.o \
	${OBJECTDIR}/sources/Clock.o \
	${OBJECTDIR}/sources/Camera.o \
	${OBJECTDIR}/sources/Exception.o \
	${OBJECTDIR}/sources/Editbox.o \
	${OBJECTDIR}/sources/Frustum.o \
	${OBJECTDIR}/sources/VectorBox.o \
	${OBJECTDIR}/sources/Control.o \
	${OBJECTDIR}/sources/Layout.o \
	${OBJECTDIR}/sources/Image.o \
	${OBJECTDIR}/sources/Button.o \
	${OBJECTDIR}/sources/Material.o \
	${OBJECTDIR}/sources/WaterParallelScene.o \
	${OBJECTDIR}/sources/Scenemanager.o \
	${OBJECTDIR}/sources/MotionBlurEffect.o


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
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/MinGW-Windows/libtbe.a

dist/Release/MinGW-Windows/libtbe.a: ${OBJECTFILES}
	${MKDIR} -p dist/Release/MinGW-Windows
	${RM} dist/Release/MinGW-Windows/libtbe.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtbe.a ${OBJECTFILES} 
	$(RANLIB) dist/Release/MinGW-Windows/libtbe.a

${OBJECTDIR}/sources/MotionPath.o: sources/MotionPath.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/MotionPath.o sources/MotionPath.cpp

${OBJECTDIR}/sources/Heightmap.o: sources/Heightmap.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Heightmap.o sources/Heightmap.cpp

${OBJECTDIR}/sources/ListBox.o: sources/ListBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/ListBox.o sources/ListBox.cpp

${OBJECTDIR}/sources/BloomEffect.o: sources/BloomEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/BloomEffect.o sources/BloomEffect.cpp

${OBJECTDIR}/sources/MeshParallelScene.o: sources/MeshParallelScene.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/MeshParallelScene.o sources/MeshParallelScene.cpp

${OBJECTDIR}/sources/GLee.o: sources/GLee.c 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/GLee.o sources/GLee.c

${OBJECTDIR}/sources/Rtt.o: sources/Rtt.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Rtt.o sources/Rtt.cpp

${OBJECTDIR}/sources/Shader.o: sources/Shader.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Shader.o sources/Shader.cpp

${OBJECTDIR}/sources/DofEffect.o: sources/DofEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/DofEffect.o sources/DofEffect.cpp

${OBJECTDIR}/sources/PostProcessManager.o: sources/PostProcessManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/PostProcessManager.o sources/PostProcessManager.cpp

${OBJECTDIR}/sources/ColorEffect.o: sources/ColorEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/ColorEffect.o sources/ColorEffect.cpp

${OBJECTDIR}/sources/Fog.o: sources/Fog.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Fog.o sources/Fog.cpp

${OBJECTDIR}/sources/BlurEffect.o: sources/BlurEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/BlurEffect.o sources/BlurEffect.cpp

${OBJECTDIR}/sources/GuiManager.o: sources/GuiManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/GuiManager.o sources/GuiManager.cpp

${OBJECTDIR}/sources/ObjMesh.o: sources/ObjMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/ObjMesh.o sources/ObjMesh.cpp

${OBJECTDIR}/sources/Node.o: sources/Node.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Node.o sources/Node.cpp

${OBJECTDIR}/sources/Skin.o: sources/Skin.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Skin.o sources/Skin.cpp

${OBJECTDIR}/sources/ParticlesParallelScene.o: sources/ParticlesParallelScene.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/ParticlesParallelScene.o sources/ParticlesParallelScene.cpp

${OBJECTDIR}/sources/Pencil.o: sources/Pencil.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Pencil.o sources/Pencil.cpp

${OBJECTDIR}/sources/Light.o: sources/Light.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Light.o sources/Light.cpp

${OBJECTDIR}/sources/StateShow.o: sources/StateShow.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/StateShow.o sources/StateShow.cpp

${OBJECTDIR}/sources/Primitives.o: sources/Primitives.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Primitives.o sources/Primitives.cpp

${OBJECTDIR}/sources/Switch.o: sources/Switch.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Switch.o sources/Switch.cpp

${OBJECTDIR}/sources/Gauge.o: sources/Gauge.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Gauge.o sources/Gauge.cpp

${OBJECTDIR}/sources/HardwareBuffer.o: sources/HardwareBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/HardwareBuffer.o sources/HardwareBuffer.cpp

${OBJECTDIR}/sources/KeyConfig.o: sources/KeyConfig.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/KeyConfig.o sources/KeyConfig.cpp

${OBJECTDIR}/sources/FrameBufferObject.o: sources/FrameBufferObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/FrameBufferObject.o sources/FrameBufferObject.cpp

${OBJECTDIR}/sources/Skybox.o: sources/Skybox.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Skybox.o sources/Skybox.cpp

${OBJECTDIR}/sources/Device.o: sources/Device.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Device.o sources/Device.cpp

${OBJECTDIR}/sources/AABB.o: sources/AABB.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/AABB.o sources/AABB.cpp

${OBJECTDIR}/sources/Water.o: sources/Water.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Water.o sources/Water.cpp

${OBJECTDIR}/sources/ParallelScene.o: sources/ParallelScene.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/ParallelScene.o sources/ParallelScene.cpp

${OBJECTDIR}/sources/Particles.o: sources/Particles.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Particles.o sources/Particles.cpp

${OBJECTDIR}/sources/Mesh.o: sources/Mesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Mesh.o sources/Mesh.cpp

${OBJECTDIR}/sources/TextBox.o: sources/TextBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/TextBox.o sources/TextBox.cpp

${OBJECTDIR}/sources/PosMap.o: sources/PosMap.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/PosMap.o sources/PosMap.cpp

${OBJECTDIR}/sources/Texture.o: sources/Texture.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Texture.o sources/Texture.cpp

${OBJECTDIR}/sources/FPSManager.o: sources/FPSManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/FPSManager.o sources/FPSManager.cpp

${OBJECTDIR}/sources/Clock.o: sources/Clock.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Clock.o sources/Clock.cpp

${OBJECTDIR}/sources/Camera.o: sources/Camera.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Camera.o sources/Camera.cpp

${OBJECTDIR}/sources/Exception.o: sources/Exception.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Exception.o sources/Exception.cpp

${OBJECTDIR}/sources/Editbox.o: sources/Editbox.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Editbox.o sources/Editbox.cpp

${OBJECTDIR}/sources/Frustum.o: sources/Frustum.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Frustum.o sources/Frustum.cpp

${OBJECTDIR}/sources/VectorBox.o: sources/VectorBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/VectorBox.o sources/VectorBox.cpp

${OBJECTDIR}/sources/Control.o: sources/Control.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Control.o sources/Control.cpp

${OBJECTDIR}/sources/Layout.o: sources/Layout.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Layout.o sources/Layout.cpp

${OBJECTDIR}/sources/Image.o: sources/Image.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Image.o sources/Image.cpp

${OBJECTDIR}/sources/Button.o: sources/Button.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Button.o sources/Button.cpp

${OBJECTDIR}/sources/Material.o: sources/Material.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Material.o sources/Material.cpp

${OBJECTDIR}/sources/WaterParallelScene.o: sources/WaterParallelScene.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/WaterParallelScene.o sources/WaterParallelScene.cpp

${OBJECTDIR}/sources/Scenemanager.o: sources/Scenemanager.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/Scenemanager.o sources/Scenemanager.cpp

${OBJECTDIR}/sources/MotionBlurEffect.o: sources/MotionBlurEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/sources
	${RM} $@.d
	$(COMPILE.cc) -O2 -s -I/D/Librairies/C++/Devil/DevIL-SDK-x86-1.7.8/include -I/D/Librairies/C++/Freetype/freetype-2.3.9/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/sources/MotionBlurEffect.o sources/MotionBlurEffect.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/MinGW-Windows/libtbe.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
