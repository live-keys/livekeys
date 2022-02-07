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

#include "qprojectionsurface.h"
#include "qmat.h"
#include "qprojectionmapper.h"

QProjectionSurface::QProjectionSurface(QObject *parent)
    : QObject(parent)
    , m_destination(QJSValue::NullValue)
    , m_input(nullptr)
{
}

QProjectionSurface::~QProjectionSurface()
{
}

QMat *QProjectionSurface::input() const
{
    return m_input;
}

void QProjectionSurface::setInput(QMat *input)
{
    if (m_input == input)
        return;
    m_input = input;
    emit inputChanged();

}

QProjectionMapper *QProjectionSurface::mapper() const
{
    return qobject_cast<QProjectionMapper*>(parent());
}

QJSValue QProjectionSurface::destination() const
{
    return m_destination;
}

void QProjectionSurface::setDestination(const QJSValue &destination)
{
    if (m_destination.equals(destination))
        return;
    m_destination = destination;
    emit destinationChanged();
}
