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

#ifndef QCODEDOCUMENT_HPP
#define QCODEDOCUMENT_HPP

#include <QQuickItem>

class QCodeDocument : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit QCodeDocument(QQuickItem *parent = 0);

public slots:
    QString openFile(const QUrl& file);
    void saveFile(const QUrl &file, const QString& content);
    
    const QString& path() const;
    void setPath(const QString& path);

    void setEngine(QQmlEngine* engine);

signals:
    void pathChanged();

private:
    QString     m_path;
    QString     m_openedFile;
    QQmlEngine* m_engine;
    QStringList m_importPaths;

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

#endif // QCODEDOCUMENT_HPP
