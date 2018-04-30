#pragma once

#include <GL/gl3w.h>

#include <map>
#include <string>

/*! A container for storing, compiling, and swapping shader programs. \file
 */
class ShaderLibrary
{
public:
    
    /*! Constructor for `ShaderLibrary`.
     *
     * @param  shaderDirectory the directory containing the shaders used by the library
     */
    ShaderLibrary(const std::string& shaderDirectory);
    
    std::string GetCurrentProgramName() const;
    GLuint      GetCurrentProgramHandle() const;

private:
    
    std::pair<std::string, GLuint> m_currentProgram;
    std::map<std::string, GLuint>  m_programs;
};
