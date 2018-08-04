#include <Shading/ProgramLibrary.h>

ProgramLibrary::ProgramLibrary() :
    m_activeProgram(-1)
{
}

void ProgramLibrary::AddProgram(const std::string& programName, std::vector<GLuint> shaderHandles)
{
    GLuint programHandle = glCreateProgram();
    for (GLuint shaderHandle : shaderHandles)
    {
        glAttachShader(programHandle, shaderHandle);
    }
    glLinkProgram(programHandle);
    
    int linked;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLength = 0;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLength);
        if (infoLength > 1)
        {
            char* buffer = new char[infoLength];
            glGetProgramInfoLog(programHandle, infoLength, NULL, buffer);
            printf("Linking error: \n%s\n", buffer);    // TODO: print this to console in error mode
            delete[] buffer;
        }
    }
    
    for (GLuint shaderHandle : shaderHandles)
    {
        glDetachShader(programHandle, shaderHandle);
        glDeleteShader(shaderHandle);
    }
    
    m_programHandles[programName] = programHandle;
}

GLuint ProgramLibrary::GetProgramByName(const std::string& programName) const
{
    auto it = m_programHandles.find(programName);
    if (it == m_programHandles.end())
    {
        // TODO: print warning to console
        return -1;
    }
    return it->second;
}

GLuint ProgramLibrary::GetActiveProgram() const
{
    if (m_activeProgram == -1)
    {
        // TODO: print warning to console
    }
    return m_activeProgram;
}

void ProgramLibrary::SetActiveProgram(const std::string& programName)
{
    auto it = m_programHandles.find(programName);
    if (it == m_programHandles.end())
    {
        // TODO: print warning to console
        return;
    }
    glUseProgram(it->second);
    m_activeProgram = it->second;
}

void ProgramLibrary::SetActiveProgram(GLuint programHandle)
{
    // TODO: return if handle doesn't exist in m_programHandles, throw warning
    glUseProgram(programHandle);
    m_activeProgram = programHandle;
}
