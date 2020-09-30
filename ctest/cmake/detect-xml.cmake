#[[
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0.  If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright 1997 - July 2008 CWI, August 2008 - 2020 MonetDB B.V.
#]]

if (${LINUX_DISTRO} STREQUAL "debian")
  if(${LINUX_DISTRO_VERSION} STREQUAL "9")
    assert_package_detected(
      detect TRUE
      legacyvariable HAVE_LIBXML
      variablename LibXml2_FOUND)
  endif()
  if(${LINUX_DISTRO_VERSION} STREQUAL "10")
    assert_package_detected(
      detect TRUE
      legacyvariable HAVE_LIBXML
      variablename LibXml2_FOUND)
  endif()
elseif (${LINUX_DISTRO} STREQUAL "ubuntu")
  if(${LINUX_DISTRO_VERSION} VERSION_GREATER_EQUAL "18")
    assert_package_detected(
      detect FALSE
      legacyvariable HAVE_LIBXML
      variablename LibXml2_FOUND)
  endif()
elseif(${LINUX_DISTRO} STREQUAL "fedora")
  if(${LINUX_DISTRO_VERSION} STREQUAL "30")
    assert_package_detected(
      detect TRUE
      legacyvariable HAVE_LIBXML
      variablename LibXml2_FOUND)
  endif()
  if(${LINUX_DISTRO_VERSION} STREQUAL "31")
    assert_package_detected(
      detect TRUE
      legacyvariable HAVE_LIBXML
      variablename LibXml2_FOUND)
  endif()
  if(${LINUX_DISTRO_VERSION} STREQUAL "32")
    set(DETECT "0")
    set(UNDETECT "1")
    assert_package_detected(
      detect TRUE
      legacyvariable HAVE_LIBXML
      variablename LibXml2_FOUND)
  endif()
else()
  message(ERROR "Linux distro: ${LINUX_DISTRO} not known")
  message(ERROR "Linux distro version: ${LINUX_DISTRO_VERSION} not known")
endif()
