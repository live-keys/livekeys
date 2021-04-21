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

#include "editor_plugin.h"
#include "live/codepalette.h"
#include "live/palettelist.h"
#include "live/applicationcontext.h"
#include "live/visuallog.h"
#include "live/settings.h"
#include "live/workspaceextension.h"
#include "live/documenthandler.h"
#include "live/theme.h"
#include "live/documentationloader.h"

#include "textsearch.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

void EditorPlugin::registerTypes(const char *uri){
    // @uri editor
    qmlRegisterUncreatableType<lv::PaletteList>(
        uri, 1, 0, "PaletteList", "PaletteList can be accessed through a DocumentHandler."
    );
    qmlRegisterType<lv::DocumentHandler>(uri, 1, 0, "DocumentHandler");
    qmlRegisterType<lv::CodeCompletionModel>(uri, 1, 0, "CodeCompletionModel");
    qmlRegisterType<lv::CodePalette>(uri, 1, 0, "CodePalette");
    qmlRegisterType<lv::WorkspaceExtension>(uri, 1, 0, "WorkspaceExtension");
    qmlRegisterType<lv::Theme>(uri, 1, 0, "Theme");
    qmlRegisterType<lv::DocumentationLoader>(uri, 1, 0, "DocumentationLoader");

    qmlRegisterType<lv::TextSearch>(     uri, 1, 0, "TextSearch");
}

void EditorPlugin::initializeEngine(QQmlEngine *, const char *){
}
