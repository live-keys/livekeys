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
#include "live/lvliveglobal.h"

namespace lv{

class ScriptCommandLineParser;

///\private
class QmlMain : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QJSValue options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QString version  READ version WRITE setVersion NOTIFY versionChanged)

public:
    explicit QmlMain(QQuickItem* parent = 0);
    ~QmlMain();

    const QJSValue& options() const;
    const QString& version() const;

    void setOptions(const QJSValue& options);
    void setVersion(const QString& version);

protected:
    void componentComplete() Q_DECL_OVERRIDE;

public slots:
    void beforeCompile();
    void afterCompile();

    const QStringList& arguments() const;
    QString option(const QString& key) const;
    bool isOptionSet(const QString& key) const;

signals:
    void optionsChanged();
    void versionChanged();
    void run();

private:
    QJSValue m_options;
    QString  m_version;

    ScriptCommandLineParser* m_parser;
};

inline const QJSValue &QmlMain::options() const{
    return m_options;
}

inline const QString &QmlMain::version() const{
    return m_version;
}

inline void QmlMain::setOptions(const QJSValue& options){
    m_options = options;
    emit optionsChanged();
}

inline void QmlMain::setVersion(const QString &version){
    if (m_version == version)
        return;

    m_version = version;
    emit versionChanged();
}

}// namespace

#endif // QMLMAIN_H
