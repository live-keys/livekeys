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

#include "shapes_plugin.h"

#include "svgview.h"
#include "triangle.h"
#include "linegrid.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include <QSvgRenderer>
#include <QPainter>

void ShapesPlugin::registerTypes(const char *uri){
    qmlRegisterType<lv::SvgView>( uri, 1, 0, "SvgView");
    qmlRegisterType<lv::Triangle>(uri, 1, 0, "Triangle");
    qmlRegisterType<lv::LineGrid>(uri, 1, 0, "LineGrid");
}

void ShapesPlugin::initializeEngine(QQmlEngine *, const char *){
}
