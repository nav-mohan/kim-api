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


#ifndef KIM_SUPPORT_STATUS_HPP_
#define KIM_SUPPORT_STATUS_HPP_

#include <string>

namespace KIM
{
/// \brief An \ref extensible_enumeration "Extensible Enumeration" for the
/// SupportStatus's supported by the %KIM API.
///
/// The enumeration constants are contained in the SUPPORT_STATUS namespace.
///
/// \sa KIM_SupportStatus, kim_support_status_module::kim_support_status_type
///
/// \since 2.0
class SupportStatus
{
 public:
  /// \brief Integer identifying the specific SupportStatus represented.
  ///
  /// \note This should not be directly accessed and is only public for
  /// cross-language reasons.
  ///
  /// \sa KIM_SupportStatus::supportStatusID,
  /// kim_support_status_module::kim_support_status_type::support_status_id
  ///
  /// \since 2.0
  int supportStatusID;

  /// \brief Create an uninitialized SupportStatus object.
  ///
  /// \since 2.0
  SupportStatus();

  /// \brief Create a SupportStatus object with the specified id.
  ///
  /// \note This should not be used directly.
  ///
  /// \since 2.0
  SupportStatus(int const id);

  /// \brief Create a SupportStatus object corresponding to the provided
  /// string.  If the string does not match one of the values defined by the
  /// %KIM API, then an "unknown" object is generated.
  ///
  /// \sa KIM_SupportStatus_FromString,
  /// kim_support_status_module::kim_from_string
  ///
  /// \since 2.0
  SupportStatus(std::string const & str);

  /// \brief Determines if the object is a quantity known to the %KIM API.
  ///
  /// SupportStatus's known to the %KIM API are found in the SUPPORT_STATUS
  /// namespace.
  ///
  /// \sa KIM_SupportStatus_Known, kim_support_status_module::kim_known
  ///
  /// \since 2.0
  bool Known() const;

  /// \brief Compares SupportStatus objects for equality.
  ///
  /// \note Not all "unknown" objects are equal.
  ///
  /// \sa KIM_SupportStatus_Equal, kim_support_status_module::operator(.eq.)
  ///
  /// \since 2.0
  bool operator==(SupportStatus const & rhs) const;

  /// \brief Compares SupportStatus objects for inequality.
  ///
  /// \note It is possible for two "unknown" objects to be not equal.
  ///
  /// \sa KIM_SupportStatus_NotEqual, kim_support_status_module::operator(.ne.)
  ///
  /// \since 2.0
  bool operator!=(SupportStatus const & rhs) const;

  /// \brief Converts the object to a string.
  ///
  /// \return A string object representing the SupportStatus object.
  ///
  /// \note If the SupportStatus object does not correspond to a value defined
  /// by the %KIM API, then the string "unknown" is returned.
  ///
  /// \sa KIM_SupportStatus_ToString, kim_support_status_module::kim_to_string
  ///
  /// \since 2.0
  std::string const & ToString() const;
};  // class SupportStatus

/// \brief Contains the enumeration constants and the discovery routines for
/// the SupportStatus \ref extensible_enumeration "Extensible Enumeration".
namespace SUPPORT_STATUS
{
/// \brief The standard \c requiredByAPI status.
///
/// \todo Add more detailed description of status.
///
/// \sa KIM_SUPPORT_STATUS_requiredByAPI,
/// kim_support_status_module::kim_support_status_required_by_api
///
/// \since 2.0
extern SupportStatus const requiredByAPI;

/// \brief The standard \c notSupported status.
///
/// \todo Add more detailed description of status.
///
/// \sa KIM_SUPPORT_STATUS_notSupported,
/// kim_support_status_module::kim_support_status_not_supported
///
/// \since 2.0
extern SupportStatus const notSupported;

/// \brief The standard \c required status.
///
/// \todo Add more detailed description of status.
///
/// \sa KIM_SUPPORT_STATUS_required,
/// kim_support_status_module::kim_support_status_required
///
/// \since 2.0
extern SupportStatus const required;

/// \brief The standard \c optional status.
///
/// \todo Add more detailed description of status.
///
/// \sa KIM_SUPPORT_STATUS_optional,
/// kim_support_status_module::kim_support_status_optional
///
/// \since 2.0
extern SupportStatus const optional;


/// \brief Get the number of standard SupportStatus's defined by the %KIM
/// API.
///
/// \param[out] numberOfSupportStatuses The number of standard SupportStatus's
///             defined by the %KIM API.
///
/// \sa KIM_SUPPORT_STATUS_GetNumberOfSupportStatuses,
/// kim_support_status_module::kim_get_number_of_support_statuses
///
/// \since 2.0
void GetNumberOfSupportStatuses(int * const numberOfSupportStatuses);

/// \brief Get the identity of each defined standard SupportStatus.
///
/// \param[in]  index Zero-based index uniquely labeling each defined standard
///             SupportStatus.  This index ordering is only guaranteed to be
///             stable during the lifetime of the current process.
/// \param[out] supportStatus The SupportStatus object associated with \c index.
///
/// \return \c true if `index < 0` or `index >= numberOfSupportStatuses`.
/// \return \c false otherwise.
///
/// \sa KIM_SUPPORT_STATUS_GetSupportStatus,
/// kim_support_status_module::kim_get_support_status
///
/// \since 2.0
int GetSupportStatus(int const index, SupportStatus * const supportStatus);

/// \brief Structure provided for use with std::map.
///
/// \since 2.0
struct Comparator
{
  /// \brief Provides an (logically unmeaningful) ordering for SupportStatus
  /// objects so that they can be stored in a std::map.
  ///
  /// \since 2.0
  bool operator()(SupportStatus const & a, SupportStatus const & b) const
  {
    return a.supportStatusID < b.supportStatusID;
  }
};  // struct Comparator
}  // namespace SUPPORT_STATUS
}  // namespace KIM

#endif  // KIM_SUPPORT_STATUS_HPP_
