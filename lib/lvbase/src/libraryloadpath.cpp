/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/libraryloadpath.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>

namespace lv{

/**
 * \class lv::LibraryLoadPath
 *
 * \brief Class that provides an ability to load libraries given a path (both recursively and non-recursively)
 *
 * The implementation in the background is reliant on the platform on which we're running Livekeys.
 * \ingroup lvbase
 */

/**
 * \brief Non-recursive version of loader function, adds only libraries available on given folder level
 */
void LibraryLoadPath::add(const std::string path, const std::string &linkPath){
    addImpl(QString::fromStdString(path), QString::fromStdString(linkPath));
}


/**
 * \brief Recursive version of loader function, adds all of the relevant subfolder contents as well
 */
void LibraryLoadPath::addRecursive(const std::string &path, const std::string &linkPath){
    addImpl(QString::fromStdString(path), QString::fromStdString(linkPath), true);
}

}// namespace
