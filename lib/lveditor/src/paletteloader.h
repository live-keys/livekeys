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
#include "live/palettecontainer.h"

#include <QString>

class QJSValue;
class QQmlEngine;
class QQmlComponent;

/*
 *TODO: Clear paletteContainer when packageGraph is cleared
 *
PaletteLoader{
   std::map<std::string, PaletteComponent> m_loaders // this is a path to Loader
}
*/

namespace lv{

class CodePalette;
class ViewEngine;
class PaletteLoaderPrivate;
class LV_EDITOR_EXPORT PaletteLoader{

public:
    ~PaletteLoader();
    static PaletteLoader* create(ViewEngine* engine);

    PaletteContainer::PaletteInfo findPaletteByName(const QString& name) const;
    PaletteContainer::PaletteInfo findFirstPalette(const QString &type) const;
    std::list<PaletteContainer::PaletteInfo> findPalettes(const QString& type) const;

    CodePalette* createPalette(const PaletteContainer::PaletteInfo& pi);
    QJSValue paletteContent(const PaletteContainer::PaletteInfo& pi);

    static bool configuresLayout(const PaletteContainer::PaletteInfo& pi);

private:
    PaletteLoader(ViewEngine *engine);

    Q_DISABLE_COPY(PaletteLoader)
    Q_DECLARE_PRIVATE(PaletteLoader)

    PaletteLoaderPrivate* const d_ptr;
};

}// namespace

#endif // LVLIVEPALLETECONTAINER_H
