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


#include "KIM_Configuration.hpp"
#include "KIM_Version.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

#define LINELEN 256

#ifndef KIM_LOG_HPP_
#include "KIM_Log.hpp"
#endif

#ifndef KIM_COLLECTIONS_IMPLEMENTATION_HPP_
#include "KIM_CollectionsImplementation.hpp"
#endif

#ifndef KIM_FILESYSTEM_PATH_HPP_
#include "KIM_FilesystemPath.hpp"
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


namespace
{
typedef std::map<KIM::CollectionItemType,
                 KIM::FILESYSTEM::PathList,
                 KIM::COLLECTION_ITEM_TYPE::Comparator>
    ItemTypeToPathListMap;

KIM::FILESYSTEM::Path LibraryName(KIM::CollectionItemType const itemType,
                                  KIM::FILESYSTEM::Path const & path)
{
  std::string libName = KIM_SHARED_MODULE_PREFIX;
  libName += KIM_PROJECT_NAME;
  libName += "-";

  using namespace KIM::COLLECTION_ITEM_TYPE;
  if (itemType == modelDriver)
    libName += KIM_MODEL_DRIVER_IDENTIFIER;
  else if (itemType == portableModel)
    libName += KIM_PORTABLE_MODEL_IDENTIFIER;
  else if (itemType == simulatorModel)
    libName += KIM_SIMULATOR_MODEL_IDENTIFIER;
  else
    libName += "UNKNOWN-COLLECTION-ITEM-TYPE";

  libName += KIM_SHARED_MODULE_SUFFIX;

  return path / libName;
}

int ProcessConfigFileLine(
    char const * const line,
    KIM::FILESYSTEM::Path const & configFile,
    char const * const identifier,
    std::vector<std::string> const & deprecatedIdentifiers,
    KIM::Log * const log,
    KIM::FILESYSTEM::PathList & userDirs)
{
  char linecpy[LINELEN];
  char * word;
  char const * const sep = " \t=";

  strncpy(linecpy, line, LINELEN - 1);
  linecpy[LINELEN - 1] = '\0';

  word = strtok(linecpy, sep);
  bool const foundIdentifier = !strcmp(identifier, word);
  bool const foundDeprecatedIdentifier
      = deprecatedIdentifiers.end()
        != std::find(deprecatedIdentifiers.begin(),
                     deprecatedIdentifiers.end(),
                     std::string(word));
  if (!foundIdentifier && !foundDeprecatedIdentifier)
  {
    if (log)
    {
      std::stringstream ss;
      ss << "Expected identifier '" << identifier << "' but found '" << word
         << "' instead in '" << configFile << "' file." << std::endl;
      log->LogEntry(KIM::LOG_VERBOSITY::error, ss, __LINE__, __FILE__);
    }
    userDirs.clear();
    return true;
  }
  else if (!foundIdentifier)
  {
    if (log)
    {
      std::stringstream ss;
      ss << "Deprecated identifier '" << word << "' found in " << configFile
         << " file.  Use '" << identifier << "' instead." << std::endl;
      log->LogEntry(KIM::LOG_VERBOSITY::warning, ss, __LINE__, __FILE__);
    }
  }
  word = strtok(NULL, sep);
  userDirs.Parse(word);
  if (userDirs.empty())  // error
  {
    if (log)
    {
      std::stringstream ss;
      ss << "Invalid value in " << configFile << " file: " << word << std::endl;
      log->LogEntry(KIM::LOG_VERBOSITY::error, ss, __LINE__, __FILE__);
    }
    return true;
  }

  return false;
}

void PrivateGetConfigurationFileEnvironmentVariable(std::string & name,
                                                    std::string & value)
{
  name = KIM_ENVIRONMENT_CONFIGURATION_FILE;

  char const * const varVal = getenv(name.c_str());
  if (NULL != varVal)
    value = varVal;
  else
    value.clear();
}

void PrivateGetConfigurationFileName(KIM::FILESYSTEM::Path & fileName)
{
  fileName = KIM_USER_CONFIGURATION_FILE;

  if (fileName.is_relative())
  {
    fileName = KIM::FILESYSTEM::Path::HomePath() / fileName;
  }

  std::string varName;
  std::string varVal;
  PrivateGetConfigurationFileEnvironmentVariable(varName, varVal);
  if (!varVal.empty())
  {
    // ensure we have an absolute path
    KIM::FILESYSTEM::Path path(varVal);
    if (path.is_relative())
    {
      fileName = KIM::FILESYSTEM::Path::current_path() / path;
    }
    else { fileName = varVal; }
  }
}

void PrivateGetEnvironmentVariableName(KIM::CollectionItemType const itemType,
                                       std::string & name)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;

  if (itemType == modelDriver)
  {
    name = KIM_ENVIRONMENT_MODEL_DRIVER_PLURAL_DIR;
  }
  else if (itemType == portableModel)
  {
    name = KIM_ENVIRONMENT_PORTABLE_MODEL_PLURAL_DIR;
  }
  else if (itemType == simulatorModel)
  {
    name = KIM_ENVIRONMENT_SIMULATOR_MODEL_PLURAL_DIR;
  }
}

void PrivateGetProjectNameAndSemVer(std::string & projectName,
                                    std::string & semVer)
{
  projectName = KIM_PROJECT_NAME;
  semVer = KIM_VERSION_STRING;
}

void PrivateGetCWDDirs(ItemTypeToPathListMap & dirsMap)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;

  dirsMap[modelDriver].push_back(".");
  dirsMap[portableModel].push_back(".");
  dirsMap[simulatorModel].push_back(".");
}

void PrivateGetEnvironmentDirs(ItemTypeToPathListMap & dirsMap)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;

  dirsMap[modelDriver].Parse(getenv(KIM_ENVIRONMENT_MODEL_DRIVER_PLURAL_DIR));
  dirsMap[portableModel].Parse(
      getenv(KIM_ENVIRONMENT_PORTABLE_MODEL_PLURAL_DIR));
  dirsMap[simulatorModel].Parse(
      getenv(KIM_ENVIRONMENT_SIMULATOR_MODEL_PLURAL_DIR));
}

int PrivateGetUserDirs(KIM::Log * log, ItemTypeToPathListMap & dirsMap)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;

  KIM::FILESYSTEM::Path configFile;
  PrivateGetConfigurationFileName(configFile);

  std::ifstream cfl;
  cfl.open(configFile.string().c_str());
  if (!cfl)
  {
    // unable to open file; create with default locations
    KIM::FILESYSTEM::Path path = configFile;
    path.remove_filename();

    // create collection directories
    dirsMap[modelDriver].Parse(KIM_USER_MODEL_DRIVERS_DIR_DEFAULT);
    dirsMap[portableModel].Parse(KIM_USER_PORTABLE_MODELS_DIR_DEFAULT);
    dirsMap[simulatorModel].Parse(KIM_USER_SIMULATOR_MODELS_DIR_DEFAULT);

    KIM::CollectionsImplementation::WriteConfigurationFileAndCreateDirectories(
        configFile, dirsMap);
  }
  else
  {
    char line[LINELEN];
    std::vector<std::string> deprecatedIdentifiers;
    cfl.getline(line, LINELEN);
    if ((cfl.fail())
        || (ProcessConfigFileLine(line,
                                  configFile,
                                  KIM_MODEL_DRIVER_PLURAL_DIR_IDENTIFIER,
                                  deprecatedIdentifiers,
                                  log,
                                  dirsMap[modelDriver])))
    {
      cfl.close();
      return true;
    }

    deprecatedIdentifiers.clear();
    deprecatedIdentifiers.push_back("models-dir");
    cfl.getline(line, LINELEN);
    if (cfl.fail()
        || ProcessConfigFileLine(line,
                                 configFile,
                                 KIM_PORTABLE_MODEL_PLURAL_DIR_IDENTIFIER,
                                 deprecatedIdentifiers,
                                 log,
                                 dirsMap[portableModel]))
    {
      cfl.close();
      return true;
    }

    deprecatedIdentifiers.clear();
    cfl.getline(line, LINELEN);
    if (!cfl.fail())
    {
      if (ProcessConfigFileLine(line,
                                configFile,
                                KIM_SIMULATOR_MODEL_PLURAL_DIR_IDENTIFIER,
                                deprecatedIdentifiers,
                                log,
                                dirsMap[simulatorModel]))
      {
        cfl.close();
        return true;
      }
      else { cfl.close(); }
    }
    else
    {
      cfl.close();

      // unable to read SM settings; rewrite file with default SM location
      KIM::FILESYSTEM::Path path = configFile;
      path.remove_filename();

      dirsMap[simulatorModel].Parse(KIM_USER_SIMULATOR_MODELS_DIR_DEFAULT);
      if (dirsMap[simulatorModel].MakeDirectories()) return true;

      std::ofstream fl;
      fl.open(configFile.string().c_str());
      fl << KIM_MODEL_DRIVER_PLURAL_DIR_IDENTIFIER " = "
         << dirsMap[modelDriver].ToString() << "\n";
      fl << KIM_PORTABLE_MODEL_PLURAL_DIR_IDENTIFIER " = "
         << dirsMap[portableModel].ToString() << "\n";
      fl << KIM_SIMULATOR_MODEL_PLURAL_DIR_IDENTIFIER " = "
         << KIM_USER_SIMULATOR_MODELS_DIR_DEFAULT << "\n";
      fl.close();
    }
  }

  return false;
}

void PrivateReplaceORIGIN(KIM::FILESYSTEM::PathList & pathList,
                          KIM::FILESYSTEM::Path const & origin)
{
  using namespace KIM::FILESYSTEM;

  std::vector<Path>::iterator path;
  for (path = pathList.begin(); path != pathList.end(); ++path)
  {
    if ((*path).is_relative())  // ORIGIN should not occurr in an absolute path
    {
      Path current(*path);
      Path left(current);
      Path right;

      // make left contain first directory name in path
      // make right contain remainder of path
      {
        // ensure current has no trailing slash
        if (current.filename().empty()) current = current.parent_path();
        while (!current.parent_path().empty()
               && current.parent_path() != current)
        {
          right = current.filename() / right;
          current = current.parent_path();
          left = current;
        }
        right = right.parent_path();  // remove trailing slash
      }

      if (left == "$ORIGIN" || left == "${ORIGIN}") { *path = origin / right; }
    }
  }
}

void PrivateGetSystemDirs(ItemTypeToPathListMap & dirsMap)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;
  using namespace KIM::FILESYSTEM;

  Path origin(KIM::SharedLibrary::GetORIGIN());

  dirsMap[modelDriver].Parse(KIM_SYSTEM_MODEL_DRIVERS_DIR);
  PrivateReplaceORIGIN(dirsMap[modelDriver], origin);
  dirsMap[portableModel].Parse(KIM_SYSTEM_PORTABLE_MODELS_DIR);
  PrivateReplaceORIGIN(dirsMap[portableModel], origin);
  dirsMap[simulatorModel].Parse(KIM_SYSTEM_SIMULATOR_MODELS_DIR);
  PrivateReplaceORIGIN(dirsMap[simulatorModel], origin);
}

void PrivateGetListOfItemPathsByCollectionAndType(
    KIM::Collection const collection,
    KIM::CollectionItemType const itemType,
    KIM::Log * log,
    std::vector<KIM::FILESYSTEM::Path> & paths)
{
  ItemTypeToPathListMap dirsMap;
  if (collection == KIM::COLLECTION::system)
    PrivateGetSystemDirs(dirsMap);
  else if (collection == KIM::COLLECTION::user)
    PrivateGetUserDirs(log, dirsMap);
  else if (collection == KIM::COLLECTION::environmentVariable)
    PrivateGetEnvironmentDirs(dirsMap);
  else if (collection == KIM::COLLECTION::currentWorkingDirectory)
    PrivateGetCWDDirs(dirsMap);
  const KIM::FILESYSTEM::PathList & listOfDirs = dirsMap[itemType];

  KIM::FILESYSTEM::PathList::const_iterator dir;
  for (dir = listOfDirs.begin(); dir != listOfDirs.end(); ++dir)
  {
    std::vector<KIM::FILESYSTEM::Path> subDirs = dir->Subdirectories();

    std::vector<KIM::FILESYSTEM::Path>::const_iterator subDir;
    for (subDir = subDirs.begin(); subDir != subDirs.end(); ++subDir)
    {
      KIM::FILESYSTEM::Path const libName = LibraryName(itemType, *subDir);
      if (libName.exists()) { paths.push_back(*subDir); }
    }
  }
}

int PrivateGetItemLibraryFileNameByCollectionAndType(
    KIM::Collection const collection,
    KIM::CollectionItemType const itemType,
    std::string const & itemName,
    KIM::Log * log,
    KIM::FILESYSTEM::Path * const fileName)
{
  namespace KC = KIM::COLLECTION;

  ItemTypeToPathListMap dirsMap;
  if (collection == KC::system)
    PrivateGetSystemDirs(dirsMap);
  else if (collection == KC::user)
    PrivateGetUserDirs(log, dirsMap);
  else if (collection == KC::environmentVariable)
    PrivateGetEnvironmentDirs(dirsMap);
  else if (collection == KC::currentWorkingDirectory)
    PrivateGetCWDDirs(dirsMap);
  const KIM::FILESYSTEM::PathList & listOfDirs = dirsMap[itemType];

  KIM::FILESYSTEM::PathList::const_iterator dir;
  for (dir = listOfDirs.begin(); dir != listOfDirs.end(); ++dir)
  {
    KIM::FILESYSTEM::Path const libPath
        = LibraryName(itemType, *dir / itemName);
    if (libPath.exists())
    {
      if (fileName) *fileName = libPath;
      return false;
    }
  }

  if (fileName) fileName->clear();
  return true;
}

int PrivateGetListOfItemMetadataFilesByCollectionAndType(
    KIM::Collection const collection,
    KIM::CollectionItemType const itemType,
    std::string const & itemName,
    KIM::Log * const log,
    std::vector<std::string> & fileNames,
    std::vector<int> & availableAsStrings,
    std::vector<std::string> & fileStrings)
{
  fileNames.clear();
  availableAsStrings.clear();
  fileStrings.clear();

  KIM::FILESYSTEM::Path path;
  if (PrivateGetItemLibraryFileNameByCollectionAndType(
          collection, itemType, itemName, log, &path))
  {
    return true;
  }

  KIM::SharedLibrary lib(log);
  int error = lib.Open(path);
  if (error) { return true; }
  int extent;
  error = lib.GetNumberOfMetadataFiles(&extent);
  if (error)
  {
    lib.Close();
    return true;
  }

  for (int i = 0; i < extent; ++i)
  {
    std::string flnm;
    unsigned int length;
    unsigned char const * data;
    error = lib.GetMetadataFile(i, &flnm, &length, &data);
    if (error)
    {
      lib.Close();
      return true;
    }

    fileNames.push_back(flnm);
    fileStrings.push_back(std::string());
    fileStrings.back().assign(reinterpret_cast<char const *>(data), length);
    availableAsStrings.push_back(
        (strlen(fileStrings.back().c_str()) == length) ? true : false);
  }

  lib.Close();
  return false;
}

int PrivateGetItemLibraryFileNameAndCollection(
    KIM::CollectionItemType const itemType,
    std::string const & itemName,
    KIM::Log * const log,
    KIM::FILESYSTEM::Path * const fileName,
    KIM::Collection * const collection)
{
  namespace KC = KIM::COLLECTION;

  KIM::FILESYSTEM::Path itemPath;
  KIM::Collection col;
  if (!PrivateGetItemLibraryFileNameByCollectionAndType(
          KC::currentWorkingDirectory, itemType, itemName, log, &itemPath))
  {
    col = KC::currentWorkingDirectory;
  }
  else if (!PrivateGetItemLibraryFileNameByCollectionAndType(
               KC::environmentVariable, itemType, itemName, log, &itemPath))
  {
    col = KC::environmentVariable;
  }
  else if (!PrivateGetItemLibraryFileNameByCollectionAndType(
               KC::user, itemType, itemName, log, &itemPath))
  {
    col = KC::user;
  }
  else if (!PrivateGetItemLibraryFileNameByCollectionAndType(
               KC::system, itemType, itemName, log, &itemPath))
  {
    col = KC::system;
  }
  else { return true; }

  if (fileName) *fileName = itemPath;
  if (collection) *collection = col;

  return false;
}

int PrivateGetListOfItemMetadataFiles(KIM::CollectionItemType const itemType,
                                      std::string const & itemName,
                                      KIM::Log * const log,
                                      std::vector<std::string> & fileNames,
                                      std::vector<int> & availableAsStrings,
                                      std::vector<std::string> & fileStrings)
{
  fileNames.clear();
  availableAsStrings.clear();
  fileStrings.clear();

  KIM::Collection collection;
  int error = PrivateGetItemLibraryFileNameAndCollection(
      itemType, itemName, log, NULL, &collection);

  if (error) { return true; }

  return PrivateGetListOfItemMetadataFilesByCollectionAndType(
      collection,
      itemType,
      itemName,
      log,
      fileNames,
      availableAsStrings,
      fileStrings);
}

int PrivateGetItemType(std::string const & itemName,
                       KIM::Log * const log,
                       KIM::CollectionItemType * const itemType)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;

  KIM::CollectionItemType it;
  if (!PrivateGetItemLibraryFileNameAndCollection(
          portableModel, itemName, log, NULL, NULL))
  {
    it = portableModel;
  }
  else if (!PrivateGetItemLibraryFileNameAndCollection(
               simulatorModel, itemName, log, NULL, NULL))
  {
    it = simulatorModel;
  }
  else if (!PrivateGetItemLibraryFileNameAndCollection(
               modelDriver, itemName, log, NULL, NULL))
  {
    it = modelDriver;
  }
  else { return true; }

  *itemType = it;

  return false;
}
}  // namespace


#include "KIM_LogMacros.hpp"
#define KIM_LOGGER_OBJECT_NAME this
namespace KIM
{
int CollectionsImplementation::Create(
    CollectionsImplementation ** const collectionsImplementation)
{
  // error checking of arguments performed as part of CollectionsCreate()

  Log * pLog;
  int error = Log::Create(&pLog);
  if (error) { return true; }

  CollectionsImplementation * pCollectionsImplementation;
  pCollectionsImplementation = new CollectionsImplementation(pLog);
#if DEBUG_VERBOSITY
  std::string const callString
      = "Create(" + SPTR(collectionsImplementation) + ").";
  pCollectionsImplementation->LogEntry(
      LOG_VERBOSITY::debug,
      "Created Log and CollectionsImplementation objects after enter "
          + callString,
      __LINE__,
      __FILE__);
#endif

  *collectionsImplementation = pCollectionsImplementation;
#if DEBUG_VERBOSITY
  (*collectionsImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug, "Exit 0=" + callString, __LINE__, __FILE__);
#endif
  return false;
}

void CollectionsImplementation::Destroy(
    CollectionsImplementation ** const collectionsImplementation)
{
#if DEBUG_VERBOSITY
  std::string callString = "Destroy(" + SPTR(collectionsImplementation) + ").";
  (*collectionsImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug, "Enter  " + callString, __LINE__, __FILE__);
#endif

#if DEBUG_VERBOSITY
  (*collectionsImplementation)
      ->LogEntry(LOG_VERBOSITY::debug,
                 "Destroying CollectionsImplementation object and exit "
                     + callString,
                 __LINE__,
                 __FILE__);
#endif
  delete *collectionsImplementation;  // also deletes Log object
  *collectionsImplementation = NULL;
}

bool CollectionsImplementation::
    WriteConfigurationFileAndCreateDirectories(  // for internal use only
        FILESYSTEM::Path const & fileName,
        std::map<CollectionItemType,
                 FILESYSTEM::PathList,
                 COLLECTION_ITEM_TYPE::Comparator> const & dirsMap)
{
  using namespace KIM::COLLECTION_ITEM_TYPE;

  // create config file directory
  KIM::FILESYSTEM::Path path = fileName;
  path.remove_filename();

  if (path.MakeDirectory()) return true;

  // create/open config file
  std::ofstream fl;
  fl.open(fileName.string().c_str(), std::ofstream::out | std::ofstream::trunc);
  if (!fl) return true;

  // create collection directories
  if (dirsMap.find(modelDriver)->second.MakeDirectories()) return true;
  if (dirsMap.find(portableModel)->second.MakeDirectories()) return true;
  if (dirsMap.find(simulatorModel)->second.MakeDirectories()) return true;

  // write config file
  fl << KIM_MODEL_DRIVER_PLURAL_DIR_IDENTIFIER " = "
     << dirsMap.find(modelDriver)->second << "\n";
  fl << KIM_PORTABLE_MODEL_PLURAL_DIR_IDENTIFIER " = "
     << dirsMap.find(portableModel)->second << "\n";
  fl << KIM_SIMULATOR_MODEL_PLURAL_DIR_IDENTIFIER " = "
     << dirsMap.find(simulatorModel)->second << "\n";
  fl << "\n"
     << "This file was created by a kim-api installation with "
        "<kim-api-uid>:\n"
     << "   " << KIM_UID << "\n"
     << "with path\n"
     << "   " << fileName << "\n\n"
     << "The associated kim-api dynamic library was installed in\n"
     << "   " << SharedLibrary::GetORIGIN() << "\n"
     << "at the time this file was created.\n";
  fl.close();

  return false;
}

int CollectionsImplementation::GetItemType(
    std::string const & itemName, CollectionItemType * const itemType) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetItemType(\"" + itemName + "\", " + SPTR(itemType) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  CollectionItemType it;
  if (PrivateGetItemType(itemName, log_, &it))
  {
    LOG_ERROR("Unable to find item.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  *itemType = it;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetItemLibraryFileNameAndCollection(
    CollectionItemType const itemType,
    std::string const & itemName,
    std::string const ** const fileName,
    Collection * const collection) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetItemLibraryFileNameAndCollection(" + itemType.ToString() + ", \""
        + itemName + "\", " + SPTR(fileName) + ", " + SPTR(collection) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if (!itemType.Known())
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  Collection col;
  FILESYSTEM::Path path;
  if (PrivateGetItemLibraryFileNameAndCollection(
          itemType, itemName, log_, &path, &col))
  {
    LOG_ERROR("Unable to find item.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (fileName)
  {
    // Convert from internal KIM::FILESYSTEM::Path represention to conventional
    // string representation:
    getItemLibraryFileNameAndCollection_FileName_ = path.string();
    *fileName = &getItemLibraryFileNameAndCollection_FileName_;
  }
  if (collection) *collection = col;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::CacheListOfItemMetadataFiles(
    CollectionItemType const itemType,
    std::string const & itemName,
    int * const extent)
{
#if DEBUG_VERBOSITY
  std::string const callString = "CacheListOfItemMetadataFiles("
                                 + itemType.ToString() + ", \"" + itemName
                                 + "\", " + SPTR(extent) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *extent = 0;
  cacheListOfItemMetadataFiles_Names_.clear();
  cacheListOfItemMetadataFiles_availableAsString_.clear();
  cacheListOfItemMetadataFiles_RawData_.clear();

#if ERROR_VERBOSITY
  if (!itemType.Known())
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  int error = PrivateGetListOfItemMetadataFiles(
      itemType,
      itemName,
      log_,
      cacheListOfItemMetadataFiles_Names_,
      cacheListOfItemMetadataFiles_availableAsString_,
      cacheListOfItemMetadataFiles_RawData_);

  if (error)
  {
    LOG_ERROR("Unable to cache item metadata files.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  *extent = cacheListOfItemMetadataFiles_Names_.size();

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetItemMetadataFile(
    int const index,
    std::string const ** const fileName,
    unsigned int * const fileLength,
    unsigned char const ** const fileRawData,
    int * const availableAsString,
    std::string const ** const fileString) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetItemMetadataFile(" + SNUM(index) + ", " + SPTR(fileName) + ", "
        + SPTR(fileLength) + ", " + SPTR(fileRawData) + ", "
        + SPTR(availableAsString) + ", " + SPTR(fileString) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if ((index < 0)
      || (size_t(index) > cacheListOfItemMetadataFiles_Names_.size()))
  {
    LOG_ERROR("Invalid metadata file index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (fileName) *fileName = &(cacheListOfItemMetadataFiles_Names_[index]);
  if (fileLength)
    *fileLength = (cacheListOfItemMetadataFiles_RawData_[index].length() - 1);
  if (fileRawData)
    *fileRawData = reinterpret_cast<unsigned char const *>(
        cacheListOfItemMetadataFiles_RawData_[index].c_str());
  if (availableAsString)
    *availableAsString = cacheListOfItemMetadataFiles_availableAsString_[index];
  if (fileString)
  {
    if (cacheListOfItemMetadataFiles_availableAsString_[index])
      *fileString = &(cacheListOfItemMetadataFiles_RawData_[index]);
    else
      *fileString = NULL;
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::CacheListOfItemNamesByType(
    CollectionItemType const itemType, int * const extent)
{
#if DEBUG_VERBOSITY
  std::string const callString = "CacheListOfItemNamesByType("
                                 + itemType.ToString() + ", " + SPTR(extent)
                                 + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *extent = 0;
  cacheListOfItemNamesByType_.clear();

#if ERROR_VERBOSITY
  if (!itemType.Known())
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  namespace KC = KIM::COLLECTION;

  std::vector<FILESYSTEM::Path> listOfPaths;
  PrivateGetListOfItemPathsByCollectionAndType(
      KC::system, itemType, log_, listOfPaths);

  PrivateGetListOfItemPathsByCollectionAndType(
      KC::user, itemType, log_, listOfPaths);

  PrivateGetListOfItemPathsByCollectionAndType(
      KC::environmentVariable, itemType, log_, listOfPaths);

  PrivateGetListOfItemPathsByCollectionAndType(
      KC::currentWorkingDirectory, itemType, log_, listOfPaths);

  // Sort path list and remove duplicates.
  std::sort(listOfPaths.begin(), listOfPaths.end());
  listOfPaths.erase(std::unique(listOfPaths.begin(), listOfPaths.end()),
                    listOfPaths.end());

  // Get the item names from the KIM::FILESYSTEM::Path entries.
  cacheListOfItemNamesByType_.clear();
  cacheListOfItemNamesByType_.reserve(listOfPaths.size());
  std::vector<FILESYSTEM::Path>::const_iterator path;
  for (path = listOfPaths.begin(); path != listOfPaths.end(); ++path)
  {
    cacheListOfItemNamesByType_.push_back(path->filename().string());
  }

  *extent = cacheListOfItemNamesByType_.size();

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetItemNameByType(
    int const index, std::string const ** const itemName) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetItemNameByType(" + SNUM(index) + ", " + SPTR(itemName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if ((index < 0) || (size_t(index) > cacheListOfItemNamesByType_.size()))
  {
    LOG_ERROR("Invalid item name index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  *itemName = &(cacheListOfItemNamesByType_[index]);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::CacheListOfItemNamesByCollectionAndType(
    Collection const collection,
    CollectionItemType const itemType,
    int * const extent)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "CacheListOfItemNamesByCollectionAndType(" + collection.ToString()
        + ", " + itemType.ToString() + ", " + SPTR(extent) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *extent = 0;
  cacheListOfItemNamesByCollectionAndType_.clear();

#if ERROR_VERBOSITY
  if ((!collection.Known()) || (!itemType.Known()))
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::vector<FILESYSTEM::Path> listOfPaths;
  PrivateGetListOfItemPathsByCollectionAndType(
      collection, itemType, log_, listOfPaths);

  // Sort path list and remove duplicates.
  std::sort(listOfPaths.begin(), listOfPaths.end());
  listOfPaths.erase(std::unique(listOfPaths.begin(), listOfPaths.end()),
                    listOfPaths.end());

  // Extract the item names from the KIM::FILESYSTEM::Path entries.
  cacheListOfItemNamesByCollectionAndType_.clear();
  cacheListOfItemNamesByCollectionAndType_.reserve(listOfPaths.size());
  std::vector<FILESYSTEM::Path>::const_iterator path;
  for (path = listOfPaths.begin(); path != listOfPaths.end(); ++path)
  {
    cacheListOfItemNamesByCollectionAndType_.push_back(
        path->filename().string());
  }

  *extent = cacheListOfItemNamesByCollectionAndType_.size();

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetItemNameByCollectionAndType(
    int const index, std::string const ** const itemName) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetItemNameByCollectionAndType(" + SNUM(index)
                                 + ", " + SPTR(itemName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if ((index < 0)
      || (size_t(index) > cacheListOfItemNamesByCollectionAndType_.size()))
  {
    LOG_ERROR("Invalid item name index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  *itemName = &(cacheListOfItemNamesByCollectionAndType_[index]);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetItemLibraryFileNameByCollectionAndType(
    Collection const collection,
    CollectionItemType const itemType,
    std::string const & itemName,
    std::string const ** const fileName) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetItemLibraryFileNameByCollectionAndType("
                                 + collection.ToString() + ", "
                                 + itemType.ToString() + ", \"" + itemName
                                 + "\", " + SPTR(fileName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if ((!collection.Known()) || (!itemType.Known()))
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  FILESYSTEM::Path path;
  int error = PrivateGetItemLibraryFileNameByCollectionAndType(
      collection, itemType, itemName, log_, &path);
  if (error)
  {
    LOG_ERROR("Unable to find item.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  // Convert from internal KIM::FILESYSTEM::Path representation to conventional
  // std::string representation.
  getItemLibraryFileNameByCollectionAndType_ = path.string();
  *fileName = &getItemLibraryFileNameByCollectionAndType_;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::CacheListOfItemMetadataFilesByCollectionAndType(
    Collection const collection,
    CollectionItemType const itemType,
    std::string const & itemName,
    int * const extent)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "CacheListOfItemMetadataFilesByCollectionAndType("
        + collection.ToString() + ", " + itemType.ToString() + ", \"" + itemName
        + "\", " + SPTR(extent) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *extent = 0;
  cacheListOfItemMetadataFilesByCollectionAndType_FileNames_.clear();
  cacheListOfItemMetadataFilesByCollectionAndType_AvailableAsString_.clear();
  cacheListOfItemMetadataFilesByCollectionAndType_FileRawData_.clear();

#if ERROR_VERBOSITY
  if ((!collection.Known()) || (!itemType.Known()))
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  int error = PrivateGetListOfItemMetadataFilesByCollectionAndType(
      collection,
      itemType,
      itemName,
      log_,
      cacheListOfItemMetadataFilesByCollectionAndType_FileNames_,
      cacheListOfItemMetadataFilesByCollectionAndType_AvailableAsString_,
      cacheListOfItemMetadataFilesByCollectionAndType_FileRawData_);

  if (error)
  {
    LOG_ERROR("Unable to cache item metadata files.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  *extent = cacheListOfItemMetadataFilesByCollectionAndType_FileNames_.size();

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetItemMetadataFileByCollectionAndType(
    int const index,
    std::string const ** const fileName,
    unsigned int * const fileLength,
    unsigned char const ** const fileRawData,
    int * const availableAsString,
    std::string const ** const fileString) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetItemMetadataFileByCollectionAndType(" + SNUM(index) + ", "
        + SPTR(fileName) + ", " + SPTR(fileLength) + ", " + SPTR(fileRawData)
        + ", " + SPTR(availableAsString) + ", " + SPTR(fileString) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if ((index < 0)
      || (size_t(index)
          > cacheListOfItemMetadataFilesByCollectionAndType_FileNames_.size()))
  {
    LOG_ERROR("Invalid metadata file index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (fileName != NULL)
    *fileName
        = &(cacheListOfItemMetadataFilesByCollectionAndType_FileNames_[index]);
  if (fileLength != NULL)
    *fileLength
        = (cacheListOfItemMetadataFilesByCollectionAndType_FileRawData_[index]
               .length()
           - 1);
  if (fileRawData != NULL)
    *fileRawData = reinterpret_cast<unsigned char const *>(
        cacheListOfItemMetadataFilesByCollectionAndType_FileRawData_[index]
            .c_str());
  if (availableAsString != NULL)
    *availableAsString
        = cacheListOfItemMetadataFilesByCollectionAndType_AvailableAsString_
            [index];
  if ((cacheListOfItemMetadataFilesByCollectionAndType_AvailableAsString_
           [index])
      && (fileString != NULL))
    *fileString = &(
        cacheListOfItemMetadataFilesByCollectionAndType_FileRawData_[index]);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void CollectionsImplementation::GetProjectNameAndSemVer(
    std::string const ** const projectName,
    std::string const ** const semVer) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetProjectNameAndSemVer(" + SPTR(projectName)
                                 + ", " + SPTR(semVer) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  PrivateGetProjectNameAndSemVer(getProjectNameAndSemVer_ProjectName_,
                                 getProjectNameAndSemVer_SemVer_);

  if (projectName) *projectName = &getProjectNameAndSemVer_ProjectName_;
  if (semVer) *semVer = &getProjectNameAndSemVer_SemVer_;

  LOG_DEBUG("Exit   " + callString);
}

int CollectionsImplementation::GetEnvironmentVariableName(
    CollectionItemType const itemType, std::string const ** const name) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetEnvironmentVariableName("
                                 + itemType.ToString() + ", " + SPTR(name)
                                 + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if (!itemType.Known())
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  PrivateGetEnvironmentVariableName(itemType, getEnvironmentVariableName_);

  *name = &getEnvironmentVariableName_;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void CollectionsImplementation::GetConfigurationFileEnvironmentVariable(
    std::string const ** const name, std::string const ** const value) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetConfigurationFileEnvironmentVariable("
                                 + SPTR(name) + ", " + SPTR(value) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  PrivateGetConfigurationFileEnvironmentVariable(
      getConfigurationFileEnvironmentVariable_Name_,
      getConfigurationFileEnvironmentVariable_Value_);

  if (name) *name = &getConfigurationFileEnvironmentVariable_Name_;
  if (value) *value = &getConfigurationFileEnvironmentVariable_Value_;

  LOG_DEBUG("Exit   " + callString);
}

void CollectionsImplementation::GetConfigurationFileName(
    std::string const ** const fileName) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetConfigurationFileName(" + SPTR(fileName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  FILESYSTEM::Path path;
  PrivateGetConfigurationFileName(path);

  // Convert from internal KIM::FILESYSTEM::Path representation to conventional
  // std::string representation.
  getConfigurationFileName_ = path.string();
  *fileName = &getConfigurationFileName_;

  LOG_DEBUG("Exit   " + callString);
}

int CollectionsImplementation::CacheListOfDirectoryNames(
    Collection const collection,
    CollectionItemType const itemType,
    int * const extent)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "CacheListOfDirectoryNames(" + collection.ToString() + ", "
        + itemType.ToString() + ", " + SPTR(extent) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *extent = 0;
  cacheListOfDirectoryNames_.clear();

#if ERROR_VERBOSITY
  if ((!collection.Known()) || (!itemType.Known()))
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  ItemTypeToPathListMap dirsMap;
  if (collection == COLLECTION::system) { PrivateGetSystemDirs(dirsMap); }
  else if (collection == COLLECTION::user)
  {
    if (PrivateGetUserDirs(log_, dirsMap))
    {
      LOG_ERROR("Unable to update user collection directories.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
  }
  else if (collection == COLLECTION::environmentVariable)
  {
    PrivateGetEnvironmentDirs(dirsMap);
  }
  else if (collection == COLLECTION::currentWorkingDirectory)
  {
    PrivateGetCWDDirs(dirsMap);
  }

  // Convert from internal KIM::FILESYSTEM::Path representation to conventional
  // std::string representation.
  FILESYSTEM::PathList const & pathList = dirsMap[itemType];
  cacheListOfDirectoryNames_.clear();
  cacheListOfDirectoryNames_.reserve(pathList.size());
  FILESYSTEM::PathList::const_iterator path;
  for (path = pathList.begin(); path != pathList.end(); ++path)
  {
    cacheListOfDirectoryNames_.push_back(path->string());
  }

  *extent = cacheListOfDirectoryNames_.size();

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int CollectionsImplementation::GetDirectoryName(
    int const index, std::string const ** const directoryName) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "GetDirectoryName(" + SNUM(index) + ", " + SPTR(directoryName) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  if ((index < 0) || (size_t(index) > cacheListOfDirectoryNames_.size()))
  {
    LOG_ERROR("Invalid directory index, " + SNUM(index) + ".");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  *directoryName = &(cacheListOfDirectoryNames_[index]);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void CollectionsImplementation::SetLogID(std::string const & logID)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetLogID('" + logID + "').";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->SetID(logID);

  LOG_DEBUG("Exit   " + callString);
}

void CollectionsImplementation::PushLogVerbosity(
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

void CollectionsImplementation::PopLogVerbosity()
{
#if DEBUG_VERBOSITY
  std::string const callString = "PopLogVerbosity().";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->PopVerbosity();

  LOG_DEBUG("Exit   " + callString);
}

void CollectionsImplementation::LogEntry(LogVerbosity const logVerbosity,
                                         std::string const & message,
                                         int const lineNumber,
                                         std::string const & fileName) const
{
  // No debug logs to avoid infinite loop
  log_->LogEntry(logVerbosity, message, lineNumber, fileName);
}

void CollectionsImplementation::LogEntry(LogVerbosity const logVerbosity,
                                         std::stringstream const & message,
                                         int const lineNumber,
                                         std::string const & fileName) const
{
  // No debug logs to avoid infinite loop
  log_->LogEntry(logVerbosity, message, lineNumber, fileName);
}

CollectionsImplementation::CollectionsImplementation(Log * const log) :
    log_(log)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "CollectionsImplementation(" + SPTR(log) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  // nothing to do

  LOG_DEBUG("Exit   " + callString);
}

CollectionsImplementation::~CollectionsImplementation()
{
#if DEBUG_VERBOSITY
  std::string const callString = "~CollectionsImplementation().";
#endif
  LOG_DEBUG("Enter  " + callString);

  // nothing to do

  LOG_DEBUG("Destroying Log object and exit " + callString);
  Log::Destroy(&log_);
}
}  // namespace KIM
