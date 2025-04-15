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
// Release: This file is part of the kim-api-2.4.1 package.
//


#include <cstddef>

#ifndef KIM_LOG_VERBOSITY_HPP_
#include "KIM_LogVerbosity.hpp"
#endif

#ifndef KIM_LANGUAGE_NAME_HPP_
#include "KIM_LanguageName.hpp"
#endif

#ifndef KIM_MODEL_ROUTINE_NAME_HPP_
#include "KIM_ModelRoutineName.hpp"
#endif

#ifndef KIM_NUMBERING_HPP_
#include "KIM_Numbering.hpp"
#endif

#ifndef KIM_SPECIES_NAME_HPP_
#include "KIM_SpeciesName.hpp"
#endif

#ifndef KIM_UNIT_SYSTEM
#include "KIM_UnitSystem.hpp"
#endif

#ifndef KIM_MODEL_DRIVER_CREATE_HPP_
#include "KIM_ModelDriverCreate.hpp"
#endif

#ifndef KIM_MODEL_IMPLEMENTATION_HPP_
#include "KIM_ModelImplementation.hpp"
#endif

#define CONVERT_POINTER \
  ModelImplementation * pImpl = reinterpret_cast<ModelImplementation *>(pimpl)


namespace KIM
{
void ModelDriverCreate::GetParameterFileDirectoryName(
    std::string const ** const directoryName) const
{
  CONVERT_POINTER;

  pImpl->GetParameterFileDirectoryName(directoryName);
}

void ModelDriverCreate::GetNumberOfParameterFiles(
    int * const numberOfParameterFiles) const
{
  CONVERT_POINTER;

  pImpl->GetNumberOfParameterFiles(numberOfParameterFiles);
}

int ModelDriverCreate::GetParameterFileName(
    int const index, std::string const ** const parameterFileName) const
{
  CONVERT_POINTER;

  return pImpl->GetParameterFileName(index, parameterFileName);
}
int ModelDriverCreate::GetParameterFileBasename(
    int const index, std::string const ** const parameterFileBasename) const
{
  CONVERT_POINTER;

  return pImpl->GetParameterFileBasename(index, parameterFileBasename);
}


int ModelDriverCreate::SetModelNumbering(Numbering const numbering)
{
  CONVERT_POINTER;

  return pImpl->SetModelNumbering(numbering);
}

void ModelDriverCreate::SetInfluenceDistancePointer(
    double const * const influenceDistance)
{
  CONVERT_POINTER;

  pImpl->SetInfluenceDistancePointer(influenceDistance);
}

void ModelDriverCreate::SetNeighborListPointers(
    int const numberOfNeighborLists,
    double const * const cutoffs,
    int const * const modelWillNotRequestNeighborsOfNoncontributingParticles)
{
  CONVERT_POINTER;

  pImpl->SetNeighborListPointers(
      numberOfNeighborLists,
      cutoffs,
      modelWillNotRequestNeighborsOfNoncontributingParticles);
}

int ModelDriverCreate::SetRoutinePointer(
    ModelRoutineName const modelRoutineName,
    LanguageName const languageName,
    int const required,
    Function * const fptr)
{
  CONVERT_POINTER;

  return pImpl->SetRoutinePointer(
      modelRoutineName, languageName, required, fptr);
}

int ModelDriverCreate::SetSpeciesCode(SpeciesName const speciesName,
                                      int const code)
{
  CONVERT_POINTER;

  return pImpl->SetSpeciesCode(speciesName, code);
}

int ModelDriverCreate::SetParameterPointer(int const extent,
                                           int * const ptr,
                                           std::string const & name,
                                           std::string const & description)
{
  CONVERT_POINTER;

  return pImpl->SetParameterPointer(extent, ptr, name, description);
}

int ModelDriverCreate::SetParameterPointer(int const extent,
                                           double * const ptr,
                                           std::string const & name,
                                           std::string const & description)
{
  CONVERT_POINTER;

  return pImpl->SetParameterPointer(extent, ptr, name, description);
}

void ModelDriverCreate::SetModelBufferPointer(void * const ptr)
{
  CONVERT_POINTER;

  pImpl->SetModelBufferPointer(ptr);
}


int ModelDriverCreate::SetUnits(LengthUnit const lengthUnit,
                                EnergyUnit const energyUnit,
                                ChargeUnit const chargeUnit,
                                TemperatureUnit const temperatureUnit,
                                TimeUnit const timeUnit)
{
  CONVERT_POINTER;

  return pImpl->SetUnits(
      lengthUnit, energyUnit, chargeUnit, temperatureUnit, timeUnit);
}


int ModelDriverCreate::ConvertUnit(LengthUnit const fromLengthUnit,
                                   EnergyUnit const fromEnergyUnit,
                                   ChargeUnit const fromChargeUnit,
                                   TemperatureUnit const fromTemperatureUnit,
                                   TimeUnit const fromTimeUnit,
                                   LengthUnit const toLengthUnit,
                                   EnergyUnit const toEnergyUnit,
                                   ChargeUnit const toChargeUnit,
                                   TemperatureUnit const toTemperatureUnit,
                                   TimeUnit const toTimeUnit,
                                   double const lengthExponent,
                                   double const energyExponent,
                                   double const chargeExponent,
                                   double const temperatureExponent,
                                   double const timeExponent,
                                   double * const conversionFactor)
{
  return ModelImplementation::ConvertUnit(fromLengthUnit,
                                          fromEnergyUnit,
                                          fromChargeUnit,
                                          fromTemperatureUnit,
                                          fromTimeUnit,
                                          toLengthUnit,
                                          toEnergyUnit,
                                          toChargeUnit,
                                          toTemperatureUnit,
                                          toTimeUnit,
                                          lengthExponent,
                                          energyExponent,
                                          chargeExponent,
                                          temperatureExponent,
                                          timeExponent,
                                          conversionFactor);
}

void ModelDriverCreate::LogEntry(LogVerbosity const logVerbosity,
                                 std::string const & message,
                                 int const lineNumber,
                                 std::string const & fileName) const
{
  CONVERT_POINTER;

  pImpl->LogEntry(logVerbosity, message, lineNumber, fileName);
}

void ModelDriverCreate::LogEntry(LogVerbosity const logVerbosity,
                                 std::stringstream const & message,
                                 int const lineNumber,
                                 std::string const & fileName) const
{
  CONVERT_POINTER;

  pImpl->LogEntry(logVerbosity, message, lineNumber, fileName);
}

std::string const & ModelDriverCreate::ToString() const
{
  CONVERT_POINTER;

  return pImpl->ToString();
}

ModelDriverCreate::ModelDriverCreate() : pimpl(NULL) {}

ModelDriverCreate::~ModelDriverCreate() {}

}  // namespace KIM
