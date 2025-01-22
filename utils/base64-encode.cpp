//
// KIM-API: An API for interatomic models
// Copyright (c) 2013--2022, Regents of the University of Minnesota.
// All rights reserved.
//
// Contributors:
//    Navaneeth Mohan
//    Amit Gupta
//    Ryan S. Elliott
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//

//
// Release: This file is part of the kim-api.git repository.
//


#include "KIM_Version.hpp"
#include "base64-encode/base64.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

void usage(std::string name)
{
  size_t beg = name.find_last_of("/\\");
  if (beg != std::string::npos) name = name.substr(beg + 1, std::string::npos);

  // Follows docopt.org format
  std::cerr << "Usage:\n"
            << "  " << name << " -i <file-in> <file-out>\n"
            << "  " << name << "--version\n";
  // note: this interface is likely to change in future kim-api releases
}


namespace B64ENCODE
{
// quick replacement of to_string for C++98
inline std::string int_to_string(int value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

/// \brief Write the Base^4 encode binary file as a C++ source file, emulating
/// XXD
///
/// \param[in] fileName The name of the file to encode
/// \param[in] outputFileName The name of the file to write the encoded file to
/// \sa base64::encode
void WriteEncodedFile(std::string & fileName, std::string & outputFileName)
{
  unsigned int len = 0;  // total written len of all the content
  unsigned int linepos
      = 0;  // current io positions, always < base64::MAX_BASE64_WIDTH
  unsigned int n_base64_char;  // total base64 char obtained
  char rawBuffer[base64::IO_CHUNK];  // buffer to store raw io data
  char encodedBuffer[4 * ((base64::IO_CHUNK + 2) / 3)];  // buffer for converted
                                                         // data

  // setup fine names and pointers
  std::string encodeFormatFileName = fileName;
  // Make the file names compatible with encode variable name format
  std::replace(
      encodeFormatFileName.begin(), encodeFormatFileName.end(), '.', '_');
  std::replace(
      encodeFormatFileName.begin(), encodeFormatFileName.end(), '-', '_');

  std::ifstream inputFile(fileName.c_str(), std::ios::binary);
  std::ofstream outputFile(outputFileName.c_str());


  // C++ const is different from C const (equivalent to static const),
  // so we need to use extern
  std::string header
      = "extern unsigned char " + encodeFormatFileName + "[] = \n\"";
  outputFile.write(header.data(), header.length());

  // Read IO_CHUNK, convert to base64 and write to file
  while (inputFile.read(rawBuffer, base64::IO_CHUNK) || inputFile.gcount())
  {
    n_base64_char
        = base64::encode(encodedBuffer, rawBuffer, inputFile.gcount());

    for (unsigned int i = 0; i < n_base64_char; i++)
    {
      outputFile.put(encodedBuffer[i]);
      linepos++;
      len++;
      if (linepos >= base64::MAX_BASE64_WIDTH)
      {
        outputFile.write("\"\n\"", 3);
        linepos = 0;
      }
    }
  }

  std::string footer = "\";\nextern unsigned int " + encodeFormatFileName
                       + "_len = " + int_to_string(len) + ";\n";

  outputFile.write(footer.data(),
                   static_cast<std::streamsize>(footer.length()));
  inputFile.close();
  outputFile.close();
}
}  // namespace B64ENCODE

int main(int argc, char * argv[])
{
  if ((argc < 2) || (argc > 4))
  {
    usage(argv[0]);
    return 1;
  }
  std::string option = argv[1];
  if (option == "--version") { std::cout << KIM_VERSION_STRING << std::endl; }
  else if (option == "-i" && argc == 4)
  {
    std::string fileName = argv[2];
    std::string outputFileName = argv[3];
    B64ENCODE::WriteEncodedFile(fileName, outputFileName);
  }
  else
  {
    usage(argv[0]);
    return 1;
  }

  return 0;
}
