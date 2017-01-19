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

#ifndef QPROJECTFILE_H
#define QPROJECTFILE_H

#include "qlcveditorglobal.h"

#include <QObject>
#include "qprojectentry.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QProjectFile : public QProjectEntry{

    Q_OBJECT
    Q_PROPERTY(QString text     READ text        WRITE setText        NOTIFY textChanged)
    Q_PROPERTY(bool isOpen      READ isOpen      WRITE setIsOpen      NOTIFY isOpenChanged)
    Q_PROPERTY(bool isDirty     READ isDirty     WRITE setIsDirty     NOTIFY isDirtyChanged)
    Q_PROPERTY(bool isMonitored READ isMonitored WRITE setIsMonitored NOTIFY isMonitoredChanged)

public:
    explicit QProjectFile(const QString& path, QProjectEntry *parent = 0);
    QProjectFile(const QString &path, const QString& name, QProjectEntry *parent);
    ~QProjectFile();

    const QString &text() const;
    void setText(const QString& text);

    bool isOpen() const;
    void setIsOpen(bool isOpen);

    bool isMonitored() const;
    void setIsMonitored(bool monitored);

    bool isDirty() const;
    void setIsDirty(bool isDirty);

signals:
    void textChanged();
    void isActiveChanged();
    void isOpenChanged();
    void isDirtyChanged();
    void isMonitoredChanged();

private:
    QString m_text;
    bool    m_isOpen;
    bool    m_isDirty;
    bool    m_isMonitored;
};

inline const QString &QProjectFile::text() const{
    return m_text;
}

inline void QProjectFile::setText(const QString &text){
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged();
}

inline bool QProjectFile::isOpen() const{
    return m_isOpen;
}

inline bool QProjectFile::isDirty() const{
    return m_isDirty;
}

inline void QProjectFile::setIsOpen(bool isOpen){
    if (m_isOpen == isOpen)
        return;

    m_isOpen = isOpen;
    emit isOpenChanged();
}

inline void QProjectFile::setIsDirty(bool isDirty){
    if (m_isDirty == isDirty)
        return;


    m_isDirty = isDirty;
    emit isDirtyChanged();
}

inline bool QProjectFile::isMonitored() const{
    return m_isMonitored;
}

inline void QProjectFile::setIsMonitored(bool monitored){
    if ( m_isMonitored != monitored ){
        m_isMonitored = monitored;
        emit isMonitoredChanged();
    }
}

}// namespace

#endif // QPROJECTFILE_H
