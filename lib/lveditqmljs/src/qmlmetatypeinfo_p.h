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

#ifndef LVQMLMETATYPEINFO_H
#define LVQMLMETATYPEINFO_H

#include <QObject>
#include <QJSValue>
#include "live/qmllanguageinfo.h"
#include "live/viewengine.h"

namespace lv{

class QmlMetaTypeInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlMetaTypeInfo(const QmlInheritanceInfo& typeInfo, ViewEngine *engine, QObject *parent = nullptr);
    ~QmlMetaTypeInfo();

public slots:
    QJSValue typeName(const QString& typeReference);
    QJSValue propertyInfo(const QString& propertyName);

private:
    QmlInheritanceInfo m_inheritanceInfo;
    ViewEngine*        m_engine;
};

}// namespace

#endif // LVQMLMETATYPEINFO_H
