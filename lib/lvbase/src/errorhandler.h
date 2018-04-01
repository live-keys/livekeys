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

#ifndef LVERRORHANDLER_H
#define LVERRORHANDLER_H

#include "live/lvbaseglobal.h"
#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class Engine;
class LV_BASE_EXPORT ErrorHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QObject* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit ErrorHandler(QObject *parent = 0);
    ~ErrorHandler();

    void componentComplete();
    void classBegin(){}

    void signalError(const QJSValue& error);
    void signalWarning(const QJSValue& error);

    void setTarget(QObject* target);
    QObject* target() const;

signals:
    void error(QJSValue e);
    void fatal(QJSValue e);
    void warning(QJSValue e);

    void targetChanged(QObject* target);

public slots:
    void skip(const QJSValue& error);

private:
    Engine*  m_engine;
    QObject* m_target;
    bool     m_componentComplete;
};

inline QObject *ErrorHandler::target() const{
    return m_target;
}

}// namespace

#endif // LVERRORHANDLER_H
