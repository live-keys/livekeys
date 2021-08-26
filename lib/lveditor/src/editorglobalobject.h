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

#ifndef LVEDITORGLOBALOBJECT_H
#define LVEDITORGLOBALOBJECT_H

#include "lveditorglobal.h"
#include <QObject>

namespace lv{

class Project;
class PaletteLoader;

/// \private
class LV_EDITOR_EXPORT EditorGlobalObject : public QObject{

    Q_OBJECT
    Q_PROPERTY(Project* project READ project NOTIFY projectChanged)

public:
    explicit EditorGlobalObject(
        Project* project,
        PaletteLoader* paletteContainer,
        QObject *parent = 0
    );

    Project* project();
    PaletteLoader* paletteContainer();

signals:
    void projectChanged();

private:
    Project*              m_project;
    PaletteLoader* m_paletteContainer;
};

inline PaletteLoader *EditorGlobalObject::paletteContainer(){
    return m_paletteContainer;
}

}// namespace

#endif // LVEDITORGLOBALOBJECT_H
