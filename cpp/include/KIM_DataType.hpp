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


#ifndef KIM_DATA_TYPE_HPP_
#define KIM_DATA_TYPE_HPP_

#include <string>

namespace KIM
{
/// \brief An \ref extensible_enumeration "Extensible Enumeration" for the
/// DataType's supported by the %KIM API.
///
/// The enumeration constants are contained in the DATA_TYPE namespace.
///
/// \sa KIM_DataType, kim_data_type_module::kim_data_type_type
///
/// \since 2.0
class DataType
{
 public:
  /// \brief Integer identifying the specific DataType represented.
  ///
  /// \note This should not be directly accessed and is only public for
  /// cross-language reasons.
  ///
  /// \sa KIM_DataType::dataTypeID,
  /// kim_data_type_module::kim_data_type_type::data_type_id
  ///
  /// \since 2.0
  int dataTypeID;

  /// \brief Create an uninitialized DataType object.
  ///
  /// \since 2.0
  DataType();

  /// \brief Create a DataType object with the specified id.
  ///
  /// \note This should not be used directly.
  ///
  /// \since 2.0
  DataType(int const id);

  /// \brief Create a DataType object corresponding to the provided string.  If
  /// the string does not match one of the values defined by the %KIM API, then
  /// an "unknown" object is generated.
  ///
  /// \sa KIM_DataType_FromString, kim_data_type_module::kim_from_string
  ///
  /// \since 2.0
  DataType(std::string const & str);

  /// \brief Determines if the object is a quantity known to the %KIM API.
  ///
  /// DataType's known to the %KIM API are found in the DATA_TYPE namespace.
  ///
  /// \sa KIM_DataType_Known, kim_data_type_module::kim_known
  ///
  /// \since 2.0
  bool Known() const;

  /// \brief Compares DataType objects for equality.
  ///
  /// \note Not all "unknown" objects are equal.
  ///
  /// \sa KIM_DataType_Equal, kim_data_type_module::operator(.eq.)
  ///
  /// \since 2.0
  bool operator==(DataType const & rhs) const;

  /// \brief Compares DataType objects for inequality.
  ///
  /// \note It is possible for two "unknown" objects to be not equal.
  ///
  /// \sa KIM_DataType_NotEqual, kim_data_type_module::operator(.ne.)
  ///
  /// \since 2.0
  bool operator!=(DataType const & rhs) const;

  /// \brief Converts the object to a string.
  ///
  /// \return A string object representing the DataType object.
  ///
  /// \note If the DataType object does not correspond to a value defined by
  /// the %KIM API, then the string "unknown" is returned.
  ///
  /// \sa KIM_DataType_ToString, kim_data_type_module::kim_to_string()
  ///
  /// \since 2.0
  std::string const & ToString() const;
};  // class DataType

/// \brief Contains the enumeration constants and the discovery routines for
/// the DataType \ref extensible_enumeration "Extensible Enumeration".
namespace DATA_TYPE
{
/// \brief The standard \c Integer data type.
///
/// \todo Add more detailed description of data type.
///
/// \sa KIM_DATA_TYPE_Integer, kim_data_type_module::kim_data_type_integer
///
/// \since 2.0
extern DataType const Integer;

/// \brief The standard \c Double data type.
///
/// \todo Add more detailed description of data type.
///
/// \sa KIM_DATA_TYPE_Double, kim_data_type_module::kim_data_type_double
///
/// \since 2.0
extern DataType const Double;


/// \brief Get the number of standard DataType's defined by the %KIM
/// API.
///
/// \param[out] numberOfDataTypes The number of standard DataType's defined by
///             the %KIM API.
///
/// \sa KIM_DATA_TYPE_GetNumberOfDataTypes,
/// kim_data_type_module::kim_get_number_of_data_types
///
/// \since 2.0
void GetNumberOfDataTypes(int * const numberOfDataTypes);

/// \brief Get the identity of each defined standard DataType.
///
/// \param[in]  index Zero-based index uniquely labeling each defined standard
///             DataType.  This index ordering is only guaranteed to be stable
///             during the lifetime of the current process.
/// \param[out] dataType The DataType object associated with \c index.
///
/// \return \c true if `index < 0` or `index >= numberOfDataTypes`.
/// \return \c false otherwise.
///
/// \sa KIM_DATA_TYPE_GetDataType, kim_data_type_module::kim_get_data_type
///
/// \since 2.0
int GetDataType(int const index, DataType * const dataType);

/// \brief Structure provided for use with std::map.
///
/// \since 2.0
struct Comparator
{
  /// \brief Provides an (logically unmeaningful) ordering for DataType objects
  /// so that they can be stored in a std::map.
  ///
  /// \since 2.0
  bool operator()(DataType const & a, DataType const & b) const
  {
    return a.dataTypeID < b.dataTypeID;
  }
};  // struct Comparator
}  // namespace DATA_TYPE
}  // namespace KIM

#endif  // KIM_DATA_TYPE_HPP_
