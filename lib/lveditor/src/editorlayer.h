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

#ifndef LVEDITORLAYER_H
#define LVEDITORLAYER_H

#include <QObject>
#include "lveditorglobal.h"
#include "live/layer.h"

namespace lv{

class PaletteLoader;

class LV_EDITOR_EXPORT EditorLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* environment READ environment WRITE setEnvironment NOTIFY environmentChanged)

public:
    explicit EditorLayer(QObject *parent = nullptr);
    ~EditorLayer();

    QObject* environment() const;
    void setEnvironment(QObject* environment);

    PaletteLoader* paletteLoader();

signals:
    void environmentChanged();

private:
    PaletteLoader* m_paletteLoader;
    QObject*       m_environment;
};

inline QObject *EditorLayer::environment() const{
    return m_environment;
}

inline PaletteLoader *EditorLayer::paletteLoader(){
    return m_paletteLoader;
}

}// namespace

#endif // LVEDITORLAYER_H
