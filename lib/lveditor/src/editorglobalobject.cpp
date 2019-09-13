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

#include "editorglobalobject.h"
#include "live/project.h"
#include <QDebug>
/**
 * \class lv::EditorGlobalObject
 * \brief Global object used through LiveKeys just to have access to project and the palette container.
 *
 * Used by a lot of different classes just for sharing these two properties from their QML context.
 * \ingroup lveditor
 */
namespace lv{

/**
 * @brief Default constructor of EditorGlobalObject
 */
EditorGlobalObject::EditorGlobalObject(
        Project *project,
        PaletteContainer *paletteContainer,
        QObject *parent)
    : QObject(parent)
    , m_project(project)
    , m_paletteContainer(paletteContainer)
{
}

/**
 * \brief Project getter
 */
Project *EditorGlobalObject::project(){
    return m_project;
}

}// namespace
