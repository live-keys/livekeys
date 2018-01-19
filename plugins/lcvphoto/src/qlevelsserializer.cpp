#include "qlevelsserializer.h"
#include "qlevels.h"

#include <QJSValueIterator>

#include "live/documentqmlinfo.h"
#include "live/documentqmlfragment.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"

namespace{

QString arrayToCode(const QJSValue& value){
    return "[" +
        value.property(0).toString() + ", " +
        value.property(1).toString() + ", " +
        value.property(2).toString() +
    "]";
}

QString channelsToCode(const QJSValue& value){
    QString result = "{";
    QJSValueIterator it(value);
    while ( it.hasNext() ){
        it.next();

        result += it.name() + ": " + arrayToCode(it.value());
        if ( it.hasNext() )
            result += ",";
    }
    result += "}";

    return result;
}

}


QLevelsSerializer::QLevelsSerializer(QObject *parent) :
    lv::AbstractCodeSerializer(parent)
{

}

QLevelsSerializer::~QLevelsSerializer(){

}

QString QLevelsSerializer::toCode(const QVariant &value, const lv::DocumentEditFragment *channel){
    const lv::DocumentQmlFragment* qmlfragment = static_cast<const lv::DocumentQmlFragment*>(channel);
    int valuePosition = qmlfragment->declaration()->valuePosition();
    int valueLength   = qmlfragment->declaration()->valueLength();
    QString source = qmlfragment->declaration()->document()->content().mid(valuePosition, valueLength);

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(qmlfragment->declaration()->document()->file()->path());
    if ( !docinfo->parse(source) )
        return "";

    lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

    QObject* ob = value.value<QObject*>();
    if ( !ob )
        return "";

    QLevels* lvlob = qobject_cast<QLevels*>(ob);
    if ( !lvlob )
        return "";

    QJSValue lightness = lvlob->lightness();
    QJSValue channels = lvlob->channels();

    for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
        lv::DocumentQmlValueObjects::RangeProperty* p = *it;
        QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);
        if ( propertyName == "lightness" ){
            if ( lightness.isArray() && lightness.property("length").toInt() == 3){
                source.replace(p->valueBegin, p->end - p->valueBegin, arrayToCode(lightness));
            } else {
                source.replace(p->valueBegin, p->end - p->valueBegin, "undefined");
            }
        } else if ( propertyName == "channels" ){
            if ( channels.isObject() ){
                QString code = channelsToCode(channels);
                if ( code.length() > 2 )
                    source.replace(p->valueBegin, p->end - p->valueBegin, channelsToCode(channels));
            } else {
                source.replace(p->valueBegin, p->end - p->valueBegin, "{}");
            }
        }
    }

    return source;
}

QVariant QLevelsSerializer::fromCode(const QString &, const lv::DocumentEditFragment *channel){
    const lv::DocumentQmlFragment* qmlfragment = static_cast<const lv::DocumentQmlFragment*>(channel);
    if ( !qmlfragment )
        return QVariant();

    QQmlListReference ppref = qvariant_cast<QQmlListReference>(qmlfragment->property().read());
    return QVariant::fromValue(ppref.at(qmlfragment->listIndex()));
}
