/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#ifndef LVREMOTELINERESPONSE_H
#define LVREMOTELINERESPONSE_H

#include <QObject>
#include <functional>

namespace lv{

class RemoteLineResponse : public QObject{

    Q_OBJECT

public:
    explicit RemoteLineResponse(QObject *parent = nullptr);
    ~RemoteLineResponse(){}

    void onResponse(std::function<void(const QString&, const QVariant&)> callback);

public slots:
    void send(const QString& propertyName, const QVariant& value);

private:
    std::function<void(const QString&, const QVariant&)> m_responseCallback;
};

}// namespace

#endif // LVREMOTELINERESPONSE_H
