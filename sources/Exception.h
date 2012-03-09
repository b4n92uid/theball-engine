#ifndef GL_EXCEPTION_H
#define GL_EXCEPTION_H

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdarg>
#include <exception>

#include "GLee.h"
#include <GL/glu.h>

namespace tbe
{

/// \brief Gestion des exceptions

class Exception : public std::exception, public std::stringstream
{
public:
    Exception();
    Exception(std::string content, ...);
    Exception(const char* content, ...);
    Exception(const Exception& copy);
    ~Exception() throw();

    const char* what() const throw();
};

}

#endif
