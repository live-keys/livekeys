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

#ifndef LVEDITOR_PRIVATE_PLUGIN_H
#define LVEDITOR_PRIVATE_PLUGIN_H

#include <QQmlExtensionPlugin>
#include "live/lveditorglobal.h"
#include "live/viewengine.h"
#include "live/settings.h"

/// \private
class LV_EDITOR_EXPORT EditorPrivatePlugin : public QQmlExtensionPlugin{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    EditorPrivatePlugin(QObject* parent = Q_NULLPTR) : QQmlExtensionPlugin(parent){}
    ~EditorPrivatePlugin(){}

    void registerTypes(const char* uri) Q_DECL_OVERRIDE;
    void initializeEngine(QQmlEngine* engine, const char* uri) Q_DECL_OVERRIDE;
    void initializeEngine(lv::ViewEngine* engine, lv::Settings* settings, const char* uri);
};

#endif // LVEDITOR_PRIVATE_PLUGIN_H
