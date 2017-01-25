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

#ifndef QCODEDOCUMENT_H
#define QCODEDOCUMENT_H

#include <QQuickItem>
#include <QUrl>

class QCodeDocument : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QUrl    file READ file WRITE setFile NOTIFY fileChanged)

public:
    explicit QCodeDocument(QQuickItem *parent = 0);
    
    const QString& path() const;
    void setPath(const QString& path);

    const QUrl &file() const;
    void setFile(const QUrl& file);

public slots:
    QString openFile(const QUrl& file);
    void saveFile(const QUrl &file, const QString& content);
    void saveFile(const QString& content);

signals:
    void pathChanged();
    void fileChanged();

private:
    QString m_path;
    QUrl    m_openedFile;
};

inline const QString &QCodeDocument::path() const{
    return m_path;
}

inline void QCodeDocument::setPath(const QString &path){
    if ( m_path != path ){
        m_path = path;
        emit pathChanged();
    }
}

inline const QUrl& QCodeDocument::file() const{
    return m_openedFile;
}

inline void QCodeDocument::setFile(const QUrl &file){
    if ( m_openedFile != file ){
        m_openedFile = file;
        emit fileChanged();
    }
}

#endif // QCODEDOCUMENT_H
