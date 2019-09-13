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

#ifndef LVPALLETECONTAINER_H
#define LVPALLETECONTAINER_H

#include "live/lveditorglobal.h"
#include "live/plugin.h"

#include <QString>

class QQmlEngine;
class QQmlComponent;

namespace lv{

class CodePalette;
class PaletteList;
class PaletteLoader;
class PaletteContainerPrivate;
class LV_EDITOR_EXPORT PaletteContainer{

public:
    ~PaletteContainer();

    void scanPalettes(const QString& path);
    void scanPalettes(Plugin::Ptr plugin);

    static PaletteContainer* create(QQmlEngine* engine, const QString& path);

    PaletteLoader* findPalette(const QString& type) const;
    PaletteList* findPalettes(const QString& type, PaletteList* list = nullptr);
    int countPalettes(const QString& type) const;

    QString paletteName(PaletteLoader* loader);
    static const QString& palettePath(PaletteLoader* loader);
    CodePalette* createPalette(PaletteLoader* loader);

    int size() const;

private:
    PaletteContainer(QQmlEngine* engine);

    Q_DISABLE_COPY(PaletteContainer)
    Q_DECLARE_PRIVATE(PaletteContainer)

    PaletteContainerPrivate* const d_ptr;
};

}// namespace

#endif // LVLIVEPALLETECONTAINER_H
