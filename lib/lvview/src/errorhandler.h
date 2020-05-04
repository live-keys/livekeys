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

#ifndef LVERRORHANDLER_H
#define LVERRORHANDLER_H

#include "live/lvviewglobal.h"
#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class ViewEngine;
class LV_VIEW_EXPORT ErrorHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QObject* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit ErrorHandler(QObject *parent = nullptr);
    ~ErrorHandler();

    void componentComplete();

    /** Blank implementation of a QQmlParserStatus method */
    void classBegin(){}

    void signalError(const QJSValue& error);
    void signalWarning(const QJSValue& error);

    void setTarget(QObject* target);

    /** Target getter */
    QObject* target() const;

signals:
    /** Signals an error */
    void error(QJSValue e);
    /** Signals a fatal error */
    void fatal(QJSValue e);
    /** Signals a warning */
    void warning(QJSValue e);
    /** Signals a target change*/
    void targetChanged(QObject* target);

public slots:
    /** */
    void skip(const QJSValue& error);

private:
    ViewEngine*  m_engine;
    QObject* m_target;
    bool     m_componentComplete;
};

inline QObject *ErrorHandler::target() const{
    return m_target;
}

}// namespace

#endif // LVERRORHANDLER_H
