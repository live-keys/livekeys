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

#include "themecontainer.h"
#include "theme.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

#include <QFileInfo>
#include <QQmlEngine>

namespace lv{

ThemeContainer::ThemeContainer(const QString &type, QObject *parent)
    : QAbstractListModel(parent)
    , m_type(type)
    , m_current(nullptr)
{
    m_roles[ThemeContainer::Label] = "label";
}

ThemeContainer::~ThemeContainer(){
}

int ThemeContainer::rowCount(const QModelIndex &) const{
    return m_themes.size();
}

QVariant ThemeContainer::data(const QModelIndex &index, int role) const{
    if ( index.row() < 0 || index.row() >= m_themes.size() )
        return QVariant();
    auto it = m_themes.constBegin() + index.row();

    if ( role == ThemeContainer::Label ){
        return QVariant::fromValue(it.value());
    }

    return QVariant();
}

QHash<int, QByteArray> ThemeContainer::roleNames() const{
    return m_roles;
}

void ThemeContainer::addTheme(const QString &name, const QString &path){
    if ( name.isEmpty() )
        THROW_EXCEPTION(lv::Exception, "Theme name is empty.", Exception::toCode("theme"));

    auto it = m_themes.find(name);
    if ( it != m_themes.end() ){
        THROW_EXCEPTION(lv::Exception, "Theme already exists: "  + name.toStdString(), Exception::toCode("theme"));
    }

    QFile file(path);
    if ( !file.exists() )
        THROW_EXCEPTION(lv::Exception, "Path doesn't exist: "  + path.toStdString(), Exception::toCode("theme"));

    beginResetModel();
    m_themes[name] = path;
    endResetModel();

    vlog("themes").v() << "Adding theme" << (m_type.isEmpty() ? "" : " for " + m_type) << ": " << name;

    if ( m_current == nullptr )
        setTheme(name);
}

void ThemeContainer::setTheme(const QString &name){
    auto it = m_themes.find(name);
    if ( it == m_themes.end() )
        THROW_EXCEPTION(lv::Exception, "Theme doesn't exist: " + name.toStdString(), Exception::toCode("~Theme"));

    QString themePath = it.value();

    QFile themeFile(themePath);
    if ( !themeFile.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read layer view file:" + themePath.toStdString(), Exception::toCode("~File"));

    QByteArray themeContent = themeFile.readAll();

    ViewEngine* engine = lv::ViewContext::instance().engine();

    QObject* themeObj = engine->createObject(themeContent, this, themePath);
    if ( !themeObj && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    Theme* theme = qobject_cast<Theme*>(themeObj);
    if (!theme)
        THROW_EXCEPTION(
            Exception, "Object is not of theme type in theme: " + name.toStdString(), Exception::toCode("~Theme")
        );

    vlog("themes").v() << "Setting current theme" << (m_type.isEmpty() ? "" : " for " + m_type) << " to " << name;

    Theme* prevTheme = m_current;
    m_current = theme;

    emit currentChanged();

    if ( prevTheme )
        prevTheme->deleteLater();
}

}// namespace
