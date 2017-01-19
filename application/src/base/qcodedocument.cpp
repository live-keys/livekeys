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

#include "qcodedocument.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QDebug>
#include <QQmlEngine>

QCodeDocument::QCodeDocument(QQuickItem *parent)
    : QQuickItem(parent)
{
}

QString QCodeDocument::openFile(const QUrl &file){
    QString fileName = file.toLocalFile();
    if ( fileName != "" ){
        QFile fileInput(fileName);
        if ( !fileInput.open(QIODevice::ReadOnly ) ){
            qCritical("Cannot open file: %s", qPrintable(fileName));
            return QString("Cannot open file: ") + fileName;
        }
        QTextStream in(&fileInput);
        setFile(file);
        setPath(QFileInfo(fileInput).path());
        QString content = in.readAll();
        fileInput.close();
        return content;
    }
    return "";
}

void QCodeDocument::saveFile(const QUrl& file, const QString &content){
    QString fileName = file.toLocalFile();
    if ( fileName != "" ){
        QFile fileInput(fileName);
        if ( !fileInput.open(QIODevice::WriteOnly ) ){
            qCritical("Can't open file for writing");
            return;
        } else {
            QTextStream stream(&fileInput);
            stream << content;
            stream.flush();
            fileInput.close();
            setPath(QFileInfo(fileInput).path());
            setFile(file);
        }
    }
}

void QCodeDocument::saveFile(const QString& content){
    if ( !m_openedFile.isEmpty() )
        saveFile(m_openedFile, content);
}
