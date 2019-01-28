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

#include "editor_plugin.h"
#include "live/codepalette.h"
#include "live/palettelist.h"
#include "live/applicationcontext.h"
#include "live/visuallog.h"
#include "live/settings.h"
#include "live/liveextension.h"
#include "live/documenthandler.h"

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
    qmlRegisterUncreatableType<lv::AbstractCodeHandler>(
        uri, 1, 0, "AbstractCodeHandler", "AbstractCodeHandler is of abstract type."
    );
    qmlRegisterType<lv::DocumentHandler>(uri, 1, 0, "DocumentHandler");
    qmlRegisterType<lv::CodePalette>(    uri, 1, 0, "CodePalette");
    qmlRegisterType<lv::LiveExtension>(  uri, 1, 0, "LiveExtension");

    qmlRegisterType<lv::TextSearch>(     uri, 1, 0, "TextSearch");
}

void EditorPlugin::initializeEngine(QQmlEngine *, const char *){
}
