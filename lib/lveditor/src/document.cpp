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

#include "document.h"
#include "live/project.h"
#include "live/viewengine.h"
#include "live/visuallog.h"
#include "live/projectfile.h"
#include "live/lockedfileiosession.h"

#include <QFileInfo>

namespace lv{

// class Document::RunTrigger
// ----------------------------------------------------------------------------

Document::RunTrigger::RunTrigger(){
}

Document::RunTrigger::~RunTrigger(){
}

// class Document
// ----------------------------------------------------------------------------

Document::Document(const QString& filePath, const QString &formatType, bool isMonitored, Project *parent)
    : QObject(parent)
    , m_formatType(formatType)
    , m_isMonitored(isMonitored)
    , m_isDirty(false)
    , m_runTrigger(nullptr)
{
    setPath(filePath);
    readContent();
}


Document::Document(const QString& filePath, Project *parent)
    : QObject(parent)
    , m_isMonitored(false)
    , m_isDirty(false)
    , m_runTrigger(nullptr)
{
    setPath(filePath);
}

void Document::beforeClose(){
    emit aboutToClose();
}

void Document::setPath(const QString &path){
    if ( m_path != path ){
        m_path = path;
        if ( !m_path.isEmpty() ){
            if ( m_path.contains(':') ){
                m_fileName = m_path;
            } else {
                QFileInfo finfo(m_path);
                m_fileName = finfo.fileName();
            }
        }
        emit pathChanged();
    }
}

Document::~Document(){    
}

/**
 * \brief Open the document in a read states
 */
void Document::readContent(){
    if ( isOnDisk() ){
        try{
            m_content = QByteArray::fromStdString(parentAsProject()->viewEngine()->fileIO()->readFromFile(m_path.toStdString()));
            m_lastModified = QFileInfo(m_path).lastModified();
            emit contentChanged();
        } catch ( lv::Exception& e ){
            QmlError(parentAsProject()->viewEngine(), e, this).jsThrow();
        }
    }
}

/**
 * \brief Returns the content legnth of this document
 */
int Document::contentLength(){
    return m_content.length();
}

/**
 * \brief Save modified document to its respective file
 */
bool Document::save(){
    if ( isOnDisk() ){
        try{
            QByteArray data = content();
            if ( parentAsProject()->viewEngine()->fileIO()->writeToFile(
                     path().toStdString(), data.constData(), static_cast<size_t>(data.length() )) )
            {
                setIsDirty(false);
                setLastModified(QDateTime::currentDateTime());
                emit saved();
                if ( parentAsProject() )
                    parentAsProject()->documentSaved(this);
                if ( m_runTrigger )
                    m_runTrigger->onContentSaved();
                return true;
            }
        } catch ( Exception& e ){
            QmlError(parentAsProject()->viewEngine(), e, this).jsThrow();
            return false;
        }
    }
    return false;
}


/**
 * \brief Save document content in a different file
 */
bool Document::saveAs(const QString& savePath){
    if ( path() == savePath ){
        return save();
    } else if ( savePath != "" ){
        try{
            QByteArray data = content();
            if ( parentAsProject()->viewEngine()->fileIO()->writeToFile(
                     savePath.toStdString(), data, static_cast<size_t>(data.size())) )
            {
                parentAsProject()->relocateDocument(path(), savePath, this);
                setIsDirty(false);
                emit saved();
                m_lastModified = QDateTime::currentDateTime();
                parentAsProject()->documentSaved(this);
                return true;
            }
        } catch ( Exception& e ){
            QmlError(parentAsProject()->viewEngine(), e, this).jsThrow();
            return false;
        }
    }
    return false;
}

/**
 * \brief Variant of saveAs using QUrl parameter
 *
 * \sa ProjectDocument::saveAs(const QString &path)
 */
bool Document::saveAs(const QUrl& url){
    return saveAs(url.toLocalFile());
}

void Document::setFormatType(const QString &formatType){
    if ( m_formatType == formatType )
        return;

    m_formatType = formatType;
    emit formatTypeChanged();
}


/**
 * \brief Returns the parent, which is a Project
 */
Project *Document::parentAsProject(){
    return qobject_cast<Project*>(parent());
}

QString Document::fileName() const{
    return m_fileName;
}

bool Document::isOnDisk() const{
    if ( m_fileName.isEmpty() )
        return false;
    if ( m_fileName.contains(':') )
        return false;
    return true;
}

QString Document::pathHash() const{
    if ( isOnDisk() )
        return QString::fromUtf8(Project::hashPath(path().toUtf8()).toHex());
    return "";
}

void Document::notifyRunTriggerChanged(){
    emit runTriggerChanged();
}

void Document::setRunTrigger(Document::RunTrigger *trigger){
    if ( m_runTrigger )
        delete m_runTrigger;
    m_runTrigger = trigger;

}

Document::RunTrigger *Document::runTrigger() const{
    return m_runTrigger;
}

}// namespace
