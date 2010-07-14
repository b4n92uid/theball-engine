#include "Exception.h"

using namespace tbe;
using namespace std;

Exception::Exception()
{

}

Exception::Exception(const char* content, ...)
{
    char buffer[1024];

    va_list list;
    va_start(list, content);
    vsprintf(buffer, content, list);
    va_end(list);

    str(buffer);
}

Exception::Exception(const Exception& copy)
{
    str(copy.str());
}

Exception::~Exception() throw ()
{

}

const char* Exception::what() const throw ()
{
    string error = str();

    GLenum glErrorNum = glGetError();

    if(glErrorNum)
    {
        error += "\nOpenGL error; ";
        error += (char*)gluErrorString(glErrorNum);
    }

    return error.c_str();
}
