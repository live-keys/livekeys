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

#include "qmljssettings.h"
#include "qmljshighlighter_p.h"

namespace lv{

QmlJsSettings::QmlJsSettings()
{
    m_formats[QmlJsSettings::Text]        = createFormat("#fff");
    m_formats[QmlJsSettings::Comment]     = createFormat("#56748a");
    m_formats[QmlJsSettings::Number]      = createFormat("#ba761d");
    m_formats[QmlJsSettings::String]      = createFormat("#86a930");
    m_formats[QmlJsSettings::Operator]    = createFormat("#bc900c");
    m_formats[QmlJsSettings::Identifier]  = createFormat("#93672f");
    m_formats[QmlJsSettings::Keyword]     = createFormat("#a0a000");
    m_formats[QmlJsSettings::BuiltIn]     = createFormat("#93672f");

    m_formats[QmlJsSettings::QmlProperty] = createFormat("#ccc");
    m_formats[QmlJsSettings::QmlType]     = createFormat("#0080a0");
    m_formats[QmlJsSettings::QmlRuntimeBoundProperty] = createFormat("#26539f");
    m_formats[QmlJsSettings::QmlRuntimeModifiedValue] = createFormat("#0080a0");
    m_formats[QmlJsSettings::QmlEdit] = createFormat("#fff", "#0b273f");
}

QmlJsSettings::~QmlJsSettings(){
}

void QmlJsSettings::fromJson(const QJsonValue &json){
    QJsonObject obj = json.toObject();
    if ( obj.contains("style") ){
        QJsonObject style = obj["style"].toObject();
        for ( QJsonObject::iterator it = style.begin(); it != style.end(); ++it ){
            QTextCharFormat& fmt = (*this)[it.key()];
            if ( it.value().isObject() ){
                QJsonObject styleOb = it.value().toObject();
                fmt.setBackground(QBrush(QColor(styleOb["background"].toString())));
                fmt.setForeground(QBrush(QColor(styleOb["foreground"].toString())));
            } else {
                fmt.clearBackground();
                fmt.setForeground(QBrush(QColor(it.value().toString())));
            }
        }
    }
}

QJsonValue QmlJsSettings::toJson() const{
    QJsonObject obj;

    QJsonObject style;
    for ( auto it = QmlJsSettings::rolesBegin(); it != QmlJsSettings::rolesEnd(); ++it )
    {
        const QTextCharFormat& fmt = (*this)[it.value()];

        if ( fmt.background().style() != Qt::NoBrush ){
            QJsonObject highlightob;
            highlightob["foreground"] = fmt.foreground().color().name();
            highlightob["background"] = fmt.background().color().name();
            style[it.key()] = highlightob;
        } else {
            style[it.key()] = fmt.foreground().color().name();
        }
    }

    obj["style"] = style;

    return obj;
}

QHash<QString, QmlJsSettings::ColorComponent> QmlJsSettings::m_formatRoles = QmlJsSettings::createFormatRoles();

QHash<QString, QmlJsSettings::ColorComponent> QmlJsSettings::createFormatRoles(){
    QHash<QString, QmlJsSettings::ColorComponent> formatRoles;
    formatRoles["text"]        = QmlJsSettings::Text;
    formatRoles["comment"]     = QmlJsSettings::Comment;
    formatRoles["number"]      = QmlJsSettings::Number;
    formatRoles["string"]      = QmlJsSettings::String;
    formatRoles["operator"]    = QmlJsSettings::Operator;
    formatRoles["identifier"]  = QmlJsSettings::Identifier;
    formatRoles["keyword"]     = QmlJsSettings::Keyword;
    formatRoles["builtin"]     = QmlJsSettings::BuiltIn;
    formatRoles["qmlproperty"] = QmlJsSettings::QmlProperty;
    formatRoles["qmltype"]     = QmlJsSettings::QmlType;
    formatRoles["qmlruntimeboundproperty"] = QmlJsSettings::QmlRuntimeBoundProperty;
    formatRoles["qmlruntimemodifiedvalue"] = QmlJsSettings::QmlRuntimeModifiedValue;
    formatRoles["qmledit"] = QmlJsSettings::QmlEdit;
    return formatRoles;
}


}// namespace
