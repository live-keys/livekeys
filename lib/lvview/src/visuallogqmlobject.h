/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVVISUALLOGQMLOBJECT_H
#define LVVISUALLOGQMLOBJECT_H

#include "live/lvviewglobal.h"
#include <QJSValue>
#include <QObject>

namespace lv{

class LV_VIEW_EXPORT VisualLogQmlObject : public QObject{

    Q_OBJECT

public:
    VisualLogQmlObject(QObject* parent = nullptr);
    ~VisualLogQmlObject();

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

#endif // LVVISUALLOGQMLOBJECT_H
