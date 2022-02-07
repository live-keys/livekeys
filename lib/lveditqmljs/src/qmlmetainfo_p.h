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

#ifndef LVQMLMEMORYINFO_H
#define LVQMLMEMORYINFO_H

#include <QObject>
#include <QJSValue>

namespace lv {

class QmlMetaInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlMetaInfo(QObject *parent = nullptr);
    ~QmlMetaInfo();

public slots:
    bool isCppOwned(QObject* obj);
    bool isJsOwned(QObject* obj);
    QObject* cppParent(QObject* obj);
    bool isWritable(QObject* obj, QString name);
    QJSValue listProperties(QObject* obj);
    QString defaultTypeValue(const QString& type);
    QString typeName(const QString& typeReference);
};

}

#endif // LVQMLMEMORYINFO_H
