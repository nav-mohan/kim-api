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


#ifndef KIM_COLLECTION_ITEM_TYPE_HPP_
#define KIM_COLLECTION_ITEM_TYPE_HPP_

#include <string>

namespace KIM
{
/// \brief An \ref extensible_enumeration "Extensible Enumeration" for the
/// CollectionItemType's supported by the %KIM API.
///
/// The enumeration constants are contained in the COLLECTION_ITEM_TYPE
/// namespace.
///
/// \sa KIM_CollectionItemType,
/// kim_collection_item_type_module::kim_collection_item_type_type
///
/// \since 2.1
class CollectionItemType
{
 public:
  /// \brief Integer identifying the specific CollectionItemType represented.
  ///
  /// \note This should not be directly accessed and is only public for
  /// cross-language reasons.
  ///
  /// \sa KIM_CollectionItemType::collectionItemTypeID,
  /// kim_collection_item_type_module::kim_collection_item_type_type::collection_item_type_id
  ///
  /// \since 2.1
  int collectionItemTypeID;

  /// \brief Create an uninitialized CollectionItemType object.
  ///
  /// \since 2.1
  CollectionItemType();

  /// \brief Create a CollectionItemType object with the specified id.
  ///
  /// \note This should not be used directly.
  ///
  /// \since 2.1
  CollectionItemType(int const id);

  /// \brief Create a CollectionItemType object corresponding to the provided
  /// string.  If the string does not match one of the values defined by the
  /// %KIM API, then an "unknown" object is generated.
  ///
  /// \sa KIM_CollectionItemType_FromString,
  /// kim_collection_item_type_module::kim_from_string
  ///
  /// \since 2.1
  CollectionItemType(std::string const & str);

  /// \brief Determines if the object is a quantity known to the %KIM API.
  ///
  /// CollectionItemType's known to the %KIM API are found in the
  /// COLLECTION_TIME_TYPE namespace.
  ///
  /// \sa KIM_CollectionItemType_Known,
  /// kim_collection_item_type_module::kim_known
  ///
  /// \since 2.1
  bool Known() const;

  /// \brief Compares CollectionItemType objects for equality.
  ///
  /// \note Not all "unknown" objects are equal.
  ///
  /// \sa KIM_CollectionItemType_Equal,
  /// kim_collection_item_type_module::operator(.eq.)
  ///
  /// \since 2.1
  bool operator==(CollectionItemType const & rhs) const;

  /// \brief Compares CollectionItemType objects for inequality.
  ///
  /// \note It is possible for two "unknown" objects to be not equal.
  ///
  /// \sa KIM_CollectionItemType_NotEqual,
  /// kim_collection_item_type_module::operator(.ne.)
  ///
  /// \since 2.1
  bool operator!=(CollectionItemType const & rhs) const;

  /// \brief Converts the object to a string.
  ///
  /// \return A string object representing the CollectionItemType object.
  ///
  /// \note If the CollectionItemType object does not correspond to a value
  /// defined by the %KIM API, then the string "unknown" is returned.
  ///
  /// \sa KIM_CollectionItemType_ToString,
  /// kim_collection_item_type_module::kim_to_string
  ///
  /// \since 2.1
  std::string const & ToString() const;
};  // class CollectionItemType

/// \brief Contains the enumeration constants and the discovery routines for
/// the CollectionItemType \ref extensible_enumeration "Extensible Enumeration".
namespace COLLECTION_ITEM_TYPE
{
/// \brief The standard \c modelDriver CollectionItemType.
///
/// The item is a model driver.
///
/// \sa KIM_COLLECTION_ITEM_TYPE_modelDriver,
/// kim_collection_item_type_module::kim_collection_item_type_model_driver
///
/// \since 2.1
extern CollectionItemType const modelDriver;

/// \brief The standard \c portableModel CollectionItemType.
///
/// The item is a portable model.
///
/// \sa KIM_COLLECTION_ITEM_TYPE_portableModel,
/// kim_collection_item_type_module::kim_collection_item_type_portable_model
///
/// \since 2.1
extern CollectionItemType const portableModel;

/// \brief The standard \c simulatorModel CollectionItemType.
///
/// The item is a simulator model.
///
/// \sa KIM_COLLECTION_ITEM_TYPE_simulatorModel,
/// kim_collection_item_type_module::kim_collection_item_type_simulator_model
///
/// \since 2.1
extern CollectionItemType const simulatorModel;


/// \brief Get the number of standard CollectionItemType's defined by the %KIM
/// API.
///
/// \param[out] numberOfCollectionItemTypes The number of standard
///             CollectionItemType's defined by the %KIM API.
///
/// \sa KIM_COLLECTION_ITEM_TYPE_GetNumberOfCollectionItemTypes,
/// kim_collection_item_type_module::kim_get_number_of_collection_item_types
///
/// \since 2.1
void GetNumberOfCollectionItemTypes(int * const numberOfCollectionItemTypes);

/// \brief Get the identity of each defined standard CollectionItemType.
///
/// \param[in]  index Zero-based index uniquely labeling each defined standard
///             CollectionItemType.  This index ordering is only guaranteed to
///             be stable during the lifetime of the current process.
/// \param[out] collectionItemType The CollectionItemType object associated with
///             \c index.
///
/// \return \c true if `index < 0` or `index >= numberOfCollectionItemTypes`.
/// \return \c false otherwise.
///
/// \sa KIM_COLLECTION_ITEM_TYPE_GetCollectionItemType,
/// kim_collection_item_type_module::kim_get_collection_item_type
///
/// \since 2.1
int GetCollectionItemType(int const index,
                          CollectionItemType * const collectionItemType);

/// \brief Structure provided for use with std::map.
///
/// \since 2.1
struct Comparator
{
  /// \brief Provides an (logically unmeaningful) ordering for
  /// CollectionItemType objects so that they can be stored in a std::map.
  ///
  /// \since 2.1
  bool operator()(CollectionItemType const & a,
                  CollectionItemType const & b) const
  {
    return a.collectionItemTypeID < b.collectionItemTypeID;
  }
};  // struct Comparator
}  // namespace COLLECTION_ITEM_TYPE
}  // namespace KIM

#endif  // KIM_COLLECTION_ITEM_TYPE_HPP_
