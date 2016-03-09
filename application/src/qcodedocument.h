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

#ifndef QCODEDOCUMENT_H
#define QCODEDOCUMENT_H

#include <QQuickItem>

class QCodeDocument : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)

public:
    explicit QCodeDocument(QQuickItem *parent = 0);
    
    const QString& path() const;
    void setPath(const QString& path);

    const QString& file() const;
    void setFile(const QString& file);

public slots:
    QString openFile(const QUrl& file);
    void saveFile(const QUrl &file, const QString& content);
    void saveFile(const QString& content);

signals:
    void pathChanged();
    void fileChanged();

private:
    QString     m_path;
    QString     m_openedFile;
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

inline const QString& QCodeDocument::file() const{
    return m_openedFile;
}

inline void QCodeDocument::setFile(const QString& file){
    if ( m_openedFile != file ){
        m_openedFile = file;
        emit fileChanged();
    }
}

#endif // QCODEDOCUMENT_H
