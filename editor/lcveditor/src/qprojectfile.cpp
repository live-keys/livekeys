/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qprojectfile.h"
#include <QFileInfo>

namespace lcv{

QProjectFile::QProjectFile(const QString &path, QProjectEntry *parent)
    : QProjectEntry(QFileInfo(path).path(), QFileInfo(path).fileName(), true, parent)
    , m_isOpen(false)
    , m_isDirty(false)
    , m_isMonitored(false)
{
}

QProjectFile::QProjectFile(const QString &path, const QString &name, QProjectEntry *parent)
    : QProjectEntry(path, name, true, parent)
    , m_isOpen(false)
    , m_isDirty(false)
    , m_isMonitored(false)
{
}

QProjectFile::~QProjectFile(){
}

}// namespace
