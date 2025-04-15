//
// KIM-API: An API for interatomic models
// Copyright (c) 2013--2022, Regents of the University of Minnesota.
// All rights reserved.
//
// Contributors:
//    Ryan S. Elliott
//    Alexander Stukowski
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
// Release: This file is part of the kim-api-2.4.1 package.
//


#include "KIM_SimulatorModel.hpp"
#include "KIM_Version.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

void usage(std::string name)
{
  size_t beg = name.find_last_of("/\\");
  if (beg != std::string::npos) name = name.substr(beg + 1, std::string::npos);

  // Follows docopt.org format
  std::cerr << "Usage:\n"
            << "  " << name << " "
            << "<simulator-model-name>\n"
            << "  " << name << " "
            << "<simulator-model-name> "
            << "number-of-parameter-files\n"
            << "  " << name << " "
            << "<simulator-model-name> "
            << "(smspec-file | <one-based-parameter-file-index>) "
            << "(data | name)\n"
            << "  " << name << " "
            << "--version\n";
  // note: this interface is likely to change in future kim-api releases
}


int main(int argc, char * argv[])
{
  if ((argc < 2) || (argc >= 5))
  {
    usage(argv[0]);
    return -1;
  }

  if ((argc == 2) && (std::string(argv[1]) == "--version"))
  {
    std::cout << KIM_VERSION_STRING << std::endl;
    return 0;
  }

  char const * const simulatorModelName = argv[1];

  KIM::SimulatorModel * simulatorModel;
  int error = KIM::SimulatorModel::Create(simulatorModelName, &simulatorModel);
  if (error)
  {
    std::cout << "* Error: Could not create object for name: '"
              << simulatorModelName << "'" << std::endl;
    return 1;
  }

  int returnValue = 0;

  if (argc == 2)
  {
    std::cout << "SIMULATOR_MODEL";
    returnValue = 0;
    goto cleanup;
  }

  if (std::string(argv[2]) == "number-of-parameter-files")
  {
    int number;
    simulatorModel->GetNumberOfParameterFiles(&number);
    std::cout << number << std::endl;
    returnValue = 0;
    goto cleanup;
  }
  else if (argc < 4)
  {
    usage(argv[0]);
    returnValue = -2;
    goto cleanup;
  }
  else
  {
    std::string const * dirName;
    simulatorModel->GetParameterFileDirectoryName(&dirName);
    std::string const * name;
    if (std::string(argv[2]) == "smspec-file")
    {
      simulatorModel->GetSpecificationFileName(&name);
      std::string const filePath = *dirName + "/" + *name;
      FILE * file = fopen(filePath.c_str(), "rb");
      if (file == NULL)
      {
        std::cout << "* Error: unable to open smspec file." << std::endl;
        returnValue = 8;
        goto cleanup;
      }
      fseek(file, 0, SEEK_END);
      long int size = ftell(file);
      fclose(file);
      file = fopen(filePath.c_str(), "rb");
      unsigned char * fileData = new unsigned char[size];
      size_t numberReadObjects;
      numberReadObjects = fread(fileData, sizeof(unsigned char), size, file);
      fclose(file);
      if (numberReadObjects != static_cast<size_t>(size))
      {
        std::cout << "* Error: unable to fully read smspec file." << std::endl;
        returnValue = 8;
        goto cleanup;
      }

      if (std::string(argv[3]) == "name") { std::cout << *name << std::endl; }
      else if (std::string(argv[3]) == "data")
      {
        fwrite(fileData, sizeof(unsigned char), size, stdout);
      }
      delete[] fileData;
    }
    else  // one-based parameter file index provided
    {
      int number;
      simulatorModel->GetNumberOfParameterFiles(&number);
      int index = atol(argv[2]);
      if ((index < 1) || (index > number))
      {
        std::cout << "* Error: invalid index provided." << std::endl;
        returnValue = 7;
        goto cleanup;
      }
      else
      {
        std::string const * dirName;
        simulatorModel->GetParameterFileDirectoryName(&dirName);
        // The command line interface uses a one-based index for compatibility
        // with external scripts.  The SharedLibrary API uses a zero-based
        // index consistent with standard C++ convention.
        error = simulatorModel->GetParameterFileName(index - 1, &name);
        if (error)
        {
          std::cout << "* Error: unable to get parameter file name."
                    << std::endl;
          returnValue = 8;
          goto cleanup;
        }
        std::string const filePath = *dirName + "/" + *name;
        FILE * file = fopen(filePath.c_str(), "r");
        if (file == NULL)
        {
          std::cout << "* Error: unable to open parameter file." << std::endl;
          returnValue = 8;
          goto cleanup;
        }
        fseek(file, 0, SEEK_END);
        long int size = ftell(file);
        fclose(file);
        file = fopen(filePath.c_str(), "r");
        unsigned char * fileData = new unsigned char[size];
        size_t numberReadObjects;
        numberReadObjects = fread(fileData, sizeof(unsigned char), size, file);
        fclose(file);
        if (numberReadObjects != static_cast<size_t>(size))
        {
          std::cout << "* Error: unable to fully read parameter file."
                    << std::endl;
          returnValue = 8;
          goto cleanup;
        }

        if (std::string(argv[3]) == "name") { std::cout << *name << std::endl; }
        else if (std::string(argv[3]) == "data")
        {
          fwrite(fileData, sizeof(unsigned char), size, stdout);
        }
        delete[] fileData;
      }
    }

    returnValue = 0;
    goto cleanup;
  }

cleanup:
  KIM::SimulatorModel::Destroy(&simulatorModel);
  return returnValue;
}
