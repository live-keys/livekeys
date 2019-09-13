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

#ifndef LVQMLJSSETTINGS_H
#define LVQMLJSSETTINGS_H

#include "live/editorsettingscategory.h"
#include <QHash>
#include <QTextCharFormat>

namespace lv{

/// \private
class QmlJsSettings : public EditorSettingsCategory{

public:
    enum ColorComponent{
        Text,
        Comment,
        Number,
        String,
        Operator,
        Identifier,
        Keyword,
        BuiltIn,
        QmlProperty,
        QmlType,
        QmlRuntimeBoundProperty,
        QmlRuntimeModifiedValue,
        QmlEdit
    };

public:
    QmlJsSettings();
    ~QmlJsSettings();

    void fromJson(const QJsonValue &json) Q_DECL_OVERRIDE;
    QJsonValue toJson() const Q_DECL_OVERRIDE;

    bool hasKey(const QString& key);

    QTextCharFormat& operator[](const QString& key);
    QTextCharFormat& operator[](const ColorComponent& key);

    QTextCharFormat operator[](const QString& key) const;
    QTextCharFormat operator[](const ColorComponent& key) const;

    static QHash<QString, ColorComponent>::ConstIterator rolesBegin();
    static QHash<QString, ColorComponent>::ConstIterator rolesEnd();


private:
    QTextCharFormat createFormat(const QColor& foreground);
    QTextCharFormat createFormat(const QColor& foreground, const QColor& background);

    static QHash<QString, ColorComponent> createFormatRoles();
    static QHash<QString, ColorComponent> m_formatRoles;

    QHash<ColorComponent, QTextCharFormat> m_formats;

};

inline QTextCharFormat &QmlJsSettings::operator[](const QString &key){
    return m_formats[m_formatRoles[key]];
}

inline QTextCharFormat &QmlJsSettings::operator[](const QmlJsSettings::ColorComponent &key){
    return m_formats[key];
}

inline QTextCharFormat QmlJsSettings::operator[](const QString &key) const{
    return m_formats[m_formatRoles[key]];
}

inline QTextCharFormat QmlJsSettings::operator[](const QmlJsSettings::ColorComponent &key) const{
    return m_formats[key];
}

inline bool QmlJsSettings::hasKey(const QString &key){
    return m_formatRoles.contains(key);
}

inline QHash<QString, QmlJsSettings::ColorComponent>::ConstIterator QmlJsSettings::rolesBegin(){
    return m_formatRoles.begin();
}

inline QHash<QString, QmlJsSettings::ColorComponent>::ConstIterator QmlJsSettings::rolesEnd(){
    return m_formatRoles.end();
}

inline QTextCharFormat QmlJsSettings::createFormat(const QColor &foreground){
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(foreground));
    return fmt;
}

inline QTextCharFormat QmlJsSettings::createFormat(const QColor &foreground, const QColor &background){
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(foreground));
    fmt.setBackground(QBrush(background));
    return fmt;
}


}// namespace

#endif // LVQMLJSSETTINGS_H
