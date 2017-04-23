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

#ifndef QPROJECTDOCUMENT_H
#define QPROJECTDOCUMENT_H

#include <QObject>
#include <QDateTime>
#include "qlcveditorglobal.h"

namespace lcv{

class QProject;
class QProjectFile;
class Q_LCVEDITOR_EXPORT QProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectFile* file    READ file        NOTIFY fileChanged)
    Q_PROPERTY(QString            content READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored           READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty               READ isDirty     WRITE setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
    enum OpenMode{
        Edit = 0,
        Monitor,
        EditIfNotOpen
    };

public:
    explicit QProjectDocument(QProjectFile* file, bool isMonitored, QProject *parent);
    ~QProjectDocument();

    lcv::QProjectFile* file() const;

    const QString& content() const;

    void setIsDirty(bool isDirty);
    bool isDirty() const;

    void setIsMonitored(bool isMonitored);
    bool isMonitored() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    QProject* parentAsProject();

public slots:
    void dumpContent(const QString& content);
    void readContent();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

signals:
    void isDirtyChanged();
    void isMonitoredChanged();
    void fileChanged();
    void contentChanged();

private:
    QProjectFile* m_file;
    QString       m_content;
    QDateTime     m_lastModified;

    bool          m_isDirty;
    bool          m_isMonitored;
};

inline QProjectFile *QProjectDocument::file() const{
    return m_file;
}

inline const QString &QProjectDocument::content() const{
    return m_content;
}

inline void QProjectDocument::setIsDirty(bool isDirty){
    if ( m_isDirty == isDirty )
        return;

    m_isDirty = isDirty;
    isDirtyChanged();
}

inline bool QProjectDocument::isDirty() const{
    return m_isDirty;
}

inline void QProjectDocument::setIsMonitored(bool isMonitored){
    if ( m_isMonitored == isMonitored )
        return;

    m_isMonitored = isMonitored;
    emit isMonitoredChanged();
}

inline bool QProjectDocument::isMonitored() const{
    return m_isMonitored;
}

inline const QDateTime &QProjectDocument::lastModified() const{
    return m_lastModified;
}

inline void QProjectDocument::setLastModified(const QDateTime &lastModified){
    m_lastModified = lastModified;
}

}// namespace

#endif // QPROJECTDOCUMENT_H
