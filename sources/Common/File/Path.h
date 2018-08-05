#pragma once

#include <string>

/*! Functions for manipulating string representations of paths. \file
 */
namespace Path
{
    /*! Returns the string file extension of the provided path.
     *
     * If multiple file extensions are present, the first is returned. For example,
     * called on the string `"path/to/file.first.second"`, `".first"` is returned.
     *
     * If no file extension is present or if the path points to a directory, an
     * empty string is returned.
     *
     * @param  path  the string path whose file extension to return
     * @return the string file extension of the provided path
     */
    std::string GetFileExtension(const std::string& path);
}
