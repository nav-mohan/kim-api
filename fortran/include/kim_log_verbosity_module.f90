!
! KIM-API: An API for interatomic models
! Copyright (c) 2013--2022, Regents of the University of Minnesota.
! All rights reserved.
!
! Contributors:
!    Ryan S. Elliott
!
! SPDX-License-Identifier: LGPL-2.1-or-later
!
! This library is free software; you can redistribute it and/or
! modify it under the terms of the GNU Lesser General Public
! License as published by the Free Software Foundation; either
! version 2.1 of the License, or (at your option) any later version.
!
! This library is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
! Lesser General Public License for more details.
!
! You should have received a copy of the GNU Lesser General Public License
! along with this library; if not, write to the Free Software Foundation,
! Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
!

!
! Release: This file is part of the kim-api-2.4.1 package.
!

!> \brief \copybrief KIM::LogVerbosity
!!
!! \sa KIM::LogVerbosity, KIM_LogVerbosity
!!
!! \since 2.0
module kim_log_verbosity_module
  use, intrinsic :: iso_c_binding
  implicit none
  private

  public &
    ! Derived types
    kim_log_verbosity_type, &
    ! Constants
    KIM_LOG_VERBOSITY_SILENT, &
    KIM_LOG_VERBOSITY_FATAL, &
    KIM_LOG_VERBOSITY_ERROR, &
    KIM_LOG_VERBOSITY_WARNING, &
    KIM_LOG_VERBOSITY_INFORMATION, &
    KIM_LOG_VERBOSITY_DEBUG, &
    ! Routines
    kim_known, &
    operator(.lt.), &
    operator(.gt.), &
    operator(.le.), &
    operator(.ge.), &
    operator(.eq.), &
    operator(.ne.), &
    kim_from_string, &
    kim_to_string, &
    kim_get_number_of_log_verbosities, &
    kim_get_log_verbosity

  !> \brief \copybrief KIM::LogVerbosity
  !!
  !! \sa KIM::LogVerbosity, KIM_LogVerbosity
  !!
  !! \since 2.0
  type, bind(c) :: kim_log_verbosity_type
    !> \brief \copybrief KIM::LogVerbosity::logVerbosityID
    !!
    !! \sa KIM::LogVerbosity::logVerbosityID, KIM_LogVerbosity::logVerbosityID
    !!
    !! \since 2.0
    integer(c_int) :: log_verbosity_id
  end type kim_log_verbosity_type

  !> \brief \copybrief KIM::LOG_VERBOSITY::silent
  !!
  !! \sa KIM::LOG_VERBOSITY::silent, KIM_LOG_VERBOSITY_silent
  !!
  !! \since 2.0
  type(kim_log_verbosity_type), protected, save, &
    bind(c, name="KIM_LOG_VERBOSITY_silent") &
    :: KIM_LOG_VERBOSITY_SILENT

  !> \brief \copybrief KIM::LOG_VERBOSITY::fatal
  !!
  !! \sa KIM::LOG_VERBOSITY::fatal, KIM_LOG_VERBOSITY_fatal
  !!
  !! \since 2.0
  type(kim_log_verbosity_type), protected, save, &
    bind(c, name="KIM_LOG_VERBOSITY_fatal") &
    :: KIM_LOG_VERBOSITY_FATAL

  !> \brief \copybrief KIM::LOG_VERBOSITY::error
  !!
  !! \sa KIM::LOG_VERBOSITY::error, KIM_LOG_VERBOSITY_error
  !!
  !! \since 2.0
  type(kim_log_verbosity_type), protected, save, &
    bind(c, name="KIM_LOG_VERBOSITY_error") &
    :: KIM_LOG_VERBOSITY_ERROR

  !> \brief \copybrief KIM::LOG_VERBOSITY::warning
  !!
  !! \sa KIM::LOG_VERBOSITY::warning, KIM_LOG_VERBOSITY_warning
  !!
  !! \since 2.0
  type(kim_log_verbosity_type), protected, save, &
    bind(c, name="KIM_LOG_VERBOSITY_warning") &
    :: KIM_LOG_VERBOSITY_WARNING

  !> \brief \copybrief KIM::LOG_VERBOSITY::information
  !!
  !! \sa KIM::LOG_VERBOSITY::information, KIM_LOG_VERBOSITY_information
  !!
  !! \since 2.0
  type(kim_log_verbosity_type), protected, save, &
    bind(c, name="KIM_LOG_VERBOSITY_information") &
    :: KIM_LOG_VERBOSITY_INFORMATION

  !> \brief \copybrief KIM::LOG_VERBOSITY::debug
  !!
  !! \sa KIM::LOG_VERBOSITY::debug, KIM_LOG_VERBOSITY_debug
  !!
  !! \since 2.0
  type(kim_log_verbosity_type), protected, save, &
    bind(c, name="KIM_LOG_VERBOSITY_debug") &
    :: KIM_LOG_VERBOSITY_DEBUG

  !> \brief \copybrief KIM::LogVerbosity::Known
  !!
  !! \sa KIM::LogVerbosity::Known, KIM_LogVerbosity_Known
  !!
  !! \since 2.0
  interface kim_known
    module procedure kim_log_verbosity_known
  end interface kim_known

  !> \brief \copybrief KIM::LogVerbosity::operator<()
  !!
  !! \sa KIM::LogVerbosity::operator<(), KIM_LogVerbosity_LessThan
  !!
  !! \since 2.0
  interface operator(.lt.)
    module procedure kim_log_verbosity_less_than
  end interface operator(.lt.)

  !> \brief \copybrief KIM::LogVerbosity::operator>()
  !!
  !! \sa KIM::LogVerbosity::operator>(), KIM_LogVerbosity_GreaterThan
  !!
  !! \since 2.0
  interface operator(.gt.)
    module procedure kim_log_verbosity_greater_than
  end interface operator(.gt.)

  !> \brief \copybrief KIM::LogVerbosity::operator<=()
  !!
  !! \sa KIM::LogVerbosity::operator<=(), KIM_LogVerbosity_LessThanEqual
  !!
  !! \since 2.0
  interface operator(.le.)
    module procedure kim_log_verbosity_less_than_equal
  end interface operator(.le.)

  !> \brief \copybrief KIM::LogVerbosity::operator>=()
  !!
  !! \sa KIM::LogVerbosity::operator>=(), KIM_LogVerbosity_GreaterThanEqual
  !!
  !! \since 2.0
  interface operator(.ge.)
    module procedure kim_log_verbosity_greater_than_equal
  end interface operator(.ge.)

  !> \brief \copybrief KIM::LogVerbosity::operator==()
  !!
  !! \sa KIM::LogVerbosity::operator==(), KIM_LogVerbosity_Equal
  !!
  !! \since 2.0
  interface operator(.eq.)
    module procedure kim_log_verbosity_equal
  end interface operator(.eq.)

  !> \brief \copybrief KIM::LogVerbosity::operator!=()
  !!
  !! \sa KIM::LogVerbosity::operator!=(), KIM_LogVerbosity_NotEqual
  !!
  !! \since 2.0
  interface operator(.ne.)
    module procedure kim_log_verbosity_not_equal
  end interface operator(.ne.)

  !> \brief \copybrief KIM::LogVerbosity::LogVerbosity(std::string const &)
  !!
  !! \sa KIM::LogVerbosity::LogVerbosity(std::string const &),
  !! KIM_LogVerbosity_FromString
  !!
  !! \since 2.0
  interface kim_from_string
    module procedure kim_log_verbosity_from_string
  end interface kim_from_string

  !> \brief \copybrief KIM::LogVerbosity::ToString
  !!
  !! \sa KIM::LogVerbosity::ToString, KIM_LogVerbosity_ToString
  !!
  !! \since 2.0
  interface kim_to_string
    module procedure kim_log_verbosity_to_string
  end interface kim_to_string

contains
  !> \brief \copybrief KIM::LogVerbosity::Known
  !!
  !! \sa KIM::LogVerbosity::Known, KIM_LogVerbosity_Known
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_known(log_verbosity)
    implicit none
    interface
      integer(c_int) recursive function known(log_verbosity) &
        bind(c, name="KIM_LogVerbosity_Known")
        use, intrinsic :: iso_c_binding
        import kim_log_verbosity_type
        implicit none
        type(kim_log_verbosity_type), intent(in), value :: log_verbosity
      end function known
    end interface
    type(kim_log_verbosity_type), intent(in) :: log_verbosity

    kim_log_verbosity_known = (known(log_verbosity) /= 0)
  end function kim_log_verbosity_known

  !> \brief \copybrief KIM::LogVerbosity::operator<()
  !!
  !! \sa KIM::LogVerbosity::operator<(), KIM_LogVerbosity_LessThan
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_less_than(lhs, rhs)
    implicit none
    type(kim_log_verbosity_type), intent(in) :: lhs
    type(kim_log_verbosity_type), intent(in) :: rhs

    kim_log_verbosity_less_than &
      = (lhs%log_verbosity_id < rhs%log_verbosity_id)
  end function kim_log_verbosity_less_than

  !> \brief \copybrief KIM::LogVerbosity::operator>()
  !!
  !! \sa KIM::LogVerbosity::operator>(), KIM_LogVerbosity_GreaterThan
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_greater_than(lhs, rhs)
    implicit none
    type(kim_log_verbosity_type), intent(in) :: lhs
    type(kim_log_verbosity_type), intent(in) :: rhs

    kim_log_verbosity_greater_than &
      = (lhs%log_verbosity_id >= rhs%log_verbosity_id)
  end function kim_log_verbosity_greater_than

  !> \brief \copybrief KIM::LogVerbosity::operator<=()
  !!
  !! \sa KIM::LogVerbosity::operator<=(), KIM_LogVerbosity_LessThanEqual
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_less_than_equal(lhs, rhs)
    implicit none
    type(kim_log_verbosity_type), intent(in) :: lhs
    type(kim_log_verbosity_type), intent(in) :: rhs

    kim_log_verbosity_less_than_equal &
      = (lhs%log_verbosity_id <= rhs%log_verbosity_id)
  end function kim_log_verbosity_less_than_equal

  !> \brief \copybrief KIM::LogVerbosity::operator>=()
  !!
  !! \sa KIM::LogVerbosity::operator>=(), KIM_LogVerbosity_GreaterThanEqual
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_greater_than_equal(lhs, rhs)
    implicit none
    type(kim_log_verbosity_type), intent(in) :: lhs
    type(kim_log_verbosity_type), intent(in) :: rhs

    kim_log_verbosity_greater_than_equal &
      = (lhs%log_verbosity_id >= rhs%log_verbosity_id)
  end function kim_log_verbosity_greater_than_equal

  !> \brief \copybrief KIM::LogVerbosity::operator==()
  !!
  !! \sa KIM::LogVerbosity::operator==(), KIM_LogVerbosity_Equal
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_equal(lhs, rhs)
    implicit none
    type(kim_log_verbosity_type), intent(in) :: lhs
    type(kim_log_verbosity_type), intent(in) :: rhs

    kim_log_verbosity_equal &
      = (lhs%log_verbosity_id == rhs%log_verbosity_id)
  end function kim_log_verbosity_equal

  !> \brief \copybrief KIM::LogVerbosity::operator!=()
  !!
  !! \sa KIM::LogVerbosity::operator!=(), KIM_LogVerbosity_NotEqual
  !!
  !! \since 2.0
  logical recursive function kim_log_verbosity_not_equal(lhs, rhs)
    implicit none
    type(kim_log_verbosity_type), intent(in) :: lhs
    type(kim_log_verbosity_type), intent(in) :: rhs

    kim_log_verbosity_not_equal = .not. (lhs == rhs)
  end function kim_log_verbosity_not_equal

  !> \brief \copybrief KIM::LogVerbosity::LogVerbosity(std::string const &)
  !!
  !! \sa KIM::LogVerbosity::LogVerbosity(std::string const &),
  !! KIM_LogVerbosity_FromString
  !!
  !! \since 2.0
  recursive subroutine kim_log_verbosity_from_string(string, log_verbosity)
    implicit none
    interface
      type(kim_log_verbosity_type) recursive function from_string(string) &
        bind(c, name="KIM_LogVerbosity_FromString")
        use, intrinsic :: iso_c_binding
        import kim_log_verbosity_type
        implicit none
        character(c_char), intent(in) :: string(*)
      end function from_string
    end interface
    character(len=*, kind=c_char), intent(in) :: string
    type(kim_log_verbosity_type), intent(out) :: log_verbosity

    log_verbosity = from_string(trim(string)//c_null_char)
  end subroutine kim_log_verbosity_from_string

  !> \brief \copybrief KIM::LogVerbosity::ToString
  !!
  !! \sa KIM::LogVerbosity::ToString, KIM_LogVerbosity_ToString
  !!
  !! \since 2.0
  recursive subroutine kim_log_verbosity_to_string(log_verbosity, string)
    use kim_convert_string_module, only: kim_convert_c_char_ptr_to_string
    implicit none
    interface
      type(c_ptr) recursive function get_string(log_verbosity) &
        bind(c, name="KIM_LogVerbosity_ToString")
        use, intrinsic :: iso_c_binding
        import kim_log_verbosity_type
        implicit none
        type(kim_log_verbosity_type), intent(in), value :: log_verbosity
      end function get_string
    end interface
    type(kim_log_verbosity_type), intent(in) :: log_verbosity
    character(len=*, kind=c_char), intent(out) :: string

    type(c_ptr) :: p

    p = get_string(log_verbosity)
    call kim_convert_c_char_ptr_to_string(p, string)
  end subroutine kim_log_verbosity_to_string

  !> \brief \copybrief KIM::LOG_VERBOSITY::GetNumberOfLogVerbosities
  !!
  !! \sa KIM::LOG_VERBOSITY::GetNumberOfLogVerbosities,
  !! KIM_LOG_VERBOSITY_GetNumberOfLogVerbosities
  !!
  !! \since 2.0
  recursive subroutine kim_get_number_of_log_verbosities( &
    number_of_log_verbosities)
    implicit none
    interface
      recursive subroutine get_number_of_log_verbosities( &
        number_of_log_verbosities) &
        bind(c, name="KIM_LOG_VERBOSITY_GetNumberOfLogVerbosities")
        use, intrinsic :: iso_c_binding
        implicit none
        integer(c_int), intent(out) :: number_of_log_verbosities
      end subroutine get_number_of_log_verbosities
    end interface
    integer(c_int), intent(out) :: number_of_log_verbosities

    call get_number_of_log_verbosities(number_of_log_verbosities)
  end subroutine kim_get_number_of_log_verbosities

  !> \brief \copybrief KIM::LOG_VERBOSITY::GetLogVerbosity
  !!
  !! \sa KIM::LOG_VERBOSITY::GetLogVerbosity, KIM_LOG_VERBOSITY_GetLogVerbosity
  !!
  !! \since 2.0
  recursive subroutine kim_get_log_verbosity(index, log_verbosity, ierr)
    implicit none
    interface
      integer(c_int) recursive function get_log_verbosity( &
        index, log_verbosity) bind(c, name="KIM_LOG_VERBOSITY_GetLogVerbosity")
        use, intrinsic :: iso_c_binding
        import kim_log_verbosity_type
        implicit none
        integer(c_int), intent(in), value :: index
        type(kim_log_verbosity_type), intent(out) :: log_verbosity
      end function get_log_verbosity
    end interface
    integer(c_int), intent(in) :: index
    type(kim_log_verbosity_type), intent(out) :: log_verbosity
    integer(c_int), intent(out) :: ierr

    ierr = get_log_verbosity(index - 1, log_verbosity)
  end subroutine kim_get_log_verbosity
end module kim_log_verbosity_module
