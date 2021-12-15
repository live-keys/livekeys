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

#ifndef QMLMAIN_H
#define QMLMAIN_H

#include <QQuickItem>
#include <QJSValue>

namespace lv{

class QmlScriptArgumentsParser;
class ViewEngine;

///\private
class QmlMain : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QJSValue args    READ args    CONSTANT)
    Q_PROPERTY(QString version  READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QJSValue run     READ run     WRITE setRun     NOTIFY runChanged)

public:
    explicit QmlMain(QQuickItem* parent = 0);
    ~QmlMain();

    const QString& version() const;
    void setVersion(const QString& version);

    const QJSValue& run() const;
    void setRun(const QJSValue& arg);

    const QJSValue& args() const;

protected:
    void classBegin() override;
    void componentComplete() override;

public slots:
    QJSValue parse(const QJSValue& options);
    QString parserHelpString() const;
    void exit(int code);

signals:
    void runChanged();
    void versionChanged();

private:
    QString     m_version;
    QJSValue    m_args;
    QJSValue    m_run;
    ViewEngine* m_engine;

    QmlScriptArgumentsParser* m_parser;
};

inline const QString &QmlMain::version() const{
    return m_version;
}

inline const QJSValue &QmlMain::run() const{
    return m_run;
}

inline const QJSValue &QmlMain::args() const{
    return m_args;
}

}// namespace

#endif // QMLMAIN_H
