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


#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#ifndef KIM_LOG_IMPLEMENTATION_HPP_
#include "KIM_LogImplementation.hpp"
#endif

#ifndef KIM_FUNCTION_TYPES_H_
extern "C" {
#include "KIM_FunctionTypes.h"
}  // extern "C"
#endif

#ifndef KIM_LOG_VERBOSITY_HPP_
#include "KIM_LogVerbosity.hpp"
#endif

#ifndef KIM_LANGUAGE_NAME_HPP_
#include "KIM_LanguageName.hpp"
#endif

#include "KIM_LOG_DEFINES.inc"

#define LOG_FILE "kim.log"

// log helper
#define SPTR(x)                                                            \
  static_cast<std::ostringstream const &>(std::ostringstream()             \
                                          << static_cast<void const *>(x)) \
      .str()


namespace KIM
{
namespace
{
std::stack<LogVerbosity> GetDefaultVerbosityStack()
{
  std::stack<LogVerbosity> s;
  s.push(KIM_LOG_MAXIMUM_LEVEL);
  return s;
}
std::stack<LogVerbosity> defaultLogVerbosity = GetDefaultVerbosityStack();

int Validate(LogVerbosity const logVerbosity)
{
  int numberOfLogVerbosities;
  LOG_VERBOSITY::GetNumberOfLogVerbosities(&numberOfLogVerbosities);

  for (int i = 0; i < numberOfLogVerbosities; ++i)
  {
    LogVerbosity logVerb;
    LOG_VERBOSITY::GetLogVerbosity(i, &logVerb);

    if (logVerbosity == logVerb) { return true; }
  }

  return false;
}
}  // namespace

int LogImplementation::Create(LogImplementation ** const logImplementation)
{
  *logImplementation = new LogImplementation();

  std::stringstream ss;
  ss << "Log object created.  Default verbosity level is '"
     << defaultLogVerbosity.top().ToString() << "'.";
  (*logImplementation)
      ->LogEntry(LOG_VERBOSITY::information, ss.str(), __LINE__, __FILE__);

  return false;
}

void LogImplementation::Destroy(LogImplementation ** const logImplementation)
{
  if (*logImplementation != NULL)
  {
    (*logImplementation)
        ->LogEntry(LOG_VERBOSITY::information,
                   "Log object destroyed.",
                   __LINE__,
                   __FILE__);
  }
  delete (*logImplementation);
  *logImplementation = NULL;
}

void LogImplementation::PushDefaultVerbosity(LogVerbosity const logVerbosity)
{
  LogVerbosity logVerb(logVerbosity);
  if (!Validate(logVerbosity)) logVerb = defaultLogVerbosity.top();

  defaultLogVerbosity.push(logVerb);
}

void LogImplementation::PopDefaultVerbosity()
{
  defaultLogVerbosity.pop();
  if (defaultLogVerbosity.empty())
  {
    defaultLogVerbosity.push(KIM_LOG_MAXIMUM_LEVEL);
  }
}

namespace
{
int GlobalDefaultLogPrintFunction(std::string const & entryString)
{
  // Need to figure out how to do file locking to make this work for
  // parallel computations.

  std::ofstream file;
  file.open(LOG_FILE, std::ios_base::out | std::ios_base::app);
  if (!file)
  {
    std::cerr << "Unable to open " LOG_FILE " file." << std::endl;
    return true;
  }
  else
  {
    file << entryString;
    return false;
  }
}

std::stack<LanguageName> GetDefaultLogPrintFunctionLanguageNameStack()
{
  std::stack<LanguageName> s;
  s.push(LANGUAGE_NAME::cpp);
  return s;
}
std::stack<LanguageName> defaultLogPrintFunctionLanguageName
    = GetDefaultLogPrintFunctionLanguageNameStack();

std::stack<Function *> GetDefaultLogPrintFunctionPointerStack()
{
  std::stack<Function *> s;
  s.push(reinterpret_cast<Function *>(&GlobalDefaultLogPrintFunction));
  return s;
}
std::stack<Function *> defaultLogPrintFunctionPointer
    = GetDefaultLogPrintFunctionPointerStack();
}  // namespace

void LogImplementation::PushDefaultPrintFunction(
    LanguageName const languageName, Function * const fptr)
{
  LanguageName language(languageName);
  if (!language.Known())
  {
#if ERROR_VERBOSITY
    std::cerr << "Unknown LanguageName passed to PushDefaultPrintFunction, "
                 "using LANGUAGE_NAME::cpp."
              << std::endl;
#endif
    language = LANGUAGE_NAME::cpp;
  }

  defaultLogPrintFunctionLanguageName.push(language);
  defaultLogPrintFunctionPointer.push(fptr);
}

void LogImplementation::PopDefaultPrintFunction()
{
  defaultLogPrintFunctionLanguageName.pop();
  defaultLogPrintFunctionPointer.pop();
  if (defaultLogPrintFunctionLanguageName.empty())
  {
    defaultLogPrintFunctionLanguageName.push(LANGUAGE_NAME::cpp);
    defaultLogPrintFunctionPointer.push(
        reinterpret_cast<Function *>(&GlobalDefaultLogPrintFunction));
  }
}

std::string const & LogImplementation::GetID() const { return idString_; }

namespace
{
std::string SanitizeID(std::string const & id)
{
  std::string idCopy = id;
  std::string::iterator itr;
  for (itr = idCopy.begin(); itr != idCopy.end(); ++itr)
  {
    if (isspace(*itr)) { *itr = '_'; }
    else if ('*' == *itr) { *itr = '_'; }
  }

  return idCopy;
}
}  // namespace

void LogImplementation::SetID(std::string const & id)
{
  std::string const sanitizedID = SanitizeID(id);
  std::stringstream ss;
  ss << "Log object renamed.  ID changed to '" << sanitizedID << "'.";
  std::stringstream tt;
  tt << "Log object renamed.  ID changed from '" << idString_ << "'.";

  LogEntry(LOG_VERBOSITY::information, ss.str(), __LINE__, __FILE__);

  idString_ = sanitizedID;

  LogEntry(LOG_VERBOSITY::information, tt.str(), __LINE__, __FILE__);
}

void LogImplementation::PushVerbosity(LogVerbosity const logVerbosity)
{
  LogVerbosity logVerb(logVerbosity);
  if (!Validate(logVerbosity)) logVerb = verbosity_.top();

  std::stringstream ss;
  ss << "Log verbosity '" << logVerb.ToString() << "' pushed (on top of "
     << verbosity_.top().ToString() << ").";
  LogEntry(LOG_VERBOSITY::information, ss.str(), __LINE__, __FILE__);

  verbosity_.push(logVerb);
}

void LogImplementation::PopVerbosity()
{
  std::stringstream ss;
  ss << "Log verbosity '" << verbosity_.top().ToString()
     << "' popped, revealing '";

  verbosity_.pop();
  if (verbosity_.empty()) { verbosity_.push(defaultLogVerbosity.top()); }

  ss << verbosity_.top().ToString() << "'.";
  LogEntry(LOG_VERBOSITY::information, ss.str(), __LINE__, __FILE__);
}

void LogImplementation::LogEntry(LogVerbosity const logVerbosity,
                                 std::string const & message,
                                 int const lineNumber,
                                 std::string const & fileName) const
{
  LogVerbosity logVerb(logVerbosity);
  if (!Validate(logVerbosity)) logVerb = verbosity_.top();

  if ((logVerb != LOG_VERBOSITY::silent) && (logVerb <= verbosity_.top()))
  {
    std::string tm(GetTimeStamp());
    std::string entry(EntryString(logVerb.ToString(),
                                  tm,
                                  sequence_,
                                  idString_,
                                  message,
                                  lineNumber,
                                  fileName));

    int error = true;
    if (printFunctionLanguageName_ == LANGUAGE_NAME::cpp)
    {
      LogPrintFunction * CppPrint
          = reinterpret_cast<LogPrintFunction *>(printFunctionPointer_);
      error = CppPrint(entry);
    }
    else if (printFunctionLanguageName_ == LANGUAGE_NAME::c)
    {
      KIM_LogPrintFunction * CPrint
          = reinterpret_cast<KIM_LogPrintFunction *>(printFunctionPointer_);
      error = CPrint(entry.c_str());
    }
    else if (printFunctionLanguageName_ == LANGUAGE_NAME::fortran)
    {
      typedef void(kim_log_print_function)(char const * const, int * const);
      kim_log_print_function * FPrint
          = reinterpret_cast<kim_log_print_function *>(printFunctionPointer_);
      FPrint(entry.c_str(), &error);
    }
    else
    {
      std::cerr
          << "Unknown LanguageName for log PrintFunction.  Message follows."
          << std::endl
          << entry;
    }

    if (error)
    {
      std::cerr << "Error occurred in log PrintFunction.  Message follows."
                << std::endl
                << entry;
    }
  }
}

LogImplementation::LogImplementation() :
    idString_(SPTR(this)),
    printFunctionLanguageName_(defaultLogPrintFunctionLanguageName.top()),
    printFunctionPointer_(defaultLogPrintFunctionPointer.top()),
    latestTimeStamp_(""),
    sequence_(0)
{
  verbosity_.push(defaultLogVerbosity.top());
}

LogImplementation::~LogImplementation() {}

std::string LogImplementation::EntryString(std::string const & logVerbosity,
                                           std::string const & date,
                                           int const sequence,
                                           std::string const & idString,
                                           std::string const & message,
                                           int const lineNumberString,
                                           std::string const & fileName)
{
  char const * flName = strrchr(fileName.c_str(), '/');
  if (flName != NULL)
  {
    flName++;  // drop leading '/'
  }
  else { flName = fileName.c_str(); }

  std::stringstream ssPrefix;
  ssPrefix << date << " * " << sequence << " * " << logVerbosity << " * "
           << idString << " * " << flName << ":" << lineNumberString << " * ";
  std::string const prefix(ssPrefix.str());

  std::string line;
  std::stringstream ssMessage(message);
  std::stringstream ss;

  while (std::getline(ssMessage, line, '\n')) { ss << prefix << line << "\n"; }

  return ss.str();
}

std::string LogImplementation::GetTimeStamp() const
{
  time_t rawTime;
  time(&rawTime);
  char date[1024];
#ifdef _WIN32
  struct tm * timeInfo;
  timeInfo = localtime(&rawTime);
  strftime(date, 1023, "%Y-%m-%d:%H:%M:%S%Z", timeInfo);
#else
  struct tm timeInfo;
  localtime_r(&rawTime, &timeInfo);
  strftime(date, 1023, "%Y-%m-%d:%H:%M:%S%Z", &timeInfo);
#endif
  std::string dateString(date);
  if (dateString == latestTimeStamp_) { ++sequence_; }
  else
  {
    sequence_ = 0;
    latestTimeStamp_ = dateString;
  }

  return dateString;
}

}  // namespace KIM
