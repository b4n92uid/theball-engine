# TheBall Engine

TheBall Engine is a rendering engine in C++/OpenGL created for educational purpose

## Author

- [BELDJOUHRI Abdelghani](contact@beldjouhri-abdelghani.com)

## Generating code documentation

```bash
cd doxygen
doxygen Doxyfile
```

## Compile

Download required libraries :

- [Boost](https://www.boost.org/) (v1.61.0)

Setup CMake

```bash
mkdir build
cd build

cmake -G "Visual Studio 15 2017" ..
```

Configure required libraries path

```
//TBENGINE BOOST INCLUDE
TBENGINE_BOOST_INCLUDE:PATH=D:\librairies\cpp\boost\boost_1_61_0
```

Build !

```bash
MSBuild tbengine.vcxproj
```
