/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVGLOBAL_H
#define LVGLOBAL_H


/**
  \defgroup lvbase lvbase module

  Contains the core components for Livekeys.
*/

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#define LV_EXPORT __declspec(dllexport)
#define LV_IMPORT __declspec(dllimport)
#else
#define LV_EXPORT
#define LV_IMPORT
#endif

#define DISABLE_COPY(Class) \
  Class(const Class&);   \
  void operator=(const Class&)

#define MARK_UNUSED(arg) (void)arg

#if defined(_WIN32)
    #define PLATFORM_OS_WIN
#elif defined(_WIN64)
    #define PLATFORM_OS_WIN
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #define PLATFORM_OS_WIN
#elif defined(__linux__)
    #define PLATFORM_OS_UNIX
    #define PLATFORM_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
    #define PLATFORM_OS_UNIX
    #define PLATFORM_OS_MAC
#endif


#endif //LVGLOBAL_H
