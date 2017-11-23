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

#ifndef LVVISUALLOGJSOBJECT_H
#define LVVISUALLOGJSOBJECT_H

#include "live/lvbaseglobal.h"
#include <QJSValue>
#include <QObject>

namespace lv{

class LV_BASE_EXPORT VisualLogJsObject : public QObject{

    Q_OBJECT

public:
    VisualLogJsObject(QObject* parent = 0);
    ~VisualLogJsObject();

public slots:
    void f(const QJSValue& messageOrCategory, const QJSValue& message = QJSValue());
    void e(const QJSValue& messageOrCategory, const QJSValue& message = QJSValue());
    void w(const QJSValue& messageOrCategory, const QJSValue& message = QJSValue());
    void i(const QJSValue& messageOrCategory, const QJSValue& message = QJSValue());
    void d(const QJSValue& messageOrCategory, const QJSValue& message = QJSValue());
    void v(const QJSValue& messageOrCategory, const QJSValue& message = QJSValue());
    void configure(const QString& name, const QJSValue& options);
};

}// namespace

#endif // VISUALLOGJSOBJECT_H
