#pragma once

#include <Common/FileIO.h>

#include <GL/gl3w.h>
#include <map>
#include <string>

/*! Compiles and stores GLSL shaders. \file
 */
class ShaderLibrary
{
public:
    
    /*! Default constructor for `ShaderLibrary`.
     */
    ShaderLibrary();
    
    /*! Compiles the shader from the given path and adds it to the library.
     *
     * @param  path  the string path to the GLSL shader file to be added
     */
    void AddShader(const fs::path& path);
    
    /*! Returns the handle for the shader with the given name.
     *
     * @param   name  the name of the shader whose handle to return, e.g. "ShaderName.fs"
     * @return  the handle for the selected shader
     */
    GLuint GetShaderHandle(const std::string& name);

private:
    
    std::map<std::string, GLuint> m_shaderHandles;
};
