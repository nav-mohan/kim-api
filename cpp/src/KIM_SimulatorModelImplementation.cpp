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


#include "edn-cpp/edn.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <sstream>


#ifndef KIM_SIMULATOR_MODEL_IMPLEMENTATION_HPP_
#include "KIM_SimulatorModelImplementation.hpp"
#endif

#ifndef KIM_LOG_VERBOSITY_HPP_
#include "KIM_LogVerbosity.hpp"
#endif

#ifndef KIM_LOG_HPP_
#include "KIM_Log.hpp"
#endif

#ifndef KIM_COLLECTION_ITEM_TYPE_HPP_
#include "KIM_CollectionItemType.hpp"
#endif

#ifndef KIM_COLLECTIONS_HPP_
#include "KIM_Collections.hpp"
#endif

#ifndef KIM_SHARED_LIBRARY_HPP_
#include "KIM_SharedLibrary.hpp"
#endif


// log helpers
#define SNUM(x)                                                \
  static_cast<std::ostringstream const &>(std::ostringstream() \
                                          << std::dec << x)    \
      .str()
#define SPTR(x)                                                            \
  static_cast<std::ostringstream const &>(std::ostringstream()             \
                                          << static_cast<void const *>(x)) \
      .str()
#define SFUNC(x)                                                    \
  static_cast<std::ostringstream const &>(                          \
      std::ostringstream() << reinterpret_cast<KIM::Function *>(x)) \
      .str()


#include "KIM_LogMacros.hpp"
#define KIM_LOGGER_OBJECT_NAME this
namespace KIM
{
int SimulatorModelImplementation::Create(
    std::string const & simulatorModelName,
    SimulatorModelImplementation ** const simulatorModelImplementation)
{
  // error checking of arguments performed as part of SimulatorModelCreate()

  Log * pLog;
  int error = Log::Create(&pLog);
  if (error)
  {
    *simulatorModelImplementation = NULL;
    return true;
  }

  SimulatorModelImplementation * pSimulatorModelImplementation;
  pSimulatorModelImplementation
      = new SimulatorModelImplementation(new SharedLibrary(pLog), pLog);
#if DEBUG_VERBOSITY
  std::string const callString = "Create('" + simulatorModelName + "', "
                                 + SPTR(simulatorModelImplementation) + ").";
  pSimulatorModelImplementation->LogEntry(
      LOG_VERBOSITY::debug,
      "Created Log and SimulatorModelImplementation objects after enter "
          + callString,
      __LINE__,
      __FILE__);
#endif

  error = pSimulatorModelImplementation->Initialize(simulatorModelName);
  if (error)
  {
#if DEBUG_VERBOSITY
    pSimulatorModelImplementation->LogEntry(
        LOG_VERBOSITY::debug,
        "Destroying SimulatorModelImplementation object and exit " + callString,
        __LINE__,
        __FILE__);
#endif
    delete pSimulatorModelImplementation;  // also deletes Log object
    *simulatorModelImplementation = NULL;
    return true;
  }

  *simulatorModelImplementation = pSimulatorModelImplementation;
#if DEBUG_VERBOSITY
  (*simulatorModelImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug, "Exit 0=" + callString, __LINE__, __FILE__);
#endif
  return false;
}

void SimulatorModelImplementation::Destroy(
    SimulatorModelImplementation ** const simulatorModelImplementation)
{
#if DEBUG_VERBOSITY
  std::string callString
      = "Destroy(" + SPTR(simulatorModelImplementation) + ").";
  (*simulatorModelImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug, "Enter  " + callString, __LINE__, __FILE__);
#endif

#if DEBUG_VERBOSITY
  (*simulatorModelImplementation)
      ->LogEntry(LOG_VERBOSITY::debug,
                 "Destroying SimulatorModelImplementation object and exit "
                     + callString,
                 __LINE__,
                 __FILE__);
#endif
  delete *simulatorModelImplementation;  // also deletes Log object
  *simulatorModelImplementation = NULL;
}

void SimulatorModelImplementation::GetSimulatorNameAndVersion(
    std::string const ** const simulatorName,
    std::string const ** const simulatorVersion) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetSimulatorNameAndVersion("
                                 + SPTR(simulatorName) + ", "
                                 + SPTR(simulatorVersion) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if (simulatorName != NULL) *simulatorName = &simulatorName_;
  if (simulatorVersion != NULL) *simulatorVersion = &simulatorVersion_;

  LOG_DEBUG("Exit 0=" + callString);
}

void SimulatorModelImplementation::GetNumberOfSupportedSpecies(
    int * const numberOfSupportedSpecies) const
{
  *numberOfSupportedSpecies = simulatorSupportedSpecies_.size();
}

int SimulatorModelImplementation::GetSupportedSpecies(
    int const index, std::string const ** const speciesName) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetSupportedSpecies(" + SNUM(index) + ", " + SPTR(speciesName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if ((index < 0)
      || (static_cast<size_t>(index) >= simulatorSupportedSpecies_.size()))
  {
    LOG_ERROR("Invalid index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  *speciesName = &(simulatorSupportedSpecies_[index]);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void SimulatorModelImplementation::OpenAndInitializeTemplateMap()
{
#if DEBUG_VERBOSITY
  std::string const callString = "OpenAndInitializeTemplateMap().";
#endif
  LOG_DEBUG("Enter  " + callString);

  templateMapOpen_ = true;

  // clear data
  templateMap_.clear();
  simulatorFields_.clear();
  AddStandardTemplatesToMap();

  LOG_DEBUG("Exit 0=" + callString);
}

int SimulatorModelImplementation::TemplateMapIsOpen() const
{
#if DEBUG_VERBOSITY
  std::string const callString = "TemplateMapIsOpen().";
#endif
  LOG_DEBUG("Enter  " + callString);

  int result = templateMapOpen_;

  LOG_DEBUG("Exit " + SNUM(result) + "=" + callString);
  return result;
}

int SimulatorModelImplementation::AddTemplateMap(std::string const & key,
                                                 std::string const & value)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "AddTemplateMap('" + key + "', '" + value + "').";
#endif
  LOG_DEBUG("Enter  " + callString);

  if (!templateMapOpen_)
  {
    LOG_ERROR("Template map is closed.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  char allowedCharacters[] = "abcdefghijklmnopqrstuvwxyz"
                             "-0123456789";
  if (key.find_first_not_of(allowedCharacters) == std::string::npos)
  {
    templateMap_[key] = value;
  }
  else
  {
    LOG_ERROR("Invalid template key, '" + key + "'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void SimulatorModelImplementation::CloseTemplateMap()
{
#if DEBUG_VERBOSITY
  std::string const callString = "CloseTemplateMap().";
#endif
  LOG_DEBUG("Enter  " + callString);

  templateMapOpen_ = false;
  ProcessSimulatorFields();

  LOG_DEBUG("Exit 0=" + callString);
}

void SimulatorModelImplementation::AddStandardTemplatesToMap()
{
#if DEBUG_VERBOSITY
  std::string const callString = "AddStandardTemplatesToMap().";
#endif
  LOG_DEBUG("Enter  " + callString);

  AddTemplateMap("parameter-file-dir", parameterFileDirectoryName_.string());

  for (size_t i = 0; i < parameterFileBasenames_.size(); ++i)
  {
    AddTemplateMap("parameter-file-basename-" + SNUM(i + 1),
                   parameterFileBasenames_[i]);

    AddTemplateMap(
        "parameter-file-" + SNUM(i + 1),
        (parameterFileDirectoryName_ / parameterFileBasenames_[i]).string());
  }

  LOG_DEBUG("Exit 0=" + callString);
}

int SimulatorModelImplementation::ProcessSimulatorFields()
{
#if DEBUG_VERBOSITY
  std::string const callString = "ProcessSimulatorFields().";
#endif
  LOG_DEBUG("Enter  " + callString);

  if (templateMapOpen_)
  {
    LOG_ERROR("Template map is open.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  simulatorFields_.clear();
  for (size_t i = 0; i < originalSimulatorFields_.size(); ++i)
  {
    std::vector<std::string> lines;
    for (size_t j = 0; j < originalSimulatorFields_[i].size(); ++j)
    {
      std::string line(originalSimulatorFields_[i][j]);
      for (std::map<std::string, std::string>::const_iterator itr
           = templateMap_.begin();
           itr != templateMap_.end();
           ++itr)
      {
        std::string const key = "@<" + itr->first + ">@";
        size_t pos = 0;
        while ((pos = line.find(key, pos)) != std::string::npos)
        {
          line.replace(pos, key.length(), itr->second);
          pos += (itr->second).length();
        }
      }
      lines.push_back(line);
    }
    simulatorFields_.push_back(lines);
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void SimulatorModelImplementation::GetNumberOfSimulatorFields(
    int * const numberOfSimulatorFields) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetNumberOfSimulatorFields(" + SPTR(numberOfSimulatorFields) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *numberOfSimulatorFields = originalSimulatorFields_.size();

  LOG_DEBUG("Exit 0=" + callString);
}

int SimulatorModelImplementation::GetSimulatorFieldMetadata(
    int const fieldIndex,
    int * const extent,
    std::string const ** const fieldName) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetSimulatorFieldMetadata(" + SNUM(fieldIndex)
                                 + ", " + SPTR(extent) + ", " + SPTR(fieldName)
                                 + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if ((fieldIndex < 0)
      || (static_cast<size_t>(fieldIndex) >= originalSimulatorFields_.size()))
  {
    LOG_ERROR("Invalid simulator field index, " + SNUM(fieldIndex) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  if (extent != NULL) *extent = originalSimulatorFields_[fieldIndex].size();
  if (fieldName != NULL) *fieldName = &simulatorFieldNames_[fieldIndex];

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int SimulatorModelImplementation::GetSimulatorFieldLine(
    int const fieldIndex,
    int const lineIndex,
    std::string const ** const lineValue) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetSimulatorFieldLine(" + SNUM(fieldIndex)
                                 + ", " + SNUM(lineIndex) + ", "
                                 + SPTR(lineValue) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if (templateMapOpen_)
  {
    LOG_ERROR("Simulator field lines are not available while the template map "
              "is open.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  if ((fieldIndex < 0)
      || (static_cast<size_t>(fieldIndex) >= simulatorFields_.size()))
  {
    LOG_ERROR("Invalid simulator field index, " + SNUM(fieldIndex) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  if ((lineIndex < 0)
      || (static_cast<size_t>(lineIndex)
          >= simulatorFields_[fieldIndex].size()))
  {
    LOG_ERROR("Invalid simulator field line index, " + SNUM(lineIndex) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  *lineValue = &(simulatorFields_[fieldIndex][lineIndex]);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void SimulatorModelImplementation::GetParameterFileDirectoryName(
    std::string const ** const directoryName) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetParameterFileDirectoryName(" + SPTR(directoryName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *directoryName = &parameterFileDirectoryNameString_;

  LOG_DEBUG("Exit 0=" + callString);
}

void SimulatorModelImplementation::GetSpecificationFileName(
    std::string const ** const specificationFileName) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetSpecificationFileName(" + SPTR(specificationFileName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *specificationFileName = &specificationFileName_;

  LOG_DEBUG("Exit 0=" + callString);
}

void SimulatorModelImplementation::GetNumberOfParameterFiles(
    int * const numberOfParameterFiles) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetNumberOfParameterFiles(" + SPTR(numberOfParameterFiles) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *numberOfParameterFiles = numberOfParameterFiles_;

  LOG_DEBUG("Exit 0=" + callString);
}

int SimulatorModelImplementation::GetParameterFileName(
    int const index, std::string const ** const parameterFileName) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetParameterFileName(" + SNUM(index) + ", "
                                 + SPTR(parameterFileName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);
  LOG_WARNING("Use of the " + callString
              + " function is deprecated. "
                "Please use GetParameterFileBasename() instead.");

#if ERROR_VERBOSITY
  if ((index < 0) || (index >= numberOfParameterFiles_))
  {
    LOG_ERROR("Invalid parameter file index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  *parameterFileName = &parameterFileBasenames_[index];

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int SimulatorModelImplementation::GetParameterFileBasename(
    int const index, std::string const ** const parameterFileBasename) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetParameterFileBasename(" + SNUM(index)
                                 + ", " + SPTR(parameterFileBasename) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if ((index < 0) || (index >= numberOfParameterFiles_))
  {
    LOG_ERROR("Invalid parameter file index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  *parameterFileBasename = &parameterFileBasenames_[index];

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void SimulatorModelImplementation::SetSimulatorBufferPointer(void * const ptr)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "SetSimulatorBufferPointer(" + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  simulatorBuffer_ = ptr;

  LOG_DEBUG("Exit   " + callString);
}

void SimulatorModelImplementation::GetSimulatorBufferPointer(
    void ** const ptr) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetSimulatorBufferPointer(" + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *ptr = simulatorBuffer_;

  LOG_DEBUG("Exit   " + callString);
}

void SimulatorModelImplementation::SetLogID(std::string const & logID)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetLogID('" + logID + "').";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->SetID(logID);

  LOG_DEBUG("Exit   " + callString);
}

void SimulatorModelImplementation::PushLogVerbosity(
    LogVerbosity const logVerbosity)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "PushLogVerbosity(" + logVerbosity.ToString() + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->PushVerbosity(logVerbosity);

  LOG_DEBUG("Exit   " + callString);
}

void SimulatorModelImplementation::PopLogVerbosity()
{
#if DEBUG_VERBOSITY
  std::string const callString = "PopLogVerbosity().";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->PopVerbosity();

  LOG_DEBUG("Exit   " + callString);
}

void SimulatorModelImplementation::LogEntry(LogVerbosity const logVerbosity,
                                            std::string const & message,
                                            int const lineNumber,
                                            std::string const & fileName) const
{
  // No debug logs to avoid infinite loop
  log_->LogEntry(logVerbosity, message, lineNumber, fileName);
}

void SimulatorModelImplementation::LogEntry(LogVerbosity const logVerbosity,
                                            std::stringstream const & message,
                                            int const lineNumber,
                                            std::string const & fileName) const
{
  // No debug logs to avoid infinite loop
  log_->LogEntry(logVerbosity, message, lineNumber, fileName);
}

std::string const & SimulatorModelImplementation::ToString() const
{
#if DEBUG_VERBOSITY
  std::string const callString = "ToString().";
#endif
  LOG_DEBUG("Enter  " + callString);

  std::stringstream ss;
  ss << std::setprecision(10) << std::scientific << std::left;
  ss << "===================================================================="
        "============\n\n";

  ss << "SimulatorModel object\n"
     << "------------\n\n";
  ss << "SimulatorModel Name : " << simulatorModelName_ << "\n";
  ss << "Log ID : " << log_->GetID() << "\n";
  ss << "\n";

  ss << "Parameter file directory : " << parameterFileDirectoryName_.string()
     << "\n";

  ss << "Specification file name : " << specificationFileName_ << "\n\n";

  ss << "Simulator Name    : " << simulatorName_ << "\n"
     << "Simulator Version : " << simulatorVersion_ << "\n"
     << "Number of supported species : " << simulatorSupportedSpecies_.size()
     << "\n";
  for (size_t i = 0; i < simulatorSupportedSpecies_.size(); ++i)
  {
    ss << "\tindex : " << i << "\t'" << simulatorSupportedSpecies_[i] << "'\n";
  }
  ss << "\n";

  ss << "Number of parameter files : " << numberOfParameterFiles_ << "\n"
     << "Parameter files :\n";
  for (int i = 0; i < numberOfParameterFiles_; ++i)
  {
    ss << "\t"
       << "index : " << i << "\n"
       << "\t  "
       << "name : " << parameterFileBasenames_[i] << "\n\n";
  }

  ss << "Original simulator fields :\n";
  for (size_t i = 0; i < originalSimulatorFields_.size(); ++i)
  {
    ss << "\t" << simulatorFieldNames_[i] << " :\n";
    for (size_t j = 0; j < originalSimulatorFields_[i].size(); ++j)
    {
      ss << "\t  * '" << originalSimulatorFields_[i][j] << "'\n";
    }
  }
  ss << "\n";

  ss << "Template Map is : " << ((templateMapOpen_) ? "open" : "closed")
     << "\n";
  ss << "Template Map contents :\n";
  for (std::map<std::string, std::string>::const_iterator itr
       = templateMap_.begin();
       itr != templateMap_.end();
       ++itr)
  {
    ss << "\t" << std::setw(20) << itr->first << " -> " << std::setw(20)
       << itr->second << "\n";
  }
  ss << "\n";

  ss << "Simulator fields :\n";
  for (size_t i = 0; i < simulatorFields_.size(); ++i)
  {
    ss << "\t" << simulatorFieldNames_[i] << " :\n";
    for (size_t j = 0; j < simulatorFields_[i].size(); ++j)
    {
      ss << "\t  * '" << simulatorFields_[i][j] << "'\n";
    }
  }
  ss << "\n";


  ss << "Simulator buffer: " << SPTR(simulatorBuffer_) << "\n\n";

  ss << "===================================================================="
        "============\n";

  string_ = ss.str();
  LOG_DEBUG("Exit   " + callString);
  return string_;
}


SimulatorModelImplementation::SimulatorModelImplementation(
    SharedLibrary * const sharedLibrary, Log * const log) :
    simulatorModelName_(""),
    sharedLibrary_(sharedLibrary),
    log_(log),
    schemaVersion_(0),
    modelName_(""),
    simulatorName_(""),
    simulatorVersion_(""),
    numberOfParameterFiles_(0),
    templateMapOpen_(true),
    simulatorBuffer_(NULL),
    string_("")
{
#if DEBUG_VERBOSITY
  std::string const callString = "SimulatorModelImplementation("
                                 + SPTR(sharedLibrary) + ", " + SPTR(log)
                                 + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  LOG_DEBUG("Exit   " + callString);
}

SimulatorModelImplementation::~SimulatorModelImplementation()
{
#if DEBUG_VERBOSITY
  std::string const callString = "~SimulatorModelImplementation().";
#endif
  LOG_DEBUG("Enter  " + callString);

  sharedLibrary_->RemoveParameterFileDirectory();
  if (sharedLibrary_->IsOpen()) sharedLibrary_->Close();
  delete sharedLibrary_;

  LOG_DEBUG("Destroying Log object and exit " + callString);
  Log::Destroy(&log_);
}

int SimulatorModelImplementation::Initialize(
    std::string const & simulatorModelName)
{
#if DEBUG_VERBOSITY
  std::string const callString = "Initialize(" + simulatorModelName + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  simulatorModelName_ = simulatorModelName;

  std::string const * itemFilePath;
  Collections * collections = NULL;
  int error = Collections::Create(&collections);
  if (error)
  {
    LOG_ERROR("Could not create Collections object.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  collections->SetLogID(log_->GetID() + "_Collections");
  error = collections->GetItemLibraryFileNameAndCollection(
      COLLECTION_ITEM_TYPE::simulatorModel,
      simulatorModelName,
      &itemFilePath,
      NULL);
  if (error)
  {
    LOG_ERROR("Could not find simulator model shared library.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  error = sharedLibrary_->Open(*itemFilePath);
  if (error)
  {
    LOG_ERROR("Could not open simulator model shared library.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  Collections::Destroy(&collections);

  CollectionItemType itemType;
  error = sharedLibrary_->GetType(&itemType);
  if (error)
  {
    LOG_ERROR("Unable to get shared library type.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  {
    using namespace COLLECTION_ITEM_TYPE;

    if (itemType == simulatorModel)
    {
      LOG_DEBUG("Initializing a simulator model.");
    }
    else if (itemType == portableModel)
    {
      LOG_ERROR("Creation of a portable model is not allowed.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
    else if (itemType == modelDriver)
    {
      LOG_ERROR("Creation of a model driver is not allowed.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
    else
    {
      LOG_ERROR("Creation of an unknown item type is not allowed.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
  }

  // create parameter files on disk and set associated variables
  error
      = sharedLibrary_->WriteParameterFileDirectory()
        || sharedLibrary_->GetSimulatorModelSpecificationFile(
            &specificationFileName_, NULL, NULL)
        || sharedLibrary_->GetParameterFileDirectoryName(
            &parameterFileDirectoryName_)
        || sharedLibrary_->GetNumberOfParameterFiles(&numberOfParameterFiles_);
  for (int i = 0; i < numberOfParameterFiles_; ++i)
  {
    std::string parameterFileBasename;
    error = error
            || sharedLibrary_->GetParameterFile(
                i, &parameterFileBasename, NULL, NULL);
    if (!error) { parameterFileBasenames_.push_back(parameterFileBasename); }
  }
  if (error)
  {
    sharedLibrary_->RemoveParameterFileDirectory();
    LOG_ERROR("Could not write parameter file directory to scratch space.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  // Convert path to string representation.
  parameterFileDirectoryNameString_ = parameterFileDirectoryName_.string();

  if (GetSchemaVersion())
  {
    // GetSchemaVersion() logs error
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  if (schemaVersion_ == 1)
  {
    if (ReadEdnSchemaV1())
    {
      // ReadEdnSchemaV1() logs error
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
  }
  else
  {
    LOG_ERROR("Shouldn't ever get here.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  // check for all required data
  if (modelName_ == "")
  {
    LOG_ERROR("Required specification field 'model-name' not found.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  else if (modelName_ != simulatorModelName_)
  {
    LOG_ERROR(
        "Specificaiton field 'model-name' not equal to simulator model name.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  if (simulatorName_ == "")
  {
    LOG_ERROR("Required specification field 'simulator-name' not found.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  if (simulatorVersion_ == "")
  {
    LOG_ERROR("Required specification field 'simulator-version' not found.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  if (simulatorSupportedSpecies_.size() == 0)
  {
    LOG_ERROR("Required specification field 'supported-species' not found.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  OpenAndInitializeTemplateMap();

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int SimulatorModelImplementation::ParseEdn(edn::EdnNode & node) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "ParseEdn().";
#endif
  LOG_DEBUG("Enter  " + callString);

  FILESYSTEM::Path const filePath
      = parameterFileDirectoryName_ / specificationFileName_;
  std::ifstream ifs;
  ifs.open(filePath.string().c_str());
  if (!ifs.is_open())
  {
    LOG_ERROR("Unable to open simulator model metatdata file.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  std::string ednString((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
  ifs.close();

  try
  {
    node = edn::read(ednString);
  }
  catch (std::string e)
  {
    LOG_ERROR("Unable to parse EDN file: " + e + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int SimulatorModelImplementation::GetSchemaVersion()
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetSchemaVersion().";
#endif
  LOG_DEBUG("Enter  " + callString);

  edn::EdnNode node;
  if (ParseEdn(node))
  {
    // ParseEdn() logs error
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  LOG_DEBUG("Read " + edn::typeToString(node.type) + ": " + node.value + ".");
  if (node.type == edn::EdnMap)
  {
    std::list<edn::EdnNode>::const_iterator itr;
    for (itr = node.values.begin(); itr != node.values.end(); ++itr)
    {
      // find key
      if (itr->type == edn::EdnString)
      {
        std::string key(itr->value);
        LOG_DEBUG("Read " + edn::typeToString(itr->type) + ": " + itr->value
                  + ".");

        // get value
        ++itr;
        LOG_DEBUG("Read " + edn::typeToString(itr->type) + ": " + itr->value
                  + ".");
        if (key == "kim-api-sm-schema-version")
        {
          if (itr->type == edn::EdnInt)
          {
            std::istringstream(itr->value) >> schemaVersion_;
          }
          else
          {
            LOG_ERROR("Expecting 'EdnInt'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }
        }
      }
    }
  }
  else
  {
    LOG_ERROR("Expecting 'EdnMap'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (schemaVersion_ != 1)
  {
    LOG_ERROR("Unsupported schema version found.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int SimulatorModelImplementation::ReadEdnSchemaV1()
{
#if DEBUG_VERBOSITY
  std::string const callString = "ReadEdnSchemaV1().";
#endif
  LOG_DEBUG("Enter  " + callString);

  edn::EdnNode node;
  if (ParseEdn(node))
  {
    // ParseEdn() logs error
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  LOG_DEBUG("Read " + edn::typeToString(node.type) + ": " + node.value + ".");
  if (node.type == edn::EdnMap)
  {
    std::list<edn::EdnNode>::const_iterator itr;
    for (itr = node.values.begin(); itr != node.values.end(); ++itr)
    {
      // find key
      if (itr->type == edn::EdnString)
      {
        std::string key(itr->value);
        LOG_DEBUG("Read " + edn::typeToString(itr->type) + ": " + itr->value
                  + ".");

        // get value
        ++itr;
        LOG_DEBUG("Read " + edn::typeToString(itr->type) + ": " + itr->value
                  + ".");
        if (key == "kim-api-sm-schema-version")
        {
          if (itr->type == edn::EdnInt)
          {
            std::istringstream(itr->value) >> schemaVersion_;
          }
          else
          {
            LOG_ERROR("Expecting 'EdnInt'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }
        }
        else if (key == "simulator-version")
        {
          if (itr->type == edn::EdnString) { simulatorVersion_ = itr->value; }
          else
          {
            LOG_ERROR("Expecting 'EdnString'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }
        }
        else if (key == "simulator-name")
        {
          if (itr->type == edn::EdnString) { simulatorName_ = itr->value; }
          else
          {
            LOG_ERROR("Expecting 'EdnString'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }
        }
        else if (key == "supported-species")
        {
          if (itr->type == edn::EdnString)
          {
            std::stringstream ss(itr->value);
            while (ss)
            {
              std::string species;
              ss >> species;
              if (species != "") simulatorSupportedSpecies_.push_back(species);
            }
          }
          else
          {
            LOG_ERROR("Expecting 'EdnString'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }
        }
        else if (key == "model-name")
        {
          if (itr->type == edn::EdnString) { modelName_ = itr->value; }
          else
          {
            LOG_ERROR("Expecting 'EdnString'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }
        }
        else  // simulator field
        {
          simulatorFieldNames_.push_back(key);
          std::vector<std::string> lines;

          if (itr->type == edn::EdnVector)
          {
            std::list<edn::EdnNode>::const_iterator vec;
            for (vec = itr->values.begin(); vec != itr->values.end(); ++vec)
            {
              LOG_DEBUG("Read " + edn::typeToString(vec->type) + ": "
                        + vec->value + ".");
              if (vec->type == edn::EdnString) { lines.push_back(vec->value); }
              else
              {
                LOG_ERROR("Expecting 'EdnString'.");
                LOG_DEBUG("Exit 1=" + callString);
                return true;
              }
            }
          }
          else if (itr->type == edn::EdnString) { lines.push_back(itr->value); }
          else
          {
            LOG_ERROR("Expecting 'EdnVector' or 'EdnString'.");
            LOG_DEBUG("Exit 1=" + callString);
            return true;
          }

          originalSimulatorFields_.push_back(lines);
        }
      }
    }
  }
  else
  {
    LOG_ERROR("Expecting 'EdnMap'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}
}  // namespace KIM
