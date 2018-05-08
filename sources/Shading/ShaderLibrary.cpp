#include <Shading/ShaderLibrary.h>

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

ShaderLibrary::ShaderLibrary()
{
}

void ShaderLibrary::AddShader(const fs::path& path)
{
    std::string shaderName = path.stem().string();
    bool nameAlreadyExists = m_shaderHandles.count(shaderName);
    
    if (nameAlreadyExists)
    {
        // TODO: print warning to console
        return;
    }

    std::string shaderExtension = path.stem().extension().string();
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
        printf("%s during shader compilation.\n ", compiled == GL_TRUE ? "Warning" : "Error");
        char* buffer = new char[infoLength];
        glGetShaderInfoLog(shaderHandle, infoLength, NULL, buffer);
        printf("Compilation log: %s\n", buffer);
        delete[] buffer;
    }
    
    m_shaderHandles[shaderName] = shaderHandle;
}

GLuint ShaderLibrary::GetShaderHandle(const std::string& shaderName)
{
    bool nameExists = m_shaderHandles.count(shaderName);
    if (!nameExists)
    {
        // TODO: print warning to console
        return -1;
    }
    
    return m_shaderHandles[shaderName];
}
