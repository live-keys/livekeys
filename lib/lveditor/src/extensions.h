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

#ifndef LVEXTENSIONS_H
#define LVEXTENSIONS_H

#include <QObject>
#include <QQmlPropertyMap>

#include "live/lvviewglobal.h"
#include "live/workspaceextension.h"
#include "live/package.h"

namespace lv{

class ViewEngine;
class LV_EDITOR_EXPORT Extensions : public QObject{

    Q_OBJECT

    friend class WorkspaceLayer;

public:
    Extensions(ViewEngine* engine, const QString& settingsPath, QObject *parent = nullptr);
    ~Extensions();

    QQmlPropertyMap *globals();

    QMap<std::string, WorkspaceExtension*>::iterator begin();
    QMap<std::string, WorkspaceExtension*>::iterator end();

    const QString& path() const;

private:
    QMap<std::string, WorkspaceExtension*> m_extensions;

    QQmlPropertyMap*     m_globals;
    QString              m_path;
    ViewEngine*          m_engine;
};

/** Globals getter */
inline QQmlPropertyMap* Extensions::globals(){
    return m_globals;
}

/** Begin iterator of extensions */
inline QMap<std::string, WorkspaceExtension*>::iterator Extensions::begin(){
    return m_extensions.begin();
}

/** End iterator of extensions */
inline QMap<std::string, WorkspaceExtension*>::iterator Extensions::end(){
    return m_extensions.end();
}

} // namespace

#endif // LVEXTENSIONS_H
