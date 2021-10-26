#include <iostream>
#include "Renderer.h"

void GlClearError()
{
    while (glGetError() != GL_NO_ERROR);    
}

bool GlLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGl_Error : ("<< error << ") :"<< function << " " 
        << file << " : " << line << std::endl;
        return false;
    }  
    return true;  
}