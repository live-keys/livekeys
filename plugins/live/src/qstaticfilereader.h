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

#ifndef QSTATICFILEREADER_H
#define QSTATICFILEREADER_H

#include <QQuickItem>
#include "qliveglobal.h"

class QJSValue;
class QFileReader;

/// \private
class Q_LIVE_EXPORT QStaticFileReader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data NOTIFY dataChanged)

public:
    explicit QStaticFileReader(QQuickItem *parent = 0);
    virtual ~QStaticFileReader();

    QByteArray data() const;

signals:
    void dataChanged(const QByteArray& data);
    void init();

protected:
    void componentComplete();

public slots:
    void staticLoad(const QString& file, const QJSValue& params = QJSValue());
    QString asString(const QByteArray& data);
    void readerDataChanged(const QByteArray& data);

private:
    QFileReader* m_reader;
    QByteArray   m_data;
};

inline QByteArray QStaticFileReader::data() const{
    return m_data;
}

inline QString QStaticFileReader::asString(const QByteArray &data){
    return QString::fromUtf8(data);
}

#endif // QSTATICFILEREADER_H
