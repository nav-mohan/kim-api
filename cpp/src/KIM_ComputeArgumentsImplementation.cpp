//
// KIM-API: An API for interatomic models
// Copyright (c) 2013--2022, Regents of the University of Minnesota.
// All rights reserved.
//
// Contributors:
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


#include <cstddef>
#include <iomanip>
#include <sstream>

#ifndef KIM_LOG_HPP_
#include "KIM_Log.hpp"
#endif

#ifndef KIM_COMPUTE_ARGUMENTS_IMPLEMENTATION_HPP_
#include "KIM_ComputeArgumentsImplementation.hpp"
#endif

#ifndef KIM_FUNCTION_TYPES_H_
extern "C" {
#include "KIM_FunctionTypes.h"
}
#endif


namespace KIM
{
namespace COMPUTE_ARGUMENT_NAME
{
extern std::vector<ComputeArgumentName> const requiredByAPI_ComputeArguments;
}  // namespace COMPUTE_ARGUMENT_NAME

namespace COMPUTE_CALLBACK_NAME
{
extern std::vector<ComputeCallbackName> const requiredByAPI_ComputeCallbacks;
}  // namespace COMPUTE_CALLBACK_NAME
}  // namespace KIM


// log helpers
#define SNUM(x)                                                \
  static_cast<std::ostringstream const &>(std::ostringstream() \
                                          << std::dec << x)    \
      .str()
#define SPTR(x)                                                            \
  static_cast<std::ostringstream const &>(std::ostringstream()             \
                                          << static_cast<void const *>(x)) \
      .str()
#define SFUNC(x)                                                         \
  static_cast<std::ostringstream const &>(std::ostringstream()           \
                                          << static_cast<Function *>(x)) \
      .str()


#include "KIM_LogMacros.hpp"
#define KIM_LOGGER_OBJECT_NAME this
namespace KIM
{
int ComputeArgumentsImplementation::Create(
    std::string const & modelName,
    std::string const & modelLogID,
    Numbering const modelNumbering,
    Numbering const simulatorNumbering,
    int const numberingOffset,
    ComputeArgumentsImplementation ** const computeArgumentsImplementation)
{
  Log * pLog;
  int error = Log::Create(&pLog);
  if (error) { return true; }
  pLog->SetID(modelLogID + "_" + pLog->GetID());

  *computeArgumentsImplementation = new ComputeArgumentsImplementation(
      modelName, modelNumbering, simulatorNumbering, numberingOffset, pLog);
#if DEBUG_VERBOSITY
  std::string callString = "Create(" + modelName + ", " + modelLogID + ", "
                           + modelNumbering.ToString() + ", "
                           + simulatorNumbering.ToString() + ", "
                           + SNUM(numberingOffset) + ", "
                           + SPTR(computeArgumentsImplementation) + ").";
  (*computeArgumentsImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug,
          "Created Log and ComputeArgumentsImplementation objects and enter "
              + callString,
          __LINE__,
          __FILE__);
#endif


#if DEBUG_VERBOSITY
  (*computeArgumentsImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug, "Exit 0=" + callString, __LINE__, __FILE__);
#endif
  return false;
}

void ComputeArgumentsImplementation::Destroy(
    ComputeArgumentsImplementation ** const computeArgumentsImplementation)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "Destroy(" + SPTR(computeArgumentsImplementation) + ").";
  (*computeArgumentsImplementation)
      ->LogEntry(
          LOG_VERBOSITY::debug, "Enter  " + callString, __LINE__, __FILE__);
#endif

#if DEBUG_VERBOSITY
  (*computeArgumentsImplementation)
      ->LogEntry(LOG_VERBOSITY::debug,
                 "Destroying ComputeArgumentsImplementation object and exit "
                     + callString,
                 __LINE__,
                 __FILE__);
#endif
  delete *computeArgumentsImplementation;  // also deletes Log object
  *computeArgumentsImplementation = NULL;
}

int ComputeArgumentsImplementation::SetArgumentSupportStatus(
    ComputeArgumentName const computeArgumentName,
    SupportStatus const supportStatus)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetArgumentSupportStatus("
                                 + computeArgumentName.ToString() + ", "
                                 + supportStatus.ToString() + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known()) || (!supportStatus.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if ((computeArgumentSupportStatus_[computeArgumentName]
       == SUPPORT_STATUS::requiredByAPI)
      && (supportStatus != SUPPORT_STATUS::requiredByAPI))
  {
    LOG_ERROR("Argument '" + computeArgumentName.ToString()
              + "' SupportStatus is 'requiredByAPI' and cannot be changed.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  computeArgumentSupportStatus_[computeArgumentName] = supportStatus;

  // initialize pointer if not already done
  if (supportStatus != SUPPORT_STATUS::notSupported)
  {
    std::map<ComputeArgumentName const,
             void *,
             COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
        = computeArgumentPointer_.find(computeArgumentName);

    if (result == computeArgumentPointer_.end())
    {
      LOG_DEBUG("Initialize ComputeArgument pointer.");
      computeArgumentPointer_[computeArgumentName] = NULL;
    }
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::GetArgumentSupportStatus(
    ComputeArgumentName const computeArgumentName,
    SupportStatus * const supportStatus) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetArgumentSupportStatus("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(supportStatus) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentSupportStatus_.find(computeArgumentName);
  *supportStatus = result->second;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::SetCallbackSupportStatus(
    ComputeCallbackName const computeCallbackName,
    SupportStatus const supportStatus)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetCallbackSupportStatus("
                                 + computeCallbackName.ToString() + ", "
                                 + supportStatus.ToString() + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeCallbackName.Known()) || (!supportStatus.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if ((computeCallbackSupportStatus_[computeCallbackName]
       == SUPPORT_STATUS::requiredByAPI)
      && (supportStatus != SUPPORT_STATUS::requiredByAPI))
  {
    LOG_ERROR("ComputeCallback '" + computeCallbackName.ToString()
              + "' SupportStatus is 'requiredByAPI' and cannot "
                "be changed.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  computeCallbackSupportStatus_[computeCallbackName] = supportStatus;

  // initialize pointer if not already done
  if (supportStatus != SUPPORT_STATUS::notSupported)
  {
    std::map<ComputeCallbackName const,
             Function *,
             COMPUTE_CALLBACK_NAME::Comparator>::const_iterator result
        = computeCallbackFunctionPointer_.find(computeCallbackName);

    if (result == computeCallbackFunctionPointer_.end())
    {
      LOG_DEBUG("Initialize ComputeCallback pointer.");
      computeCallbackLanguage_[computeCallbackName] = LANGUAGE_NAME::cpp;
      computeCallbackFunctionPointer_[computeCallbackName] = NULL;
      computeCallbackDataObjectPointer_[computeCallbackName] = NULL;
    }
  }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::GetCallbackSupportStatus(
    ComputeCallbackName const computeCallbackName,
    SupportStatus * const supportStatus) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetCallbackSupportStatus("
                                 + computeCallbackName.ToString() + ", "
                                 + SPTR(supportStatus) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeCallbackName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeCallbackName const,
           SupportStatus,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator result
      = computeCallbackSupportStatus_.find(computeCallbackName);
  *supportStatus = result->second;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::SetArgumentPointer(
    ComputeArgumentName const computeArgumentName, int const * const ptr)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetArgumentPointer("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentSupportStatus_.find(computeArgumentName);
  if (result->second == SUPPORT_STATUS::notSupported)
  {
    if (ptr == NULL)
    {
      LOG_WARNING("Setting 'notSupported' ComputeArgument '"
                  + computeArgumentName.ToString()
                  + "' pointer to NULL.  This action, although innocuous, "
                    "is considered an error and should be avoided.");
      LOG_DEBUG("Exit 0=" + callString);
      return false;  // allow innocuous behavior
    }
    else
    {
      LOG_ERROR("Pointer value cannot be set for ComputeArgument '"
                + computeArgumentName.ToString()
                + "' which is 'notSupported'.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
  }
#endif

  computeArgumentPointer_[computeArgumentName]
      = reinterpret_cast<void *>(const_cast<int *>(ptr));

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::SetArgumentPointer(
    ComputeArgumentName const computeArgumentName, int * const ptr)
{
  return SetArgumentPointer(computeArgumentName, const_cast<int const *>(ptr));
}

int ComputeArgumentsImplementation::SetArgumentPointer(
    ComputeArgumentName const computeArgumentName, double const * const ptr)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetArgumentPointer("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentSupportStatus_.find(computeArgumentName);
  if (result->second == SUPPORT_STATUS::notSupported)
  {
    if (ptr == NULL)
    {
      LOG_WARNING("Setting 'notSupported' ComputeArgument '"
                  + computeArgumentName.ToString()
                  + "' pointer to NULL.  This action, although innocuous, "
                    "is considered an error and should be avoided.");
      LOG_DEBUG("Exit 0=" + callString);
      return false;  // allow innocuous behavior
    }
    else
    {
      LOG_ERROR("Pointer value cannot be set for ComputeArgument '"
                + computeArgumentName.ToString()
                + "' which is 'notSupported'.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
  }
#endif

  computeArgumentPointer_[computeArgumentName]
      = reinterpret_cast<void *>(const_cast<double *>(ptr));

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::SetArgumentPointer(
    ComputeArgumentName const computeArgumentName, double * const ptr)
{
  return SetArgumentPointer(computeArgumentName,
                            const_cast<double const *>(ptr));
}

int ComputeArgumentsImplementation::GetArgumentPointer(
    ComputeArgumentName const computeArgumentName, int const ** const ptr) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetArgumentPointer("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator statusResult
      = computeArgumentSupportStatus_.find(computeArgumentName);
  if (statusResult->second == SUPPORT_STATUS::notSupported)
  {
    LOG_ERROR("Pointer value does not exist for ComputeArgument '"
              + (statusResult->first).ToString()
              + "' which is 'notSupported'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeArgumentName const,
           void *,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentPointer_.find(computeArgumentName);
  *ptr = reinterpret_cast<int const *>(result->second);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::GetArgumentPointer(
    ComputeArgumentName const computeArgumentName, int ** const ptr) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetArgumentPointer("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator statusResult
      = computeArgumentSupportStatus_.find(computeArgumentName);
  if (statusResult->second == SUPPORT_STATUS::notSupported)
  {
    LOG_ERROR("Pointer value does not exist for ComputeArgument '"
              + (statusResult->first).ToString()
              + "' which is 'notSupported'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeArgumentName const,
           void *,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentPointer_.find(computeArgumentName);

  *ptr = reinterpret_cast<int *>(result->second);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::GetArgumentPointer(
    ComputeArgumentName const computeArgumentName,
    double const ** const ptr) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetArgumentPointer("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator statusResult
      = computeArgumentSupportStatus_.find(computeArgumentName);
  if (statusResult->second == SUPPORT_STATUS::notSupported)
  {
    LOG_ERROR("Pointer value does not exist for ComputeArgument '"
              + (statusResult->first).ToString()
              + "' which is 'notSupported'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeArgumentName const,
           void *,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentPointer_.find(computeArgumentName);

  *ptr = reinterpret_cast<double const *>(result->second);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::GetArgumentPointer(
    ComputeArgumentName const computeArgumentName, double ** const ptr) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetArgumentPointer("
                                 + computeArgumentName.ToString() + ", "
                                 + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeArgumentName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeArgumentName const,
           SupportStatus,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator statusResult
      = computeArgumentSupportStatus_.find(computeArgumentName);
  if (statusResult->second == SUPPORT_STATUS::notSupported)
  {
    LOG_ERROR("Pointer value does not exist for ComputeArgument '"
              + (statusResult->first).ToString()
              + "' which is 'notSupported'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeArgumentName const,
           void *,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator result
      = computeArgumentPointer_.find(computeArgumentName);

  *ptr = reinterpret_cast<double *>(result->second);

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::SetCallbackPointer(
    ComputeCallbackName const computeCallbackName,
    LanguageName const languageName,
    Function * const fptr,
    void * const dataObject)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetCallbackPointer("
                                 + computeCallbackName.ToString() + ", "
                                 + languageName.ToString() + ", " + SFUNC(fptr)
                                 + ", " + SPTR(dataObject) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeCallbackName.Known()) || (!languageName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeCallbackName const,
           SupportStatus,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator result
      = computeCallbackSupportStatus_.find(computeCallbackName);

  if (result->second == SUPPORT_STATUS::notSupported)
  {
    if (fptr == NULL)
    {
      LOG_WARNING("Setting 'notSupported' ComputeCallback '"
                  + computeCallbackName.ToString()
                  + "' pointer to NULL.  This action, although innocuous, "
                    "is considered an error and should be avoided.");
      LOG_DEBUG("Exit 0=" + callString);
      return false;  // allow innocuous behavior
    }
    else
    {
      LOG_ERROR("Pointer value cannot be set for ComputeCallback '"
                + computeCallbackName.ToString() + "' that is 'notSupported'.");
      LOG_DEBUG("Exit 1=" + callString);
      return true;
    }
  }
#endif

  computeCallbackLanguage_[computeCallbackName] = languageName;
  computeCallbackFunctionPointer_[computeCallbackName] = fptr;
  computeCallbackDataObjectPointer_[computeCallbackName] = dataObject;

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::IsCallbackPresent(
    ComputeCallbackName const computeCallbackName, int * const present) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "IsCallbackPresent("
                                 + computeCallbackName.ToString() + ", "
                                 + SPTR(present) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  int error = (!computeCallbackName.Known());
  if (error)
  {
    LOG_ERROR("Invalid arguments.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  std::map<ComputeCallbackName const,
           SupportStatus,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator statusResult
      = computeCallbackSupportStatus_.find(computeCallbackName);
  if (statusResult->second == SUPPORT_STATUS::notSupported)
  {
    LOG_ERROR("Pointer value does not exist for ComputeCallback '"
              + (statusResult->first).ToString()
              + "' which is 'notSupported'.");
    LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeCallbackName const,
           Function *,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator result
      = computeCallbackFunctionPointer_.find(computeCallbackName);

  if (result->second == NULL) { *present = false; }
  else { *present = true; }

  LOG_DEBUG("Exit 0=" + callString);
  return false;
}

void ComputeArgumentsImplementation::AreAllRequiredArgumentsAndCallbacksPresent(
    int * const result) const
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "AreAllRequiredArgumentsAndCallbacksPresent(" + SPTR(result) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  // Check that all required compute arguments are present
  for (std::map<ComputeArgumentName const,
                SupportStatus,
                COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator itr
       = computeArgumentSupportStatus_.begin();
       itr != computeArgumentSupportStatus_.end();
       ++itr)
  {
    if ((itr->second == SUPPORT_STATUS::requiredByAPI)
        || (itr->second == SUPPORT_STATUS::required))
    {
      if (computeArgumentPointer_.find(itr->first)->second == NULL)
      {
        LOG_ERROR("Required ComputeArgument '" + (itr->first).ToString()
                  + "' is not present.");

        *result = false;
        LOG_DEBUG("Exit   " + callString);
        return;
      }
    }
  }

  // Check that all required callbacks are present
  for (std::map<ComputeCallbackName const,
                SupportStatus,
                COMPUTE_CALLBACK_NAME::Comparator>::const_iterator itr
       = computeCallbackSupportStatus_.begin();
       itr != computeCallbackSupportStatus_.end();
       ++itr)
  {
    if ((itr->second == SUPPORT_STATUS::requiredByAPI)
        || (itr->second == SUPPORT_STATUS::required))
    {
      if (computeCallbackFunctionPointer_.find(itr->first)->second == NULL)
      {
        LOG_ERROR("Required ComputeCallback '" + (itr->first).ToString()
                  + "' is not present.");

        *result = false;
        LOG_DEBUG("Exit   " + callString);
        return;
      }
    }
  }

  *result = true;
  LOG_DEBUG("Exit   " + callString);
  return;
}

int ComputeArgumentsImplementation::GetNeighborList(
    int const neighborListIndex,
    int const particleNumber,
    int * const numberOfNeighbors,
    int const ** const neighborsOfParticle) const
{
  // No debug logging for callbacks: too expensive
  //
  // #if DEBUG_VERBOSITY
  //   std::string const callString = "GetNeighborList("
  //       + SNUM(neighborListIndex) + ", " + SNUM(particleNumber) + ", "
  //       + SPTR(numberOfNeighbors) + ", " + SPTR(neighborsOfParticle) + ").";
  // #endif
  //   LOG_DEBUG("Enter  " + callString);

#if ERROR_VERBOSITY
  if ((neighborListIndex < 0) || (neighborListIndex >= numberOfNeighborLists_))
  {
    LOG_ERROR("Invalid neighborListIndex, " + SNUM(neighborListIndex) + ".");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  int zeroBasedParticleNumber
      = particleNumber + ((NUMBERING::zeroBased == modelNumbering_) ? 0 : -1);
  std::map<ComputeArgumentName const,
           void *,
           COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator pointerResult
      = computeArgumentPointer_.find(COMPUTE_ARGUMENT_NAME::numberOfParticles);
  int const * numberOfParticles
      = reinterpret_cast<int const *>(pointerResult->second);
  if ((zeroBasedParticleNumber < 0)
      || (zeroBasedParticleNumber >= *(numberOfParticles)))
  {
    LOG_ERROR("Invalid particleNumber, " + SNUM(zeroBasedParticleNumber) + ".");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
#endif

  std::map<ComputeCallbackName const,
           LanguageName,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator languageResult
      = computeCallbackLanguage_.find(COMPUTE_CALLBACK_NAME::GetNeighborList);

  LanguageName const languageName = languageResult->second;
  void const * dataObject = (computeCallbackDataObjectPointer_.find(
                                 COMPUTE_CALLBACK_NAME::GetNeighborList))
                                ->second;

  Function * functionPointer = (computeCallbackFunctionPointer_.find(
                                    COMPUTE_CALLBACK_NAME::GetNeighborList))
                                   ->second;
  GetNeighborListFunction * CppGetNeighborList
      = reinterpret_cast<GetNeighborListFunction *>(functionPointer);
  KIM_GetNeighborListFunction * CGetNeighborList
      = reinterpret_cast<KIM_GetNeighborListFunction *>(functionPointer);
  typedef void GetNeighborListF(void * const dataObject,
                                int const numberOfNeighborLists,
                                double const * const cutoffs,
                                int const neighborListIndex,
                                int const particleNumber,
                                int * const numberOfNeighbors,
                                int const ** const neighborsOfParticle,
                                int * const ierr);
  GetNeighborListF * FGetNeighborList
      = reinterpret_cast<GetNeighborListF *>(functionPointer);


  int simulatorParticleNumber
      = particleNumber
        + ((simulatorNumbering_ == modelNumbering_) ? 0 : -numberingOffset_);
  int const * simulatorNeighborsOfParticle;
  int error;
  if (languageName == LANGUAGE_NAME::cpp)
  {
    error = CppGetNeighborList(const_cast<void *>(dataObject),
                               numberOfNeighborLists_,
                               cutoffs_,
                               neighborListIndex,
                               simulatorParticleNumber,
                               numberOfNeighbors,
                               &simulatorNeighborsOfParticle);
  }
  else if (languageName == LANGUAGE_NAME::c)
  {
    error = CGetNeighborList(const_cast<void *>(dataObject),
                             numberOfNeighborLists_,
                             cutoffs_,
                             neighborListIndex,
                             simulatorParticleNumber,
                             numberOfNeighbors,
                             &simulatorNeighborsOfParticle);
  }
  else if (languageName == LANGUAGE_NAME::fortran)
  {
    FGetNeighborList(const_cast<void *>(dataObject),
                     numberOfNeighborLists_,
                     cutoffs_,
                     neighborListIndex + 1,
                     simulatorParticleNumber,
                     numberOfNeighbors,
                     &simulatorNeighborsOfParticle,
                     &error);
  }
  else
  {
    LOG_ERROR("Unknown LanguageName.  SHOULD NEVER GET HERE.");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (error)
  {
    LOG_ERROR("Simulator supplied GetNeighborList() routine returned error.");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  // account for numbering differences if needed
  if (simulatorNumbering_ != modelNumbering_)
  {
    // LOG_DEBUG("Numbering conversion is required.");

    std::vector<int> & list = getNeighborListStorage_[neighborListIndex];
    list.resize(*numberOfNeighbors);
    for (int i = 0; i < *numberOfNeighbors; ++i)
      list[i] = simulatorNeighborsOfParticle[i] + numberingOffset_;

    *neighborsOfParticle = list.data();
  }
  else
  {
    // LOG_DEBUG("Numbering conversion is not required.");

    *neighborsOfParticle = simulatorNeighborsOfParticle;
  }

  // LOG_DEBUG("Exit 0=" + callString);
  return false;
}

int ComputeArgumentsImplementation::ProcessDEDrTerm(double const de,
                                                    double const r,
                                                    double const * const dx,
                                                    int const i,
                                                    int const j) const
{
  // No debug logging for callbacks: too expensive
  //
  // #if DEBUG_VERBOSITY
  //   std::string const callString = "ProcessDEDrTerm("
  //       + SNUM(de) + ", " + SNUM(r) + ", " + SPTR(dx) + ", "
  //       + SNUM(i) + ", " + SNUM(j) + ").";
  // #endif
  //   LOG_DEBUG("Enter  " + callString);

  std::map<ComputeCallbackName const,
           LanguageName,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator languageResult
      = computeCallbackLanguage_.find(COMPUTE_CALLBACK_NAME::ProcessDEDrTerm);

  LanguageName languageName = languageResult->second;
  void const * dataObject = (computeCallbackDataObjectPointer_.find(
                                 COMPUTE_CALLBACK_NAME::ProcessDEDrTerm))
                                ->second;

  Function * functionPointer = (computeCallbackFunctionPointer_.find(
                                    COMPUTE_CALLBACK_NAME::ProcessDEDrTerm))
                                   ->second;
  ProcessDEDrTermFunction * CppProcess_dEdr
      = reinterpret_cast<ProcessDEDrTermFunction *>(functionPointer);
  KIM_ProcessDEDrTermFunction * CProcess_dEdr
      = reinterpret_cast<KIM_ProcessDEDrTermFunction *>(functionPointer);
  typedef void ProcessDEDrTermF(void * const dataObject,
                                double const de,
                                double const r,
                                double const(*const dx)[3],
                                int const i,
                                int const j,
                                int * const ierr);
  ProcessDEDrTermF * FProcess_dEdr
      = reinterpret_cast<ProcessDEDrTermF *>(functionPointer);

  int offset
      = ((simulatorNumbering_ == modelNumbering_) ? 0 : -numberingOffset_);
  int simulatorI = i + offset;
  int simulatorJ = j + offset;

  int error;
  if (languageName == LANGUAGE_NAME::cpp)
  {
    error = CppProcess_dEdr(
        const_cast<void *>(dataObject), de, r, dx, simulatorI, simulatorJ);
  }
  else if (languageName == LANGUAGE_NAME::c)
  {
    error = CProcess_dEdr(
        const_cast<void *>(dataObject), de, r, dx, simulatorI, simulatorJ);
  }
  else if (languageName == LANGUAGE_NAME::fortran)
  {
    FProcess_dEdr(const_cast<void *>(dataObject),
                  de,
                  r,
                  reinterpret_cast<double const(*)[3]>(dx),
                  simulatorI,
                  simulatorJ,
                  &error);
  }
  else
  {
    LOG_ERROR("Unknown LanguageName.  SHOULD NEVER GET HERE.");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (error)
  {
    LOG_ERROR("Simulator supplied ProcessDEDrTerm() routine returned error.");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  else
  {
    // LOG_DEBUG("Exit 0=" + callString);
    return false;
  }
}

int ComputeArgumentsImplementation::ProcessD2EDr2Term(double const de,
                                                      double const * const r,
                                                      double const * const dx,
                                                      int const * const i,
                                                      int const * const j) const
{
  // No debug logging for callbacks: too expensive
  //
  // #if DEBUG_VERBOSITY
  //   std::string const callString = "ProcessD2EDr2Term("
  //       + SNUM(de) + ", " + SPTR(r) + ", " + SPTR(dx) + ", "
  //       + SPTR(i) + ", " + SPTR(j) + ").";
  // #endif
  //   LOG_DEBUG("Enter  " + callString);
  std::map<ComputeCallbackName const,
           LanguageName,
           COMPUTE_CALLBACK_NAME::Comparator>::const_iterator languageResult
      = computeCallbackLanguage_.find(COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term);

  LanguageName languageName = languageResult->second;
  void const * dataObject = (computeCallbackDataObjectPointer_.find(
                                 COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term))
                                ->second;

  Function * functionPointer = (computeCallbackFunctionPointer_.find(
                                    COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term))
                                   ->second;
  ProcessD2EDr2TermFunction * CppProcess_d2Edr2
      = reinterpret_cast<ProcessD2EDr2TermFunction *>(functionPointer);
  KIM_ProcessD2EDr2TermFunction * CProcess_d2Edr2
      = reinterpret_cast<KIM_ProcessD2EDr2TermFunction *>(functionPointer);
  typedef void ProcessD2EDr2TermF(void * const dataObject,
                                  double const de,
                                  double const(*const r)[2],
                                  double const(*const dx)[2][3],
                                  int const(*const i)[2],
                                  int const(*const j)[2],
                                  int * const ierr);
  ProcessD2EDr2TermF * FProcess_d2Edr2
      = reinterpret_cast<ProcessD2EDr2TermF *>(functionPointer);

  int offset
      = ((simulatorNumbering_ == modelNumbering_) ? 0 : -numberingOffset_);
  int simulatorI[2];
  simulatorI[0] = i[0] + offset;
  simulatorI[1] = i[1] + offset;

  int simulatorJ[2];
  simulatorJ[0] = j[0] + offset;
  simulatorJ[1] = j[1] + offset;

  int error;
  if (languageName == LANGUAGE_NAME::cpp)
  {
    error = CppProcess_d2Edr2(
        const_cast<void *>(dataObject), de, r, dx, simulatorI, simulatorJ);
  }
  else if (languageName == LANGUAGE_NAME::c)
  {
    error = CProcess_d2Edr2(
        const_cast<void *>(dataObject), de, r, dx, simulatorI, simulatorJ);
  }
  else if (languageName == LANGUAGE_NAME::fortran)
  {
    FProcess_d2Edr2(const_cast<void *>(dataObject),
                    de,
                    reinterpret_cast<double const(*)[2]>(r),
                    reinterpret_cast<double const(*)[2][3]>(dx),
                    &simulatorI,
                    &simulatorJ,
                    &error);
  }
  else
  {
    LOG_ERROR("Unknown LanguageName.  SHOULD NEVER GET HERE.");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }

  if (error)
  {
    LOG_ERROR("Simulator supplied ProcessD2EDr2Term() routine returned error.");
    // LOG_DEBUG("Exit 1=" + callString);
    return true;
  }
  else
  {
    // LOG_DEBUG("Exit 0=" + callString);
    return false;
  }
}

void ComputeArgumentsImplementation::SetModelBufferPointer(void * const ptr)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetModelBufferPointer(" + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  modelBuffer_ = ptr;

  LOG_DEBUG("Exit   " + callString);
}

void ComputeArgumentsImplementation::GetModelBufferPointer(
    void ** const ptr) const
{
#if DEBUG_VERBOSITY
  std::string const callString = "GetModelBufferPointer(" + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  *ptr = modelBuffer_;

  LOG_DEBUG("Exit   " + callString);
}


void ComputeArgumentsImplementation::SetSimulatorBufferPointer(void * const ptr)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "SetSimulatorBufferPointer(" + SPTR(ptr) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  simulatorBuffer_ = ptr;

  LOG_DEBUG("Exit   " + callString);
}

void ComputeArgumentsImplementation::GetSimulatorBufferPointer(
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

void ComputeArgumentsImplementation::SetLogID(std::string const & logID)
{
#if DEBUG_VERBOSITY
  std::string const callString = "SetLogID('" + logID + "').";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->SetID(logID);

  LOG_DEBUG("Exit   " + callString);
}

void ComputeArgumentsImplementation::PushLogVerbosity(
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

void ComputeArgumentsImplementation::PopLogVerbosity()
{
#if DEBUG_VERBOSITY
  std::string const callString = "PopLogVerbosity().";
#endif
  LOG_DEBUG("Enter  " + callString);

  log_->PopVerbosity();

  LOG_DEBUG("Exit   " + callString);
}

void ComputeArgumentsImplementation::LogEntry(
    LogVerbosity const logVerbosity,
    std::string const & message,
    int const lineNumber,
    std::string const & fileName) const
{
  // No debug logs to avoid infinite loop
  log_->LogEntry(logVerbosity, message, lineNumber, fileName);
}

void ComputeArgumentsImplementation::LogEntry(
    LogVerbosity const logVerbosity,
    std::stringstream const & message,
    int const lineNumber,
    std::string const & fileName) const
{
  // No debug logs to avoid infinite loop
  log_->LogEntry(logVerbosity, message, lineNumber, fileName);
}

std::string const & ComputeArgumentsImplementation::ToString() const
{
#if DEBUG_VERBOSITY
  std::string const callString = "ToString().";
#endif
  LOG_DEBUG("Enter  " + callString);

  std::stringstream ss;
  ss << std::setprecision(10) << std::scientific << std::left;
  ss << "====================================================================="
        "===========\n\n";

  ss << "ComputeArguments object\n"
     << "-----------------------\n\n";
  ss << "Model Name : " << modelName_ << "\n";
  ss << "Log ID : " << log_->GetID() << "\n";
  ss << "\n";

  ss << "Compute Arguments :\n";
  int const argW = 25;
  ss << "\t" << std::setw(argW) << "Compute Argument Name" << std::setw(argW)
     << "SupportStatus" << std::setw(argW) << "Pointer"
     << "\n";
  ss << "\t" << std::setw(argW) << "-------------------------"
     << std::setw(argW) << "-------------------------" << std::setw(argW)
     << "-------------------------"
     << "\n\n";
  for (std::map<ComputeArgumentName const,
                SupportStatus,
                COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator argName
       = computeArgumentSupportStatus_.begin();
       argName != computeArgumentSupportStatus_.end();
       ++argName)
  {
    ss << "\t" << std::setw(argW) << (argName->first).ToString()
       << std::setw(argW) << (argName->second).ToString();

    if ((argName->second) != SUPPORT_STATUS::notSupported)
    {
      std::map<ComputeArgumentName const,
               void *,
               COMPUTE_ARGUMENT_NAME::Comparator>::const_iterator ptr
          = computeArgumentPointer_.find(argName->first);
      if (ptr != computeArgumentPointer_.end())
      {
        ss << std::setw(argW) << SPTR(ptr->second);
      }
      else { ss << std::setw(argW) << "Not Set"; }
    }
    else { ss << std::setw(argW) << "N/A"; }
    ss << "\n";
  }
  ss << "\n";


  ss << "Compute Callback Functions :\n";
  int const cbWn = 25;
  int const cbWs = 15;
  int const cbWl = 12;
  int const cbWd = 25;
  int const cbWf = 25;
  ss << "\t" << std::setw(cbWn) << "Compute Callback Name" << std::setw(cbWs)
     << "SupportStatus" << std::setw(cbWl) << "Language" << std::setw(cbWd)
     << "Data Pointer" << std::setw(cbWf) << "Pointer (1-set / 0-unset)"
     << "\n";
  ss << "\t" << std::setw(cbWn) << "-------------------------"
     << std::setw(cbWs) << "---------------" << std::setw(cbWl)
     << "------------" << std::setw(cbWd) << "-------------------------"
     << std::setw(cbWf) << "-------------------------"
     << "\n\n";
  for (std::map<ComputeCallbackName const,
                SupportStatus,
                COMPUTE_CALLBACK_NAME::Comparator>::const_iterator cbName
       = computeCallbackSupportStatus_.begin();
       cbName != computeCallbackSupportStatus_.end();
       ++cbName)
  {
    ss << "\t" << std::setw(cbWn) << (cbName->first).ToString()
       << std::setw(cbWs) << (cbName->second).ToString();

    if ((cbName->second) != SUPPORT_STATUS::notSupported)
    {
      std::map<ComputeCallbackName const,
               LanguageName,
               COMPUTE_CALLBACK_NAME::Comparator>::const_iterator ptr
          = computeCallbackLanguage_.find(cbName->first);
      if (ptr != computeCallbackLanguage_.end())
      {
        ss << std::setw(cbWl) << (ptr->second).ToString();
        std::map<ComputeCallbackName const,
                 void *,
                 COMPUTE_CALLBACK_NAME::Comparator>::const_iterator ptr2
            = computeCallbackDataObjectPointer_.find(cbName->first);
        ss << std::setw(cbWd) << SPTR(ptr2->second);
        std::map<ComputeCallbackName const,
                 Function *,
                 COMPUTE_CALLBACK_NAME::Comparator>::const_iterator ptr3
            = computeCallbackFunctionPointer_.find(cbName->first);
        ss << std::setw(cbWf) << SFUNC(ptr3->second);
      }
      else { ss << std::setw(cbWl) << "Not Set"; }
    }
    else { ss << std::setw(cbWs) << "N/A"; }
    ss << "\n";
  }
  ss << "\n";

  ss << "Buffers\n";
  ss << "\t" << std::setw(15) << "Buffer" << std::setw(15) << "Pointer"
     << "\n";
  ss << "\t" << std::setw(15) << "---------------" << std::setw(15)
     << "---------------"
     << "\n\n";
  ss << "\t" << std::setw(15) << "Model" << std::setw(15) << SPTR(modelBuffer_)
     << "\n"
     << "\t" << std::setw(15) << "Simulator" << std::setw(15)
     << SPTR(simulatorBuffer_) << "\n\n";

  ss << "====================================================================="
        "===========\n";

  string_ = ss.str();
  LOG_DEBUG("Exit   " + callString);
  return string_;
}


ComputeArgumentsImplementation::ComputeArgumentsImplementation(
    std::string const & modelName,
    Numbering const modelNumbering,
    Numbering const simulatorNumbering,
    int const numberingOffset,
    Log * const log) :
    modelName_(modelName),
    log_(log),
    modelNumbering_(modelNumbering),
    simulatorNumbering_(simulatorNumbering),
    numberingOffset_(numberingOffset),
    inModelComputeRoutine_(false),
    numberOfNeighborLists_(0),
    cutoffs_(NULL),
    modelBuffer_(NULL),
    simulatorBuffer_(NULL)
{
#if DEBUG_VERBOSITY
  std::string const callString
      = "ComputeArgumentsImplementation(" + modelName + ", " + SPTR(log) + ").";
#endif
  LOG_DEBUG("Enter  " + callString);

  // populate ComputeArguments
  int numberOfComputeArgumentNames;
  COMPUTE_ARGUMENT_NAME::GetNumberOfComputeArgumentNames(
      &numberOfComputeArgumentNames);
  for (int i = 0; i < numberOfComputeArgumentNames; ++i)
  {
    ComputeArgumentName computeArgumentName;
    COMPUTE_ARGUMENT_NAME::GetComputeArgumentName(i, &computeArgumentName);
    computeArgumentSupportStatus_[computeArgumentName]
        = SUPPORT_STATUS::notSupported;
  }
  // populate requiredByAPI ComputeArguments
  for (std::vector<ComputeArgumentName>::const_iterator
           requiredByAPI_ComputeArgument
       = COMPUTE_ARGUMENT_NAME::requiredByAPI_ComputeArguments.begin();
       requiredByAPI_ComputeArgument
       != COMPUTE_ARGUMENT_NAME::requiredByAPI_ComputeArguments.end();
       ++requiredByAPI_ComputeArgument)
  {
    computeArgumentSupportStatus_[*requiredByAPI_ComputeArgument]
        = SUPPORT_STATUS::requiredByAPI;
    computeArgumentPointer_[*requiredByAPI_ComputeArgument] = NULL;
  }

  // populate ComputeCallbacks
  int numberOfComputeCallbackNames;
  COMPUTE_CALLBACK_NAME::GetNumberOfComputeCallbackNames(
      &numberOfComputeCallbackNames);
  for (int i = 0; i < numberOfComputeCallbackNames; ++i)
  {
    ComputeCallbackName computeCallbackName;
    COMPUTE_CALLBACK_NAME::GetComputeCallbackName(i, &computeCallbackName);
    computeCallbackSupportStatus_[computeCallbackName]
        = SUPPORT_STATUS::notSupported;
  }
  // populate ComputeCallbacks
  for (std::vector<ComputeCallbackName>::const_iterator
           requiredByAPI_ComputeCallback
       = COMPUTE_CALLBACK_NAME::requiredByAPI_ComputeCallbacks.begin();
       requiredByAPI_ComputeCallback
       != COMPUTE_CALLBACK_NAME::requiredByAPI_ComputeCallbacks.end();
       ++requiredByAPI_ComputeCallback)
  {
    computeCallbackSupportStatus_[*requiredByAPI_ComputeCallback]
        = SUPPORT_STATUS::requiredByAPI;
    computeCallbackLanguage_[*requiredByAPI_ComputeCallback]
        = LANGUAGE_NAME::cpp;  // place holder
    computeCallbackFunctionPointer_[*requiredByAPI_ComputeCallback] = NULL;
    computeCallbackDataObjectPointer_[*requiredByAPI_ComputeCallback] = NULL;
  }

  LOG_DEBUG("Exit   " + callString);
}

ComputeArgumentsImplementation::~ComputeArgumentsImplementation()
{
#if DEBUG_VERBOSITY
  std::string const callString = "~ComputeArgumentsImplementation().";
#endif
  LOG_DEBUG("Enter  " + callString);

  LOG_DEBUG("Destroying Log object and exit " + callString);
  Log::Destroy(&log_);
}
}  // namespace KIM
