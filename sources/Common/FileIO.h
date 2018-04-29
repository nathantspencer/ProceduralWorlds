#pragma once

#include <string>

/*! A collection of functions for reading and writing files. \file
 */
namespace FileIO
{
    /*! Reads the contents of an ASCII file into a string.
     *
     * @param   path    the path to the ASCII file from which to read
     * @return  a string containing the ASCII contents of the file at the given path
     */
    std::string ReadFileToString(const std::string& path);
};
