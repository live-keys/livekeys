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

#ifndef QLIVEPALLETECONTAINER_H
#define QLIVEPALLETECONTAINER_H

#include "qlcveditorglobal.h"
#include <QString>

class QQmlEngine;
class QQmlComponent;

namespace lcv{

class QCodeConverter;
class QLivePaletteContainerPrivate;
class Q_LCVEDITOR_EXPORT QLivePaletteContainer{

public:
    ~QLivePaletteContainer();

    void scanPalettes(const QString& path);
    void scanPaletteDir(const QString& path);

    static QLivePaletteContainer* create(QQmlEngine* engine, const QString& path);

    QCodeConverter* findPalette(const QString& type, const QString& object = "");
    QCodeConverter* findPalette(const QString &type, const QStringList &object);

    int size() const;

private:
    QLivePaletteContainer(QQmlEngine* engine);

    Q_DISABLE_COPY(QLivePaletteContainer)
    Q_DECLARE_PRIVATE(QLivePaletteContainer)

    QLivePaletteContainerPrivate* const d_ptr;
};

}// namespace

#endif // QLIVEPALLETECONTAINER_H
