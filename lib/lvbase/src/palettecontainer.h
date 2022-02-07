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

#ifndef LVPALETTECONTAINER_H
#define LVPALETTECONTAINER_H

#include "live/lvbaseglobal.h"
#include "live/utf8.h"
#include <map>
#include <list>

namespace lv{

class LV_BASE_EXPORT PaletteContainer{

public:
    class LV_BASE_EXPORT PaletteInfo{

    public:
        friend class PaletteContainer;

    public:
        PaletteInfo(const Utf8& type = "", const Utf8& path = "", const Utf8& name = "", const Utf8& extension = "", const Utf8& plugin = "");

        const Utf8& type() const;
        const Utf8& path() const;
        const Utf8& name() const;
        const Utf8& extension() const;
        const Utf8& plugin() const;
        bool isValid() const;

    private:
        Utf8 m_type;
        Utf8 m_path;
        Utf8 m_name;
        Utf8 m_extension;
        Utf8 m_plugin;
    };

public:
    PaletteContainer();
    ~PaletteContainer();

    void addPalette(const Utf8& type, const Utf8& path, const Utf8& name, const Utf8& extension, const Utf8& plugin);
    void clear();
    Utf8 toString() const;

    PaletteInfo findPaletteByName(const Utf8& name) const;
    PaletteInfo findFirstPalette(const Utf8& type) const;
    std::list<PaletteInfo> findPalettes(const Utf8& type) const;

private:
    std::map<std::string, std::list<PaletteInfo> >* m_palettes;
};

}// namespace

#endif // PALETTECONTAINER_H
