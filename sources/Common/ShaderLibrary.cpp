#include "ShaderLibrary.h"

ShaderLibrary::ShaderLibrary(const std::string& shaderDirectory)
{
}

std::string ShaderLibrary::GetCurrentProgramName() const
{
    return m_currentProgram.first;
}

GLuint ShaderLibrary::GetCurrentProgramHandle() const
{
    return m_currentProgram.second;
}
