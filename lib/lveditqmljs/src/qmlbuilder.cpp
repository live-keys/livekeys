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

#include "qmlbuilder.h"

#include <QQmlContext>
#include <QQmlEngine>

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/visuallogqt.h"
#include "live/hookcontainer.h"

#include "live/project.h"
#include "live/projectdocument.h"

namespace lv{

QmlBuilder::QmlBuilder(QQuickItem *parent)
    : QQuickItem(parent)
{
}

QmlBuilder::~QmlBuilder(){
}

void QmlBuilder::setSource(const QString &source){
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();
}

void QmlBuilder::rebuild(){
    m_object->deleteLater();
    m_object = nullptr;

    build();
}

void QmlBuilder::__updateImplicitWidth(){
    QQuickItem* item = qobject_cast<QQuickItem*>(m_object);
    if ( item ){
        setImplicitWidth(item->width());
    }
}

void QmlBuilder::__updateImplicitHeight(){
    QQuickItem* item = qobject_cast<QQuickItem*>(m_object);
    if ( item ){
        setImplicitHeight(item->height());
    }
}

void QmlBuilder::componentComplete(){
    QQuickItem::componentComplete();

    QQmlContext* ctx = qmlContext(this);

    QString id = ctx->nameForObject(this);
    QString url = ctx->baseUrl().toString();

    HookContainer* hk = qobject_cast<HookContainer*>(ctx->contextProperty("hooks").value<QObject*>());
    hk->insertKey(m_source, id, this);

    build();
}

void QmlBuilder::build(){
    QQmlContext* ctx = qmlContext(this);
    ViewEngine* ve = ViewEngine::grab(this);

    Project* proj = qobject_cast<Project*>(ctx->contextProperty("project").value<QObject*>());

    ProjectDocument* doc = qobject_cast<ProjectDocument*>(proj->isOpened(m_source));
    QByteArray contentBytes;
    if ( doc ){
        contentBytes = doc->content();
    } else {
        QFile f(m_source);
        if ( !f.open(QFile::ReadOnly) ){
            Exception e = CREATE_EXCEPTION(Exception, "Failed to read file for running:" + m_source.toStdString(), Exception::toCode("~File"));
            QmlError(e, this).jsThrow();
            return;
        }
        contentBytes = f.readAll();
    }

    ViewEngine::ComponentResult::Ptr cr = ve->createObject(m_source, contentBytes, this, ctx);
    if ( cr->hasError() ){
        QmlError::join(cr->errors).jsThrow();
        return;
    }

    m_object = cr->object;
    QQuickItem* item = qobject_cast<QQuickItem*>(m_object);
    if ( item ){
        item->setParentItem(this);
        setImplicitHeight(item->height());
        setImplicitWidth(item->width());
        connect(item, &QQuickItem::widthChanged, this, &QmlBuilder::__updateImplicitWidth);
        connect(item, &QQuickItem::heightChanged, this, &QmlBuilder::__updateImplicitHeight);
    }

    emit objectChanged();
}

void QmlBuilder::setSourceComponent(QQmlComponent *sourceComponent){
    if (m_sourceComponent == sourceComponent)
        return;

    m_sourceComponent = sourceComponent;
    emit sourceComponentChanged();
}

}// namespace
