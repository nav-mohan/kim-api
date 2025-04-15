/*                                                                            */
/* KIM-API: An API for interatomic models                                     */
/* Copyright (c) 2013--2022, Regents of the University of Minnesota.          */
/* All rights reserved.                                                       */
/*                                                                            */
/* Contributors:                                                              */
/*    Ryan S. Elliott                                                         */
/*                                                                            */
/* SPDX-License-Identifier: LGPL-2.1-or-later                                 */
/*                                                                            */
/* This library is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU Lesser General Public                 */
/* License as published by the Free Software Foundation; either               */
/* version 2.1 of the License, or (at your option) any later version.         */
/*                                                                            */
/* This library is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of             */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          */
/* Lesser General Public License for more details.                            */
/*                                                                            */
/* You should have received a copy of the GNU Lesser General Public License   */
/* along with this library; if not, write to the Free Software Foundation,    */
/* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA         */
/*                                                                            */

/*                                                                            */
/* Release: This file is part of the kim-api-2.4.1 package.                   */
/*                                                                            */


#ifndef KIM_COMPUTE_ARGUMENTS_H_
#define KIM_COMPUTE_ARGUMENTS_H_

#ifndef KIM_FUNCTION_TYPES_H_
#include "KIM_FunctionTypes.h" /* IWYU pragma: export */
#endif

/* Forward declarations */
#ifndef KIM_LOG_VERBOSITY_DEFINED_
#define KIM_LOG_VERBOSITY_DEFINED_
/**
 ** \brief Forward declaration.
 **
 ** \since 2.0
 **/
typedef struct KIM_LogVerbosity KIM_LogVerbosity;
#endif

#ifndef KIM_LANGUAGE_NAME_DEFINED_
#define KIM_LANGUAGE_NAME_DEFINED_
/**
 ** \brief Forward declaration.
 **
 ** \since 2.0
 **/
typedef struct KIM_LanguageName KIM_LanguageName;
#endif

#ifndef KIM_COMPUTE_ARGUMENT_NAME_DEFINED_
#define KIM_COMPUTE_ARGUMENT_NAME_DEFINED_
/**
 ** \brief Forward declaration.
 **
 ** \since 2.0
 **/
typedef struct KIM_ComputeArgumentName KIM_ComputeArgumentName;
#endif

#ifndef KIM_COMPUTE_CALLBACK_NAME_DEFINED_
#define KIM_COMPUTE_CALLBACK_NAME_DEFINED_
/**
 ** \brief Forward declaration.
 **
 ** \since 2.0
 **/
typedef struct KIM_ComputeCallbackName KIM_ComputeCallbackName;
#endif

#ifndef KIM_SUPPORT_STATUS_DEFINED_
#define KIM_SUPPORT_STATUS_DEFINED_
/**
 ** \brief Forward declaration.
 **
 ** \since 2.0
 **/
typedef struct KIM_SupportStatus KIM_SupportStatus;
#endif


#ifndef KIM_COMPUTE_ARGUMENTS_DEFINED_
#define KIM_COMPUTE_ARGUMENTS_DEFINED_
/**
 ** \brief \copybrief KIM::ComputeArguments
 **
 ** \sa KIM::ComputeArguments,
 ** kim_compute_arguments_module::kim_compute_arguments_handle_type
 **
 ** \since 2.0
 **/
typedef struct KIM_ComputeArguments KIM_ComputeArguments;
#endif

/**
 ** \brief \copybrief KIM::ComputeArguments::GetArgumentSupportStatus
 **
 ** \sa KIM::ComputeArguments::GetArgumentSupportStatus,
 ** kim_compute_arguments_module::kim_get_argument_support_status
 **
 ** \since 2.0
 **/
int KIM_ComputeArguments_GetArgumentSupportStatus(
    KIM_ComputeArguments const * const computeArguments,
    KIM_ComputeArgumentName const computeArgumentName,
    KIM_SupportStatus * const supportStatus);

/**
 ** \brief \copybrief KIM::ComputeArguments::GetCallbackSupportStatus
 **
 ** \sa KIM::ComputeArguments::GetCallbackSupportStatus,
 ** kim_compute_arguments_module::kim_get_callback_support_status
 **
 ** \since 2.0
 **/
int KIM_ComputeArguments_GetCallbackSupportStatus(
    KIM_ComputeArguments const * const computeArguments,
    KIM_ComputeCallbackName const computeCallbackName,
    KIM_SupportStatus * const supportStatus);

/**
 ** \brief \copybrief KIM::ComputeArguments::SetArgumentPointer
 **
 ** \sa KIM::ComputeArguments::SetArgumentPointer,
 ** kim_compute_arguments_module::kim_set_argument_pointer
 **
 ** \since 2.0
 **/
int KIM_ComputeArguments_SetArgumentPointerInteger(
    KIM_ComputeArguments * const computeArguments,
    KIM_ComputeArgumentName const computeArgumentName,
    int const * const ptr);

/**
 ** \brief \copybrief KIM::ComputeArguments::SetArgumentPointer
 **
 ** \sa KIM::ComputeArguments::SetArgumentPointer,
 ** kim_compute_arguments_module::kim_set_argument_pointer
 **
 ** \since 2.0
 **/
int KIM_ComputeArguments_SetArgumentPointerDouble(
    KIM_ComputeArguments * const computeArguments,
    KIM_ComputeArgumentName const computeArgumentName,
    double const * const ptr);

/**
 ** \brief \copybrief KIM::ComputeArguments::SetCallbackPointer
 **
 ** \sa KIM::ComputeArguments::SetCallbackPointer,
 ** kim_compute_arguments_module::kim_set_callback_pointer
 **
 ** \since 2.0
 **/
int KIM_ComputeArguments_SetCallbackPointer(
    KIM_ComputeArguments * const computeArguments,
    KIM_ComputeCallbackName const computeCallbackName,
    KIM_LanguageName const languageName,
    KIM_Function * const fptr,
    void * const dataObject);

/**
 ** \brief \copybrief <!--
 ** --> KIM::ComputeArguments::AreAllRequiredArgumentsAndCallbacksPresent
 **
 ** \sa KIM::ComputeArguments::AreAllRequiredArgumentsAndCallbacksPresent,
 ** kim_compute_arguments_module::kim_are_all_required_present
 **
 ** \since 2.0
 **/
void KIM_ComputeArguments_AreAllRequiredArgumentsAndCallbacksPresent(
    KIM_ComputeArguments const * const computeArguments, int * const result);

/**
 ** \brief \copybrief KIM::ComputeArguments::SetSimulatorBufferPointer
 **
 ** \sa KIM::ComputeArguments::SetSimulatorBufferPointer,
 ** kim_compute_arguments_module::kim_set_simulator_buffer_pointer
 **
 ** \since 2.0
 **/
void KIM_ComputeArguments_SetSimulatorBufferPointer(
    KIM_ComputeArguments * const computeArguments, void * const ptr);

/**
 ** \brief \copybrief KIM::ComputeArguments::GetSimulatorBufferPointer
 **
 ** \sa KIM::ComputeArguments::GetSimulatorBufferPointer,
 ** kim_compute_arguments_module::kim_get_simulator_buffer_pointer
 **
 ** \since 2.0
 **/
void KIM_ComputeArguments_GetSimulatorBufferPointer(
    KIM_ComputeArguments const * const computeArguments, void ** const ptr);

/**
 ** \brief \copybrief KIM::ComputeArguments::ToString
 **
 ** \sa KIM::ComputeArguments::ToString,
 ** kim_compute_arguments_module::kim_to_string
 **
 ** \since 2.0
 **/
char const * KIM_ComputeArguments_ToString(
    KIM_ComputeArguments const * const computeArguments);

/**
 ** \brief \copybrief KIM::ComputeArguments::SetLogID
 **
 ** \sa KIM::ComputeArguments::SetLogID,
 ** kim_compute_arguments_module::kim_set_log_id
 **
 ** \since 2.0
 **/
void KIM_ComputeArguments_SetLogID(
    KIM_ComputeArguments * const computeArguments, char const * const logID);

/**
 ** \brief \copybrief KIM::ComputeArguments::PushLogVerbosity
 **
 ** \sa KIM::ComputeArguments::PushLogVerbosity,
 ** kim_compute_arguments_module::kim_push_log_verbosity
 **
 ** \since 2.0
 **/
void KIM_ComputeArguments_PushLogVerbosity(
    KIM_ComputeArguments * const computeArguments,
    KIM_LogVerbosity const logVerbosity);

/**
 ** \brief \copybrief KIM::ComputeArguments::PopLogVerbosity
 **
 ** \sa KIM::ComputeArguments::PopLogVerbosity,
 ** kim_compute_arguments_module::kim_pop_log_verbosity
 **
 ** \since 2.0
 **/
void KIM_ComputeArguments_PopLogVerbosity(
    KIM_ComputeArguments * const computeArguments);

#endif /* KIM_COMPUTE_ARGUMENTS_H_ */
