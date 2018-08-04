#pragma once

#include <GL/gl3w.h>
#include <map>
#include <string>
#include <vector>

/*! Links and stores OpenGL shader programs. \file
 */
class ProgramLibrary
{
public:
    
    /*! Default constructor for `ProgramLibrary`.
     */
    ProgramLibrary();
    
    /*! Links and stores a shader program with the given name.
     *
     * Note that the shaders passed into this function will be deleted following the
     * linking of the program.
     *
     * @param  programName    a name for the program to be added
     * @param  shaderHandles  a list of shaders to attach to the program
     */
    void AddProgram(const std::string& programName, std::vector<GLuint> shaderHandles);
    
    /*! Returns a handle to the shader program with the given name.
     *
     * @param   programName  the name of the program whose handle to return
     * @return  the handle to the shader program with the given name
     */
    GLuint GetProgramByName(const std::string& programName) const;
    
    /*! Getter for the handle of the shader program currently in use.
     *
     * @return  handle of the shader program currently in use by OpenGL
     */
    GLuint GetActiveProgram() const;
    
    /*! Setter for the shader program currently in use.
     *
     * @param  name of the shader program to set as active
     */
    void SetActiveProgram(const std::string& programName);
    
    /*! Setter for the shader program currently in use.
     *
     * @param  handle of the shader program to set as active
     */
    void SetActiveProgram(GLuint programHandle);
    
private:
    
    std::map<std::string, GLuint> m_programHandles;
    
    GLuint m_activeProgram;
};
