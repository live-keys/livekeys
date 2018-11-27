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

#ifndef LVLIVEPALLETECONTAINER_H
#define LVLIVEPALLETECONTAINER_H

#include "live/lveditorglobal.h"
#include <QString>

class QQmlEngine;
class QQmlComponent;

namespace lv{

class CodeConverter;
class LivePalette;
class LivePaletteList;
class LivePaletteLoader;
class LivePaletteContainerPrivate;
class LV_EDITOR_EXPORT LivePaletteContainer{

public:
    ~LivePaletteContainer();

    void scanPalettes(const QString& path);
    void scanPaletteDir(const QString& path);

    static LivePaletteContainer* create(QQmlEngine* engine, const QString& path);

    CodeConverter* findPalette(const QString& type, const QString& object = "");
    CodeConverter* findPalette(const QString &type, const QStringList &object);

    LivePaletteList* findPalettes(const QString& type, const QString& object = "");
    LivePaletteList* findPalettes(const QString& type, const QStringList& object);

    QString paletteName(LivePaletteLoader* loader);
    static const QString& palettePath(LivePaletteLoader* loader);
    LivePalette* createPalette(LivePaletteLoader* loader);

    int size() const;

private:
    LivePaletteContainer(QQmlEngine* engine);

    Q_DISABLE_COPY(LivePaletteContainer)
    Q_DECLARE_PRIVATE(LivePaletteContainer)

    LivePaletteContainerPrivate* const d_ptr;
};

}// namespace

#endif // LVLIVEPALLETECONTAINER_H
