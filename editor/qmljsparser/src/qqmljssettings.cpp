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

#include "qqmljssettings.h"
#include "qqmljshighlighter_p.h"

#include <QDebug>

namespace lcv{

QQmlJsSettings::QQmlJsSettings(QQmlJsHighlighter *highlighter)
    : m_highlighter(highlighter)
{
}

QQmlJsSettings::~QQmlJsSettings(){
}

void QQmlJsSettings::fromJson(const QJsonValue &json){;
    QJsonObject obj = json.toObject();
    if ( obj.contains("style") ){
        QJsonObject style = obj["style"].toObject();
        for ( QJsonObject::iterator it = style.begin(); it != style.end(); ++it ){
            QTextCharFormat& fmt = (*m_highlighter)[it.key()];
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
    m_highlighter->rehighlight();
}

QJsonValue QQmlJsSettings::toJson() const{
    QJsonObject obj;

    QJsonObject style;
    for ( QHash<QString, QQmlJsHighlighter::ColorComponent>::ConstIterator it = m_highlighter->rolesBegin();
          it != m_highlighter->rolesEnd();
          ++it
    ){
        QTextCharFormat& fmt = (*m_highlighter)[it.value()];

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

}// namespace
