#ifndef GL_EXCEPTION_H
#define GL_EXCEPTION_H

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdarg>
#include <exception>

#include <boost/format.hpp>

#include <windows.h>
#include "GLee.h"
#include <GL/glu.h>

namespace tbe
{

/// \brief Gestion des exceptions

class Exception : public std::exception, public boost::format
{
public:
    Exception();
    Exception(std::string content);
    Exception(const Exception& copy);
    ~Exception() throw ();

    template<typename T> Exception& operator%(T v)
    {
        boost::format& self = *this;
        parse(self.operator%(v).str());
        return *this;
    }

    const char* what() const throw ();
};

}

#endif
