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

#include "visuallogteststub.h"

VisualLogTestStub::VisualLogTestStub(QObject *parent)
    : QObject(parent)
    , m_field1(0)
{
}

VisualLogTestStub::VisualLogTestStub(int field1, const QString &field2, QObject* parent)
    : QObject(parent)
    , m_field1(field1)
    , m_field2(field2)
{
}

VisualLogTestStub::~VisualLogTestStub(){
}
