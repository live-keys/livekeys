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

#ifndef QMATLOADER_H
#define QMATLOADER_H

#include <QQuickItem>

class QMat;
class QJSValue;

class QMatLoader : public QQuickItem{

    Q_OBJECT

public:
    explicit QMatLoader(QQuickItem* parent = 0);
    ~QMatLoader();

public slots:
    QMat* staticLoad(const QString& id, const QJSValue& params);

};

#endif // QMATLOADER_H
