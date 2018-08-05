#pragma once

#include <GL/gl3w.h>
#include <string>

/*! Functions for compiling GLSL shaders. \file
 */
namespace ShaderIO
{
    /*! Compiles the shader from the given path and adds it to the library.
     *
     * @param   path  the string path to the GLSL shader file to be compiled
     * @return  the handle for the compiled shader
     */
    GLuint CompileShader(const std::string& path);
};
