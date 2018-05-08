#pragma once

#include <map>
#include <string>

/*! Links and stores OpenGL shader programs. \file
 */
class ProgramLibrary
{
public:
    
    /*! Default constructor for `ProgramLibrary`.
     */
    ShaderLibrary();
    
    /*! Links and stores a shader program with the given name.
     *
     * @param  programName    a name for the program to be added
     * @param  shaderHandles  a list of shaders to attach to the program
     */
    AddProgram(const std::string& programName, std::vector<GLuint>)
    
    /*! Getter for the handle of the shader program currently in use.
     *
     * @return  handle of the shader program currently in use by OpenGL
     */
    GLuint GetActiveProgram() const;
    
    /*! Setter for the shader program currently in use.
     *
     * @return  handle of the shader program to set as active
     */
    void SetActiveProgram(GLuint shaderHandle);
    
private:
    
    std::map<std::string, GLuint> m_programHandles;
}
