#include "ShaderIO.h"

#include <Common/File/FileIO.h>
#include <Common/File/Path.h>

static GLint GetShaderTypeByExtension(const std::string& extension)
{
    if (extension == ".vs")
    {
        return GL_VERTEX_SHADER;
    }
    else if (extension == ".fs")
    {
        return GL_FRAGMENT_SHADER;
    }
    else if (extension == ".cs")
    {
        return GL_COMPUTE_SHADER;
    }
    else if (extension == ".tcs")
    {
        return GL_TESS_CONTROL_SHADER;
    }
    else if (extension == ".tes")
    {
        return GL_TESS_EVALUATION_SHADER;
    }
    else if (extension == ".gs")
    {
        return GL_GEOMETRY_SHADER;
    }
    
    // TODO: print warning to console
    return -1;
}

GLuint ShaderIO::CompileShader(const std::string& path)
{
    std::string shaderExtension = Path::GetFileExtension(path);    
    GLint shaderType = GetShaderTypeByExtension(shaderExtension);
    
    GLuint shaderHandle = glCreateShader(shaderType);
    std::string shaderSource = FileIO::ReadFileToString(path);
    const GLchar* shaderSourceData = shaderSource.data();
    
    glShaderSource(shaderHandle, 1, &shaderSourceData, NULL);
    glCompileShader(shaderHandle);
    
    GLint compiled = 0;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
    GLint infoLength = 0;
    glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLength);
    
    if (infoLength > 1)
    {
        // TODO: print this to console in warning or error mode as appropriate
        printf("%s during shader compilation.\n ", compiled == GL_TRUE ? "Warning" : "Error");
        char* buffer = new char[infoLength];
        glGetShaderInfoLog(shaderHandle, infoLength, NULL, buffer);
        printf("Compilation log: %s\n", buffer);
        delete[] buffer;
    }
    
    return shaderHandle;
}
