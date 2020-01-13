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

const std::string QmlJsSettings::formatStrings[] = {
    "Text",
    "Comment",
    "Number",
    "String",
    "Operator",
    "Identifier",
    "Keyword",
    "BuiltIn",
    "QmlProperty",
    "QmlType",
    "QmlRuntimeBoundProperty",
    "QmlRuntimeModifiedValue",
    "QmlEdit"
};

QmlJsSettings::QmlJsSettings(EditorSettings *parent)
    : QObject(parent)
{
    if ( parent )
        connect(parent, &EditorSettings::refresh, this, &QmlJsSettings::__refresh);

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

    MLNode s = parent->readFor("qmljs");
    if ( s.type() == MLNode::Object )
        fromJson(s);
    else
        parent->write("qmljs", toJson());
}

QmlJsSettings::~QmlJsSettings(){
}

void QmlJsSettings::fromJson(const MLNode &obj){
    if ( obj.hasKey("style") ){
        MLNode style = obj["style"];
        for ( auto it = style.begin(); it != style.end(); ++it ){
            QTextCharFormat& fmt = (*this)[QString::fromStdString(it.key())];
            if ( it.value().type() == MLNode::Object ){
                fmt.setBackground(QBrush(QColor(QString::fromStdString(it.value()["background"].asString()))));
                fmt.setForeground(QBrush(QColor(QString::fromStdString(it.value()["foreground"].asString()))));
            } else {
                fmt.clearBackground();
                fmt.setForeground(QBrush(QColor(QString::fromStdString(it.value().asString()))));
            }
        }
    }
}

MLNode QmlJsSettings::toJson() const{
    MLNode root(MLNode::Object);
    MLNode style(MLNode::Object);

    for ( auto it = QmlJsSettings::rolesBegin(); it != QmlJsSettings::rolesEnd(); ++it )
    {
        const QTextCharFormat& fmt = (*this)[it.value()];

        if ( fmt.background().style() != Qt::NoBrush ){
            MLNode highlightob(MLNode::Object);
            highlightob["foreground"] = fmt.foreground().color().name().toStdString();
            highlightob["background"] = fmt.background().color().name().toStdString();
            style[it.key().toStdString()] = highlightob;
        } else {
            style[it.key().toStdString()] = fmt.foreground().color().name().toStdString();
        }
    }

    root["style"] = style;

    return root;
}

void QmlJsSettings::__refresh(){
    EditorSettings* settings = qobject_cast<EditorSettings*>(parent());
    if ( !settings )
        return;

    MLNode s = settings->readFor("qmljs");
    if ( s.type() == MLNode::Object )
        fromJson(s);
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
