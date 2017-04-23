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

#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qlockedfileiosession.h"
#include "qproject.h"

#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QFileInfo>

namespace lcv{

QProjectDocument::QProjectDocument(QProjectFile *file, bool isMonitored, QProject *parent)
    : QObject(parent)
    , m_file(file)
    , m_isDirty(false)
    , m_isMonitored(isMonitored)
{
    readContent();
    m_file->setDocument(this);
}

void QProjectDocument::dumpContent(const QString &content){
    m_content = content;
    emit contentChanged();
}

void QProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        m_content = parentAsProject()->lockedFileIO()->readFromFile(m_file->path());
        m_lastModified = QFileInfo(m_file->path()).lastModified();
        emit contentChanged();
    }
}

QProject *QProjectDocument::parentAsProject(){
    return qobject_cast<QProject*>(parent());
}

bool QProjectDocument::save(){
    if ( m_file->path() != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(m_file->path(), m_content ) ){
            setIsDirty(false);
            m_lastModified = QDateTime::currentDateTime();
            if ( parentAsProject() )
                emit parentAsProject()->fileChanged(m_file->path());
            return true;
        }
    }
    return false;
}

bool QProjectDocument::saveAs(const QString &path){
    if ( path != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(path, m_content ) ){
            //TODO: parentAsProject()->relocateDocumentInFileSystem()
            setIsDirty(false);
            m_lastModified = QDateTime::currentDateTime();
            return true;
        }
    }
    return false;
}

bool QProjectDocument::saveAs(const QUrl &url){
    return saveAs(url.toLocalFile());
}

QProjectDocument::~QProjectDocument(){
    if ( m_file->parent() == 0 )
        m_file->deleteLater();
    else {
        m_file->setDocument(0);
    }
}

}// namespace
