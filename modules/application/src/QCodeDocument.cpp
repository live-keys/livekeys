/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "QCodeDocument.hpp"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QDebug>
#include <QQmlEngine>

QCodeDocument::QCodeDocument(QQuickItem *parent) :
    QQuickItem(parent),
    m_engine(0)
{
}

QString QCodeDocument::openFile(const QUrl &file){
    QString fileName = file.toLocalFile();
    if ( fileName != "" ){
        QFile file(fileName);
        if ( !file.open(QIODevice::ReadOnly ) ){
            return QString("Could not open file : ") + fileName;
        }
        QTextStream in(&file);
        m_openedFile = fileName;
        setPath(QFileInfo(file).path());
        if ( m_engine ){
//            qDebug() << "Import paths :\ns " << m_engine->importPathList();
//            QStringList importPathsTemp = m_importPaths;
//            importPathsTemp.append(m_path);
//            m_engine->addImportPath(m_path);
//            m_engine->setImportPathList(importPathsTemp);
        }
        QString content = in.readAll();
        file.close();
        return content;
    }
    return "";
}

void QCodeDocument::saveFile(const QUrl& file, const QString &content){
    QString fileName = file.toLocalFile();
    if ( fileName != "" ){
        QFile file(fileName);
        if ( !file.open(QIODevice::WriteOnly ) ){
            qDebug() << "Can't open file for writing";
            return;
        } else {
            QTextStream stream(&file);
            stream << content;
            stream.flush();
            file.close();
            setPath(QFileInfo(file).path());
//            if ( m_engine ){
//                QStringList importPathsTemp = m_importPaths;
//                importPathsTemp.append(m_path);
//                m_engine->setImportPathList(importPathsTemp);
//            }
        }
    }
}

void QCodeDocument::setEngine(QQmlEngine *engine){
    m_engine      = engine;
    m_engine->addImportPath(QDir::currentPath());
    m_importPaths = engine->importPathList();
}
