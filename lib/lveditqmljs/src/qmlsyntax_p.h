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

#ifndef LVQMLSYNTAX_H
#define LVQMLSYNTAX_H

#include <QObject>
#include <QJSValue>

#include "documentqmlinfo.h"

namespace lv{

class ViewEngine;
class QmlSyntax : public QObject{

    Q_OBJECT

public:
    explicit QmlSyntax(QObject *parent = nullptr);
    ~QmlSyntax() override;

public slots:
    QJSValue parseObjectDeclarations(const QString& content);
    QJSValue parsePathDeclaration(const QString& content);

private:
    QJSValue recurseObjectDeclarations(DocumentQmlValueObjects::RangeObject* object, const QString& content);

private:
    ViewEngine* m_engine;
};

}// namespace

#endif // LVQMLSYNTAX_H
