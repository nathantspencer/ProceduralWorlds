#pragma once

#include <boost/filesystem.hpp>
#include <string>

namespace fs = boost::filesystem;

/*! Functions for reading from and writing to files. \file
 *
 * Including this file adds an alias for the `boost::filesystem` namespace, shortening
 * it to just `fs`.
 */
namespace FileIO
{
    /*! Reads the contents of an ASCII file into a string.
     *
     * @param   path  the path to the ASCII file from which to read
     * @return  a string containing the ASCII contents of the file at the given path
     */
    std::string ReadFileToString(const fs::path& path);
};
