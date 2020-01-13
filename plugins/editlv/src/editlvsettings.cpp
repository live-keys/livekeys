#include "editlvsettings.h"

namespace lv{

EditLvSettings::EditLvSettings(EditorSettings* parent)
    : QObject(parent)
{
    if ( parent )
        connect(parent, &EditorSettings::refresh, this, &EditLvSettings::__refresh);

    m_formats["text"]            = createFormat("#fff");
    m_formats["comment"]         = createFormat("#56748a");
    m_formats["number"]          = createFormat("#ba761d");
    m_formats["string"]          = createFormat("#86a930");
    m_formats["operator"]        = createFormat("#86a930");
    m_formats["identifier"]      = createFormat("#93672f");
    m_formats["keyword"]         = createFormat("#a0a000");
    m_formats["variable.builtin"]= createFormat("#93672f");
    m_formats["constant.builtin"]= createFormat("#93672f");
    m_formats["builtin"]         = createFormat("#93672f");
    m_formats["property"]        = createFormat("#ccc");
    m_formats["type"]            = createFormat("#0080a0");
    m_formats["runtimeBound"]    = createFormat("#26539f");
    m_formats["runtimeModified"] = createFormat("#0080a0");
    m_formats["runtimeEdit"]     = createFormat("#fff", "#0b273f");

    if ( parent ){
        MLNode s = parent->readFor("lv");
        if ( s.type() == MLNode::Object )
            fromJson(s);
        else
            parent->write("lv", toJson());
    }
}


EditLvSettings::~EditLvSettings(){
}

void EditLvSettings::fromJson(const MLNode &obj){
    if ( obj.hasKey("style") ){
        MLNode style = obj["style"];
        for ( auto it = style.begin(); it != style.end(); ++it ){
            QTextCharFormat& fmt = (*this)[it.key()];
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

MLNode EditLvSettings::toJson() const{
    MLNode root(MLNode::Object);
    MLNode style(MLNode::Object);

    for ( auto it = m_formats.begin(); it != m_formats.end(); ++it ){

        QTextCharFormat fmt = it.value();

        if ( fmt.background().style() != Qt::NoBrush ){
            MLNode highlightob(MLNode::Object);
            highlightob["foreground"] = fmt.foreground().color().name().toStdString();
            highlightob["background"] = fmt.background().color().name().toStdString();
            style[it.key()] = highlightob;
        } else {
            style[it.key()] = fmt.foreground().color().name().toStdString();
        }
    }

    root["style"] = style;

    return root;
}

void EditLvSettings::__refresh(){
    EditorSettings* settings = qobject_cast<EditorSettings*>(parent());
    if ( !settings )
        return;

    MLNode s = settings->readFor("lv");
    if ( s.type() == MLNode::Object )
        fromJson(s);
}


}// namespace
