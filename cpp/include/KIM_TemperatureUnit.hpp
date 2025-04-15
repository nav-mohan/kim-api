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


#ifndef KIM_TEMPERATURE_UNIT_HPP_
#define KIM_TEMPERATURE_UNIT_HPP_

#include <string>

namespace KIM
{
/// \brief An \ref extensible_enumeration "Extensible Enumeration" for the
/// TemperatureUnit's supported by the %KIM API.
///
/// The enumeration constants are contained in the TEMPERATURE_UNIT namespace.
///
/// \sa KIM_TemperatureUnit,
/// kim_temperature_unit_module::kim_temperature_unit_type
///
/// \since 2.0
class TemperatureUnit
{
 public:
  /// \brief Integer identifying the specific TemperatureUnit represented.
  ///
  /// \note This should not be directly accessed and is only public for
  /// cross-language reasons.
  ///
  /// \sa KIM_TemperatureUnit::temperatureUnitID,
  /// kim_temperature_unit_module::kim_temperature_unit_type::<!--
  /// -->temperature_unit_id
  ///
  /// \since 2.0
  int temperatureUnitID;

  /// \brief Create an uninitialized TemperatureUnit object.
  ///
  /// \since 2.0
  TemperatureUnit();

  /// \brief Create a TemperatureUnit object with the specified id.
  ///
  /// \note This should not be used directly.
  ///
  /// \since 2.0
  TemperatureUnit(int const id);

  /// \brief Create a TemperatureUnit object corresponding to the provided
  /// string.  If the string does not match one of the values defined by the
  /// %KIM API, then an "unknown" object is generated.
  ///
  /// \sa KIM_TemperatureUnit_FromString,
  /// kim_temperature_unit_module::kim_from_string
  ///
  /// \since 2.0
  TemperatureUnit(std::string const & str);

  /// \brief Determines if the object is a quantity known to the %KIM API.
  ///
  /// TemperatureUnit's known to the %KIM API are found in the TEMPERATURE_UNIT
  /// namespace.
  ///
  /// \sa KIM_TemperatureUnit_Known, kim_temperature_unit_module::kim_known
  ///
  /// \since 2.0
  bool Known() const;

  /// \brief Compares TemperatureUnit objects for equality.
  ///
  /// \note Not all "unknown" objects are equal.
  ///
  /// \sa KIM_TemperatureUnit_Equal,
  /// kim_temperature_unit_module::operator(.eq.)
  ///
  /// \since 2.0
  bool operator==(TemperatureUnit const & rhs) const;

  /// \brief Compares TemperatureUnit objects for inequality.
  ///
  /// \note It is possible for two "unknown" objects to be not equal.
  ///
  /// \sa KIM_TemperatureUnit_NotEqual,
  /// kim_temperature_unit_module::operator(.ne.)
  ///
  /// \since 2.0
  bool operator!=(TemperatureUnit const & rhs) const;

  /// \brief Converts the object to a string.
  ///
  /// \return A string object representing the TemperatureUnit object.
  ///
  /// \note If the TemperatureUnit object does not correspond to a value
  /// defined by the %KIM API, then the string "unknown" is returned.
  ///
  /// \sa KIM_TemperatureUnit_ToString,
  /// kim_temperature_unit_module::kim_to_string
  ///
  /// \since 2.0
  std::string const & ToString() const;
};  // class TemperatureUnit

/// \brief Contains the enumeration constants and the discovery routines for
/// the TemperatureUnit \ref extensible_enumeration "Extensible Enumeration".
namespace TEMPERATURE_UNIT
{
/// \brief Indicates that a TemperatureUnit is not used.
///
/// \sa KIM_TEMPERATURE_UNIT_unused,
/// kim_temperature_unit_module::kim_temperature_unit_unused
///
/// \since 2.0
extern TemperatureUnit const unused;

/// \brief The standard Kelvin unit of temperature.
///
/// \sa KIM_TEMPERATURE_UNIT_K,
/// kim_temperature_unit_module::kim_temperature_unit_k
///
/// \since 2.0
extern TemperatureUnit const K;


/// \brief Get the number of standard TemperatureUnit's defined by the %KIM
/// API.
///
/// \param[out] numberOfTemperatureUnits The number of standard
///             TemperatureUnit's defined by the %KIM API.
///
/// \sa KIM_TEMPERATURE_UNIT_GetNumberOfTemperatureUnits,
/// kim_temperature_unit_module::kim_get_number_of_temperature_units
///
/// \since 2.0
void GetNumberOfTemperatureUnits(int * const numberOfTemperatureUnits);

/// \brief Get the identity of each defined standard TemperatureUnit.
///
/// \param[in]  index Zero-based index uniquely labeling each defined standard
///             TemperatureUnit.  This index ordering is only guaranteed to be
///             stable during the lifetime of the current process.
/// \param[out] temperatureUnit The TemperatureUnit object associated with \c
///             index.
///
/// \return \c true if `index < 0` or `index >= numberOfTemperatureUnits`.
/// \return \c false otherwise.
///
/// \sa KIM_TEMPERATURE_UNIT_GetTemperatureUnit,
/// kim_temperature_unit_module::kim_get_temperature_unit
///
/// \since 2.0
int GetTemperatureUnit(int const index,
                       TemperatureUnit * const temperatureUnit);

/// \brief Structure provided for use with std::map.
///
/// \since 2.0
struct Comparator
{
  /// \brief Provides an (logically unmeaningful) ordering for TemperatureUnit
  /// objects so that they can be stored in a std::map.
  ///
  /// \since 2.0
  bool operator()(TemperatureUnit const & a, TemperatureUnit const & b) const
  {
    return a.temperatureUnitID < b.temperatureUnitID;
  }
};  // struct Comparator
}  // namespace TEMPERATURE_UNIT
}  // namespace KIM

#endif  // KIM_TEMPERATURE_UNIT_HPP_
