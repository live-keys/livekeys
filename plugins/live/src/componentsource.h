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

#ifndef COMPONENTSOURCE_H
#define COMPONENTSOURCE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlComponent>

namespace lv{

class Project;

/// \private
class ComponentSource : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlComponent* source READ source WRITE setSource NOTIFY sourceChanged)
    Q_CLASSINFO("DefaultProperty", "source")

public:
    ComponentSource(QObject* parent = nullptr);
    ~ComponentSource();

    void componentComplete();
    void classBegin(){}

    const QString& sourceCode();
    const QString& importSourceCode();

    QQmlComponent* source();
    void setSource(QQmlComponent* source);

signals:
    void sourceChanged();

private:
    void extractSource();

    QQmlComponent*  m_source;
    bool            m_componentComplete;
    QString         m_sourceCode;
    QString         m_importSourceCode;
    lv::Project*    m_project;

};

inline const QString &ComponentSource::sourceCode(){
    extractSource();
    return m_sourceCode;
}

inline const QString &ComponentSource::importSourceCode(){
    extractSource();
    return m_importSourceCode;
}

inline QQmlComponent *ComponentSource::source(){
    return m_source;
}

}// namespace

#endif // COMPONENTSOURCE_H
