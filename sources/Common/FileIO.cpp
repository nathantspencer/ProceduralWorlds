#include "FileIO.h"

#include <fstream>

std::string FileIO::ReadFileToString(const std::string& path)
{
    std::ifstream fileStream(path);
    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    
    std::string buffer(size, ' ');
    fileStream.seekg(0);
    fileStream.read(&buffer[0], size);
    
    return buffer;
}
