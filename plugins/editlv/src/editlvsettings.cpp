#include "editlvsettings.h"

namespace lv{

EditLvSettings::EditLvSettings()
{
    m_formats["text"]            = createFormat("#fff");
    m_formats["comment"]         = createFormat("#56748a");
    m_formats["number"]          = createFormat("#ba761d");
    m_formats["string"]          = createFormat("#86a930");
    m_formats["operator"]        = createFormat("#bc900c");
    m_formats["identifier"]      = createFormat("#93672f");
    m_formats["keyword"]         = createFormat("#a0a000");
    m_formats["variable.builtin"]= createFormat("#93672f");
    m_formats["constant.builtin"]= createFormat("#93672f");
    m_formats["bultin"]          = createFormat("#93672f");
    m_formats["property"]        = createFormat("#ccc");
    m_formats["type"]            = createFormat("#0080a0");
    m_formats["runtimeBound"]    = createFormat("#26539f");
    m_formats["runtimeModified"] = createFormat("#0080a0");
    m_formats["runtimeEdit"]     = createFormat("#fff", "#0b273f");
}


EditLvSettings::~EditLvSettings(){
}

void EditLvSettings::fromJson(const QJsonValue &json){
    QJsonObject obj = json.toObject();
    if ( obj.contains("style") ){
        QJsonObject style = obj["style"].toObject();
        for ( QJsonObject::iterator it = style.begin(); it != style.end(); ++it ){
            QTextCharFormat& fmt = (*this)[it.key().toStdString()];
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

QJsonValue EditLvSettings::toJson() const{
    QJsonObject obj;

    QJsonObject style;
    for ( auto it = m_formats.begin(); it != m_formats.end(); ++it )
    {
        const QTextCharFormat& fmt = it.value();

        if ( fmt.background().style() != Qt::NoBrush ){
            QJsonObject highlightob;
            highlightob["foreground"] = fmt.foreground().color().name();
            highlightob["background"] = fmt.background().color().name();
            style[QString::fromStdString(it.key())] = highlightob;
        } else {
            style[QString::fromStdString(it.key())] = fmt.foreground().color().name();
        }
    }

    obj["style"] = style;

    return obj;
}


}// namespace
