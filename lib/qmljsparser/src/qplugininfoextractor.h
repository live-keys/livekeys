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

#ifndef QPLUGININFOEXTRACTOR_H
#define QPLUGININFOEXTRACTOR_H

#include <QObject>
#include "qqmljsparserglobal.h"

class QTimer;

namespace lcv{

class QProjectQmlScanner;

class Q_QMLJSPARSER_EXPORT QPluginInfoExtractor : public QObject{

    Q_OBJECT

public:
    explicit QPluginInfoExtractor(QProjectQmlScanner* scanner, const QString& path, QObject *parent = 0);
    ~QPluginInfoExtractor();
    const QByteArray result() const;
    bool isDone() const;
    bool timedOut() const;

    void waitForResult(int msTimeout);

signals:

public slots:
    void newProjectScope();
    void timeOut();

private:
    QProjectQmlScanner* m_scanner;
    QString    m_path;
    QByteArray m_result;
    QTimer*    m_timeout;
    bool       m_isDone;
    bool       m_timedOut;
};

inline const QByteArray QPluginInfoExtractor::result() const{
    return m_result;
}

inline bool QPluginInfoExtractor::isDone() const{
    return m_isDone;
}

inline bool QPluginInfoExtractor::timedOut() const{
    return m_timedOut;
}

}// namespace

#endif // QPLUGININFOEXTRACTOR_H
