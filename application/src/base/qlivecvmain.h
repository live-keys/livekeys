/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QLIVECVMAIN_HPP
#define QLIVECVMAIN_HPP

#include <QQuickItem>
#include <QJSValue>

namespace lcv{

class QLiveCVMain : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QJSValue options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QString version  READ version WRITE setVersion NOTIFY versionChanged)


public:
    QLiveCVMain(QQuickItem* parent = 0);
    ~QLiveCVMain();

    const QJSValue& options() const;
    const QString& version() const;

    void setOptions(const QJSValue& options);
    void setVersion(const QString& version);

public slots:
    void attachWindow(QQuickWindow* window);
    void aboutToRecompile();
    void afterCompile();

signals:
    void optionsChanged();
    void versionChanged();
    void run();


private:
    QJSValue m_options;
    QString  m_version;

    QQuickWindow* m_attachedWindow;
};

inline const QJSValue &QLiveCVMain::options() const{
    return m_options;
}

inline const QString &QLiveCVMain::version() const{
    return m_version;
}

inline void QLiveCVMain::setOptions(const QJSValue& options){
    m_options = options;
    emit optionsChanged();
}

inline void QLiveCVMain::setVersion(const QString &version){
    if (m_version == version)
        return;

    m_version = version;
    emit versionChanged();
}

}// namespace

#endif // QLIVECVMAIN_HPP
