#include "editlvsettings.h"

namespace lv{

EditLvSettings::EditLvSettings()
{
    m_formats[EditLvSettings::Text]        = createFormat("#fff");
    m_formats[EditLvSettings::Comment]     = createFormat("#56748a");
    m_formats[EditLvSettings::Number]      = createFormat("#ba761d");
    m_formats[EditLvSettings::String]      = createFormat("#86a930");
    m_formats[EditLvSettings::Operator]    = createFormat("#bc900c");
    m_formats[EditLvSettings::Identifier]  = createFormat("#93672f");
    m_formats[EditLvSettings::Keyword]     = createFormat("#a0a000");
    m_formats[EditLvSettings::BuiltIn]     = createFormat("#93672f");

    m_formats[EditLvSettings::LvProperty] = createFormat("#ccc");
    m_formats[EditLvSettings::LvType]     = createFormat("#0080a0");
    m_formats[EditLvSettings::LvRuntimeBoundProperty] = createFormat("#26539f");
    m_formats[EditLvSettings::LvRuntimeModifiedValue] = createFormat("#0080a0");
    m_formats[EditLvSettings::LvEdit] = createFormat("#fff", "#0b273f");
}

EditLvSettings::~EditLvSettings(){
}

void EditLvSettings::fromJson(const QJsonValue &json){
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

QJsonValue EditLvSettings::toJson() const{
    QJsonObject obj;

    QJsonObject style;
    for ( auto it = EditLvSettings::rolesBegin(); it != EditLvSettings::rolesEnd(); ++it )
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

QHash<QString, EditLvSettings::ColorComponent> EditLvSettings::m_formatRoles = EditLvSettings::createFormatRoles();


QHash<QString, EditLvSettings::ColorComponent> EditLvSettings::createFormatRoles(){
    QHash<QString, EditLvSettings::ColorComponent> formatRoles;
    formatRoles["text"]        = EditLvSettings::Text;
    formatRoles["comment"]     = EditLvSettings::Comment;
    formatRoles["number"]      = EditLvSettings::Number;
    formatRoles["string"]      = EditLvSettings::String;
    formatRoles["operator"]    = EditLvSettings::Operator;
    formatRoles["identifier"]  = EditLvSettings::Identifier;
    formatRoles["keyword"]     = EditLvSettings::Keyword;
    formatRoles["builtin"]     = EditLvSettings::BuiltIn;
    formatRoles["Lvproperty"] = EditLvSettings::LvProperty;
    formatRoles["Lvtype"]     = EditLvSettings::LvType;
    formatRoles["Lvruntimeboundproperty"] = EditLvSettings::LvRuntimeBoundProperty;
    formatRoles["Lvruntimemodifiedvalue"] = EditLvSettings::LvRuntimeModifiedValue;
    formatRoles["Lvedit"] = EditLvSettings::LvEdit;
    return formatRoles;
}



}// namespace
