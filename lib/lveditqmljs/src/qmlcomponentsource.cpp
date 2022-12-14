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

#include "qmlcomponentsource.h"
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlEngine>

#include "live/viewengine.h"
#include "live/visuallogqt.h"
#include "live/exception.h"
#include "live/documentqmlinfo.h"
#include "live/hookcontainer.h"

namespace lv{

QmlComponentSource::QmlComponentSource(QObject *parent)
    : QObject(parent)
    , m_viewEngine(nullptr)
    , m_component(nullptr)
    , m_componentSync(true)
    , m_componentComplete(false)
{
}

QmlComponentSource::QmlComponentSource(
        ViewEngine *ve, const QUrl &url, const QByteArray &imports, const QString &source, QObject *parent)
    : QObject(parent)
    , m_viewEngine(ve)
    , m_url(url)
    , m_component(nullptr)
    , m_componentSync(false)
    , m_componentComplete(false)
    , m_sourceCode(source)
    , m_importSourceCode(imports)
{
}

QmlComponentSource::~QmlComponentSource(){
}


bool QmlComponentSource::parserPropertyValidateHook(QmlSourceLocation, QmlSourceLocation, const QString &name){
    if ( name != "source" && name != "" ){
        return false;
    }
    return true;
}

void QmlComponentSource::parserPropertyHook(
        ViewEngine *ve,
        QObject *target,
        const QByteArray &imports,
        QmlSourceLocation,
        QmlSourceLocation valueLocation,
        const QString &name,
        const QString &)
{
    if ( name != "source" ){
        THROW_EXCEPTION(lv::Exception, "Invalid property: " + name.toStdString(), Exception::toCode("~Property"));
    }
    QmlComponentSource* cs = qobject_cast<QmlComponentSource*>(target);
    cs->m_viewEngine = ve;
    cs->m_url = QUrl::fromLocalFile(valueLocation.file());
    cs->m_importSourceCode = imports;
    cs->m_sourceCode = captureSourceFromLocation(ve, valueLocation);
    cs->m_componentSync = false;
}

void QmlComponentSource::parserDefaultPropertyHook(
        ViewEngine *ve,
        QObject *target,
        const QByteArray &imports,
        const QList<QmlSourceLocation> &children)
{
    if ( children.length() != 1 ){
        THROW_EXCEPTION(lv::Exception, "ComponentSource expects a single child.", Exception::toCode("SingleChild"));
    }

    QmlSourceLocation valueLocation = children[0];

    QmlComponentSource* cs = qobject_cast<QmlComponentSource*>(target);
    cs->m_viewEngine = ve;
    cs->m_url = QUrl::fromLocalFile(valueLocation.file());
    cs->m_importSourceCode = imports;
    cs->m_sourceCode = captureSourceFromLocation(ve, valueLocation);
    cs->m_componentSync = false;
}

QObject *QmlComponentSource::createObject(QObject *parent){
    QByteArray source = m_importSourceCode + "\n" + m_sourceCode.toUtf8();
    ViewEngine::ComponentResult::Ptr cr = m_viewEngine->createObject(m_url.toLocalFile(), source, parent, new QQmlContext(qmlContext(this)));
    if ( cr->hasError() ){
        QmlError::join(cr->errors).jsThrow();
        return nullptr;
    }
    return cr->object;
}


void QmlComponentSource::componentComplete(){
    m_componentComplete = true;
    if ( !m_viewEngine ){
        m_viewEngine = ViewEngine::grab(this);
        if ( !m_url.isEmpty() ){
            captureSourceFromUrl();

            QQmlContext* ctx = qmlContext(this);

            QString id = ctx->nameForObject(this);
            HookContainer* hk = qobject_cast<HookContainer*>(ctx->contextProperty("hooks").value<QObject*>());
            hk->insertKey(m_url.toLocalFile(), id, this);
        }
    }
}

QString QmlComponentSource::captureSourceFromLocation(ViewEngine *ve, const QmlSourceLocation &location){
    QString contents = ve->componentSource(location.file());
    int line = location.line();
    int col = location.column();

    int lineIndex = 1;
    int startOfLinePos = 0;
    while ( lineIndex < line ){
        startOfLinePos = contents.indexOf('\n', startOfLinePos) + 1;
        lineIndex++;
    }

    startOfLinePos += col - 1;
    int closingBracePos = DocumentQmlInfo::findClosingBrace(contents, startOfLinePos);
    return contents.mid(startOfLinePos, closingBracePos - startOfLinePos);
}

void QmlComponentSource::captureSourceFromUrl(){
    m_sourceCode = QString::fromStdString(m_viewEngine->fileIO()->readFromFile(m_url.toLocalFile().toStdString()));
    m_componentSync = false;
    emit componentChanged();
}

void QmlComponentSource::syncComponent(){
    if ( !m_componentSync ){
        if ( m_component ){
            m_component->deleteLater();
        }
        m_component = new QQmlComponent(m_viewEngine->engine(), this);
        m_component->setData(m_importSourceCode + "\n" + m_sourceCode.toUtf8(), m_url);
        m_componentSync = true;
    }
}


void QmlComponentSource::setUrl(const QUrl& url){
    if (m_url == url)
        return;

    m_url = url;
    emit urlChanged();

    if ( !m_componentComplete )
        return;

    captureSourceFromUrl();
}

void QmlComponentSource::updateFromUrl(){
    captureSourceFromUrl();
}

}// namespace
