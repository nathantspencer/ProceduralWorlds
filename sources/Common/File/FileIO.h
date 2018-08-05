#pragma once

#include <string>

/*! Functions for reading from and writing to files. \file
 */
namespace FileIO
{
    /*! Reads the contents of an ASCII file into a string.
     *
     * @param   path  the string path to the ASCII file from which to read
     * @return  a string containing the ASCII contents of the file at the given path
     */
    std::string ReadFileToString(const std::string& path);
};
