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

#ifndef LVQMLCOMPONENTSOURCE_H
#define LVQMLCOMPONENTSOURCE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlComponent>

#include "live/lveditqmljsglobal.h"
#include "live/qmlsourcelocation.h"
#include "live/viewengine.h"

namespace lv{

class Project;

class LV_EDITQMLJS_EXPORT QmlComponentSource : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlComponent* component READ component NOTIFY componentChanged)
    Q_PROPERTY(QUrl url                 READ url       WRITE setUrl NOTIFY urlChanged)

public:
    QmlComponentSource(QObject* parent = nullptr);
    QmlComponentSource(ViewEngine* ve, const QUrl& url, const QByteArray& imports, const QString& source, QObject* parent = nullptr);
    ~QmlComponentSource();

    const QString& sourceCode();
    const QByteArray& importSourceCode();
    const QUrl& url() const;

    void setUrl(const QUrl& url);

    QQmlComponent* component();

    void updateFromUrl();

    static bool parserPropertyValidateHook(
        QmlSourceLocation location,
        QmlSourceLocation valueLocation,
        const QString& name);
    static void parserPropertyHook(
        ViewEngine* ve,
        QObject* target,
        const QByteArray& imports,
        QmlSourceLocation location,
        QmlSourceLocation valueLocation,
        const QString& name,
        const QString& source
    );
    static void parserDefaultPropertyHook(
        ViewEngine* ve,
        QObject* target,
        const QByteArray& imports,
        const QList<QmlSourceLocation>& children
    );

public slots:
    QObject* createObject(QObject* parent = nullptr);

protected:
    void componentComplete();
    void classBegin(){}

signals:
    void componentChanged();
    void urlChanged();

private:
    static QString captureSourceFromLocation(ViewEngine* ve, const QmlSourceLocation &location);
    void captureSourceFromUrl();
    void syncComponent();

    ViewEngine*     m_viewEngine;
    QUrl            m_url;
    QQmlComponent*  m_component;
    bool            m_componentSync;
    bool            m_componentComplete;
    QString         m_sourceCode;
    QByteArray      m_importSourceCode;
};

inline const QString &QmlComponentSource::sourceCode(){
    return m_sourceCode;
}

inline const QByteArray &QmlComponentSource::importSourceCode(){
    return m_importSourceCode;
}

inline const QUrl &QmlComponentSource::url() const{
    return m_url;
}

inline QQmlComponent *QmlComponentSource::component(){
    syncComponent();
    return m_component;
}

}// namespace

#endif // LVQMLCOMPONENTSOURCE_H
