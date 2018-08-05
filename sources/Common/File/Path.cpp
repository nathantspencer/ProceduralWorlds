#include "Path.h"

#include <algorithm>
#include <vector>

std::string Path::GetFileExtension(const std::string& path)
{
    std::string buffer;
    std::string extension;
    
    for (auto it = path.rbegin(); it != path.rend(); ++it)
    {
        if (*it == '/' || *it == '\\')
        {
            break;
        }
        
        buffer += *it;
        if (*it == '.')
        {
            extension = buffer;
            buffer.clear();
        }
    }
    
    std::reverse(extension.begin(), extension.end());
    return extension;
}
