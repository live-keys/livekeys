/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef QFILEREADER_H
#define QFILEREADER_H

#include <QObject>
#include <QQmlParserStatus>
#include "qliveglobal.h"

class QFileSystemWatcher;

/// \private
class Q_LIVE_EXPORT QFileReader : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString source  READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(bool monitor    READ monitor WRITE setMonitor NOTIFY monitorChanged)
    Q_PROPERTY(QByteArray data READ data    NOTIFY dataChanged)

public:
    explicit QFileReader(QObject *parent = 0);
    virtual ~QFileReader();

    const QByteArray& data() const;

    void setSource(QString source);
    QString source() const;

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

    void setMonitor(bool monitor);
    bool monitor() const;

public slots:
    QString asString(const QByteArray& data);
    void systemFileChanged(const QString& file);

signals:
    void dataChanged(const QByteArray& data);
    void sourceChanged(QString source);
    void monitorChanged();

private:
    void resync();

    QByteArray m_data;
    QString    m_source;
    bool       m_componentComplete;

    QFileSystemWatcher* m_watcher;
};

inline const QByteArray &QFileReader::data() const{
    return m_data;
}

inline QString QFileReader::source() const{
    return m_source;
}

inline void QFileReader::componentComplete(){
    m_componentComplete = true;
    resync();
}

inline bool QFileReader::monitor() const{
    return (m_watcher != 0);
}

inline QString QFileReader::asString(const QByteArray &data){
    return QString::fromUtf8(data);
}

#endif // QFILEREADER_H
