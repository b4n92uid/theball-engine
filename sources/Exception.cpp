#include "Exception.h"

using namespace tbe;
using namespace std;

Exception::Exception() { }

Exception::Exception(std::string content) : boost::format(content) { }

Exception::Exception(const Exception& copy)
{
    boost::format::parse(copy.str());
}

Exception::~Exception() throw () { }

const char* Exception::what() const throw ()
{
    string error = str();

    GLenum glErrorNum = glGetError();

    if(glErrorNum)
    {
        error += "\nOpenGL error; ";
        error += (char*) gluErrorString(glErrorNum);
    }

    cout << error.c_str() << endl;

    return error.c_str();
}
