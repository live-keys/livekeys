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


#include "live/libraryloadpath.h"
#include "live/visuallog.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>

namespace lv{

namespace{

bool assertLinkPathExists(const QString& linkPath){
    if ( !QDir(linkPath).exists() )
        return QDir().mkdir(linkPath);
    return true;
}

}// namespace

void LibraryLoadPath::addImpl(const QString& path, const QString& linkPath, bool recursive){
    if ( !assertLinkPathExists(linkPath) ){
        qCritical("Failed to create link directory. Some library dependencies may fail to load.");
        return;
    }

    QDirIterator dit(path);
    while ( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." )
            continue;

        if (  (info.isFile() || info.isSymLink()) &&
               info.fileName().startsWith("lib") &&
               ( info.fileName().contains(".so") || info.fileName().contains(".dylib") )
              )
        {
            QFile f(dit.filePath());
            f.link(linkPath + "/" + info.fileName());

            vlog_debug("libraryloadpath", "Added \'" + linkPath + "/" + info.fileName() + "\' -> \'" + f.fileName() + "\'");

        } else if ( info.isDir() && recursive ){
            addImpl(info.filePath(), linkPath, recursive);
        }
    }
}

}// namespace
