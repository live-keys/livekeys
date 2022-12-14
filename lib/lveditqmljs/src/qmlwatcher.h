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

#ifndef LVQMLWATCHER_H
#define LVQMLWATCHER_H

#include "live/lveditqmljsglobal.h"

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class ViewEngine;
class HookContainer;

class LV_EDITQMLJS_EXPORT QmlWatcher : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_PROPERTY(QObject* singleton READ target WRITE setSingleton NOTIFY targetChanged)
    Q_PROPERTY(QObject* target    READ target WRITE setTarget NOTIFY targetChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QmlWatcher(QObject *parent = nullptr);
    ~QmlWatcher() override;

    const QString& referencedFile() const{ return m_referencedFile; }
    const QString& declaredId() const{ return m_declaredId; }

    bool isEnabled() const;

    void initialize(ViewEngine* engine, HookContainer* hooks, const QString& referencedFile, const QString& declaredId);

    QObject* target() const;
    void setSingleton(QObject* singleton);

    void setTarget(QObject* target);

public slots:
    void setDynamicTarget(QObject* target, const QString& id);

signals:
    void ready();
    void targetChanged();

protected:
    void classBegin() override{}
    void componentComplete() override;

private:
    void resolveTarget();
    bool checkChildDeclarations();

    QString  m_referencedFile;
    QString  m_declaredId;
    bool     m_isEnabled;
    bool     m_componentComplete;
    QObject* m_target;
};

inline bool QmlWatcher::isEnabled() const{
    return m_isEnabled;
}

inline QObject *QmlWatcher::target() const{
    return m_target;
}

}// namespace

#endif // LVQMLWATCHER_H
