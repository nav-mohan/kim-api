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


#ifndef KIM_MODEL_CREATE_HPP_
#define KIM_MODEL_CREATE_HPP_

#include <sstream>
#include <string>

#ifndef KIM_FUNCTION_TYPES_HPP_
#include "KIM_FunctionTypes.hpp"  // IWYU pragma: export
#endif

namespace KIM
{
// Forward declarations
class LogVerbosity;
class LanguageName;
class Numbering;
class ModelRoutineName;
class SpeciesName;
class LengthUnit;
class EnergyUnit;
class ChargeUnit;
class TemperatureUnit;
class TimeUnit;
class ModelCreateImplementation;


/// \brief Provides the interface to a %KIM API Model object for use by models
/// within their MODEL_ROUTINE_NAME::Create routine.
///
/// \sa ModelDriverCreate, KIM_ModelCreate, KIM_ModelDriverCreate,
/// kim_model_driver_create_module::kim_model_driver_create_handle_type
///
/// \since 2.0
class ModelCreate
{
 public:
  /// \brief Set the Model's particle Numbering.
  ///
  /// \param[in] numbering The Model's particle Numbering.
  ///
  /// \return \c true if \c numbering is unknown.
  /// \return \c false otherwise.
  ///
  /// \sa ModelDriverCreate::SetModelNumbering,
  /// KIM_ModelCreate_SetModelNumbering,
  /// KIM_ModelDriverCreate_SetModelNumbering,
  /// kim_model_driver_create_module::kim_set_model_numbering
  ///
  /// \since 2.0
  int SetModelNumbering(Numbering const numbering);

  /// \brief Set the Model's influence distance data pointer.
  ///
  /// \todo Add more detailed description of \c influenceDistance (or link to
  /// docs elsewhere?)
  ///
  /// \param[in] influenceDistance Pointer to Model's influence distance.
  ///
  /// \note The model is responsible for allocating the memory associated with
  /// the influence distance data.  The model must use the \ref
  /// cache_buffer_pointers "Model's buffer pointer" to retain access to this
  /// memory location and avoid a memory leak.
  ///
  /// \sa ModelDriverCreate::SetInfluenceDistancePointer,
  /// KIM_ModelCreate_SetInfluenceDistancePointer,
  /// KIM_ModelDriverCreate_SetInfluenceDistancePointer,
  /// kim_model_driver_create_module::kim_set_influence_distance_pointer
  ///
  /// \since 2.0
  void SetInfluenceDistancePointer(double const * const influenceDistance);

  /// \brief Set the Model's neighbor list data pointers.
  ///
  /// \todo Add more detailed description
  ///
  /// \param[in] numberOfNeighborLists The number of neighbor lists required by
  ///            the Model.
  /// \param[in] cutoffs Array of cutoff values for each of the required
  ///            neighbor lists.
  /// \param[in] modelWillNotRequestNeighborsOfNoncontributingParticles Array
  ///            of integers; \c true or \c false for each neighbor list
  ///            required by the Model.
  ///
  /// \note The model is responsible for allocating the memory associated with
  /// the neighbor list cutoffs and \c
  /// modelWillNotRequestNeighborsOfNoncontributingParticles data.  The model
  /// must use the \ref cache_buffer_pointers "Model's buffer pointer" to
  /// retain access to this memory location and avoid a memory leak.
  ///
  /// \sa ModelDriverCreate::SetNeighborListPointers,
  /// KIM_ModelCreate_SetNeighborListPointers,
  /// KIM_ModelDriverCreate_SetNeighborListPointers,
  /// kim_model_driver_create_module::kim_set_neighbor_list_pointers
  ///
  /// \since 2.0
  void SetNeighborListPointers(
      int const numberOfNeighborLists,
      double const * const cutoffs,
      int const * const modelWillNotRequestNeighborsOfNoncontributingParticles);

  /// \brief Set the function pointer for the ModelRoutineName of interest.
  ///
  /// \param[in] modelRoutineName The ModelRoutineName of interest.
  /// \param[in] languageName The LanguageName of the ModelRoutineName.

  /// \param[in] required Integer, \c true if the model requires the simulator
  ///            to call the associated ModelRoutineName in order to be used
  ///            correctly, \c false otherwise.
  /// \param[in] fptr Function pointer for the Model's ModelRoutineName
  ///            routine.
  ///
  /// \return \c true if \c modelRoutineName or \c langaugeName are unknown.
  /// \return \c true if `required == false` and \c modelRoutineName has
  ///         SupportStatus SUPPORT_STATUS::requiredByAPI.
  /// \return \c false otherwise.
  ///
  /// \sa ModelDriverCreate::SetRoutinePointer,
  /// KIM_ModelCreate_SetRoutinePointer,
  /// KIM_ModelDriverCreate_SetRoutinePointer,
  /// kim_model_driver_create_module::kim_set_routine_pointer
  ///
  /// \since 2.0
  int SetRoutinePointer(ModelRoutineName const modelRoutineName,
                        LanguageName const languageName,
                        int const required,
                        Function * const fptr);

  /// \brief Set integer code for supported SpeciesName.
  ///
  /// A call to this routine adds/updates the list of SpeciesName's supported
  /// by the Model and associates the specified SpeciesName with the integer
  /// code to be used within the COMPUTE_ARGUMENT_NAME::particleSpeciesCodes
  /// argument.
  ///
  /// \param[in] speciesName The SpeciesName of interest.
  /// \param[in] code The associated code.
  ///
  /// \return \c true if \c speciesName is unknown.
  /// \return \c false otherwise.
  ///
  /// \sa ModelDriverCreate::SetSpeciesCode, KIM_ModelCreate_SetSpeciesCode,
  /// KIM_ModelDriverCreate_SetSpeciesCode,
  /// kim_model_driver_create_module::kim_set_species_code
  ///
  /// \since 2.0
  int SetSpeciesCode(SpeciesName const speciesName, int const code);

  /// \brief Set the next parameter data pointer to be provided by the model.
  ///
  /// This routine is called once for each parameter array to be provided by
  /// the model.  The order of these calls is important and determines the
  /// index assigned to each parameter array for use in the Model::GetParameter
  /// and related routines.
  ///
  /// \param[in] extent The number of entries in the parameter array.
  /// \param[in] ptr The parameter array data pointer.
  /// \param[in] name A brief unique name for the parameter array.  This name
  ///            must be a valid C identifier.
  /// \param[in] description A free-form text description of the parameter
  ///            array.  This should include details about the data layout
  ///            (e.g., the array corresponds to a square upper-triangular
  ///            matrix in row-major storage).
  ///
  /// \note The model is responsible for allocating the memory associated with
  /// the parameter array data.  The model must use the \ref
  /// cache_buffer_pointers "Model's buffer pointer" to retain access to this
  /// memory location and avoid a memory leak.
  ///
  /// \sa ModelDriverCreate::SetParameterPointer,
  /// KIM_ModelCreate_SetParameterPointerInteger,
  /// KIM_ModelCreate_SetParameterPointerDouble,
  /// KIM_ModelDriverCreate_SetParameterPointerInteger,
  /// KIM_ModelDriverCreate_SetParameterPointerDouble,
  /// kim_model_driver_create_module::kim_set_parameter_pointer
  ///
  /// \since 2.0
  int SetParameterPointer(int const extent,
                          int * const ptr,
                          std::string const & name,
                          std::string const & description);

  /// \overload
  int SetParameterPointer(int const extent,
                          double * const ptr,
                          std::string const & name,
                          std::string const & description);

  /// \brief Set the \ref cache_buffer_pointers "Model's buffer pointer"
  /// within the Model object.
  ///
  /// The model buffer pointer may be used by the Model to associate
  /// a memory buffer with the Model object.
  ///
  /// \param[in] ptr The model buffer data pointer.
  ///
  /// \sa ModelDriverCreate::SetModelBufferPointer,
  /// KIM_ModelCreate_SetModelBufferPointer,
  /// KIM_ModelDriverCreate_SetModelBufferPointer,
  /// kim_model_driver_create_module::kim_set_model_buffer_pointer
  ///
  /// \since 2.0
  void SetModelBufferPointer(void * const ptr);

  /// \brief Set the Model's base unit values.
  ///
  /// \param[in] lengthUnit The Model's base LengthUnit.
  /// \param[in] energyUnit The Model's base EnergyUnit.
  /// \param[in] chargeUnit The Model's base ChargeUnit.
  /// \param[in] temperatureUnit The Model's base TemperatureUnit.
  /// \param[in] timeUnit The Model's base TimeUnit.
  ///
  /// \note A unit of \c unused indicates the the Model does not deal with any
  /// quantities whose derived unit involves the corresponding base unit.  For
  /// example, many models only deal with quantities that are derived from just
  /// the energy and length base units (such as force, virial, etc.), and thus
  /// should set \c chargeUnit, \c temperatureUnit, and \c timeUnit to \c
  /// unused.
  ///
  /// \return \c true if any of the base units are unknown.
  /// \return \c true if \c lengthUnit or \c energyUnit are unused.
  /// \return \c false otherwise.
  ///
  /// \sa ModelDriverCreate::SetUnits, KIM_ModelCreate_SetUnits,
  /// KIM_ModelDriverCreate_SetUnits,
  /// kim_model_driver_create_module::kim_set_units
  ///
  /// \since 2.0
  int SetUnits(LengthUnit const lengthUnit,
               EnergyUnit const energyUnit,
               ChargeUnit const chargeUnit,
               TemperatureUnit const temperatureUnit,
               TimeUnit const timeUnit);

  /// \brief Get the multiplicative factor to convert between a derived unit
  /// represented in two different sets of base units.
  ///
  /// \param[in] fromLengthUnit The "from" base length unit.
  /// \param[in] fromEnergyUnit The "from" base energy unit.
  /// \param[in] fromChargeUnit The "from" base charge unit.
  /// \param[in] fromTemperatureUnit The "from" base temperature unit.
  /// \param[in] fromTimeUnit The "from" base time unit.
  /// \param[in] toLengthUnit The "to" base length unit.
  /// \param[in] toEnergyUnit The "to" base energy unit.
  /// \param[in] toChargeUnit The "to" base charge unit.
  /// \param[in] toTemperatureUnit The "to" base temperature unit.
  /// \param[in] toTimeUnit The "to" base time unit.
  /// \param[in] lengthExponent The derived unit's length exponent.
  /// \param[in] energyExponent The derived unit's energy exponent.
  /// \param[in] chargeExponent The derived unit's charge exponent.
  /// \param[in] temperatureExponent The derived unit's temperature exponent.
  /// \param[in] timeExponent The derived unit's time exponent.
  /// \param[out] conversionFactor The desired conversion factor.
  ///
  /// \return \c true if any of the base units are unknown.
  /// \return \c true if any of the base units are \c unused and the
  ///         corresponding exponent is nonzero.
  /// \return \c false otherwise.
  ///
  /// \sa ModelDriverCreate::ConvertUnit, KIM_ModelCreate_ConvertUnit,
  /// KIM_ModelDriverCreate_ConvertUnit,
  /// kim_model_driver_create_module::kim_convert_unit
  ///
  /// \since 2.0
  static int ConvertUnit(LengthUnit const fromLengthUnit,
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
                         double * const conversionFactor);

  /// \brief Write a log entry into the log file.
  ///
  /// This results in a no-op if \c logVerbosity is LOG_VERBOSITY::silent or if
  /// \c logVerbosity is greater-than the Log object's top LogVerbosity on its
  /// stack.
  ///
  /// \param[in] logVerbosity The LogVerbosity level for the entry.
  /// \param[in] message The body text of the log entry.
  /// \param[in] lineNumber The source code file line number.
  /// \param[in] fileName The source code file name.
  ///
  /// \sa KIM_ModelCreate_LogEntry,
  /// kim_model_driver_create_module::kim_log_entry
  ///
  /// \since 2.0
  void LogEntry(LogVerbosity const logVerbosity,
                std::string const & message,
                int const lineNumber,
                std::string const & fileName) const;

  /// \overload
  void LogEntry(LogVerbosity const logVerbosity,
                std::stringstream const & message,
                int const lineNumber,
                std::string const & fileName) const;

  /// \brief Get a string representing the internal state of the Model object.
  ///
  /// This string is primarily meant for use as a debugging tool.  The string
  /// may be quite long.  It begins and ends with lines consisting only of \c
  /// ='s.
  ///
  /// \sa KIM_ModelCreate_ToString,
  /// kim_model_driver_create_module::kim_to_string
  ///
  /// \since 2.0
  std::string const & ToString() const;

 private:
  // do not allow copy constructor or operator=
  ModelCreate(ModelCreate const &);
  void operator=(ModelCreate const &);

  ModelCreate();
  ~ModelCreate();

  ModelCreateImplementation * pimpl;
};  // class ModelCreate
}  // namespace KIM

#endif  // KIM_MODEL_CREATE_HPP_
