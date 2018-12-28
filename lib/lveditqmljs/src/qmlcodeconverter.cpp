#include "qmlcodeconverter.h"
#include "live/qmleditfragment.h"
#include "live/projectfile.h"
#include "live/documentqmlinfo.h"
#include "live/codepalette.h"
#include "bindingchannel.h"

#include <QDateTime>
#include <QVariant>
#include <QJSValue>
#include <QJSValueIterator>

namespace lv{

QmlCodeConverter::QmlCodeConverter(QmlEditFragment *edit, QObject *parent)
    : QObject(parent)
    , m_edit(edit)
{
}

QmlCodeConverter::~QmlCodeConverter(){
}

void QmlCodeConverter::writeProperties(const QJSValue &properties){
    if ( !properties.isObject() ){
        qWarning("Properties must be of object type, use 'write' to add code directly.");
        return;
    }

    int valuePosition = m_edit->declaration()->valuePosition();
    int valueLength   = m_edit->declaration()->valueLength();
    QString source    = m_edit->declaration()->document()->content().mid(valuePosition, valueLength);

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(m_edit->declaration()->document()->file()->path());
    if ( !docinfo->parse(source) )
        return;

    lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

    QSet<QString> leftOverProperties;

    QJSValueIterator it(properties);
    while ( it.hasNext() ){
        it.next();
        leftOverProperties.insert(it.name());
    }

    for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
        lv::DocumentQmlValueObjects::RangeProperty* p = *it;
        QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);

        if ( leftOverProperties.contains(propertyName) ){
            source.replace(p->valueBegin, p->end - p->valueBegin, buildCode(properties.property(propertyName)));
            leftOverProperties.remove(propertyName);
        }
    }

    for ( auto it = leftOverProperties.begin(); it != leftOverProperties.end(); ++it ){
        source.insert(source.length() - 1, *it + ": " + buildCode(properties.property(*it)) + "\n");
    }

    m_edit->write(source);
}

void QmlCodeConverter::write(const QJSValue value){
    m_edit->write(buildCode(value));
}

QVariant QmlCodeConverter::parse(){
    QString val = m_edit->readValueText();
    if ( val.length() > 1 && (
             (val.startsWith('"') && val.endsWith('"')) ||
             (val.startsWith('\'') && val.endsWith('\'') )
        ))
    {
        return QVariant(val.mid(1, val.length() - 2));
    } else if ( val == "true" ){
        return QVariant(true);
    } else if ( val == "false" ){
        return QVariant(false);
    } else {
        bool ok;
        qint64 number = val.toLongLong(&ok);
        if (ok)
            return QVariant(number);

        return QVariant(val.toDouble());
    }
    return QVariant();
}

void QmlCodeConverter::updateValue(){
    CodePalette* lp = m_edit->palette();
    BindingPath* mainPath = m_edit->bindingChannel()->expressionPath();

    if ( mainPath->listIndex() == -1 )
        lp->setValueFromBinding(mainPath->property().read());
    if ( m_edit->bindingUse() ){
        m_whenBinding.call();
    }
}

void QmlCodeConverter::updateBindings(){
    if ( m_edit->bindingUse() ){
        m_whenBinding.call();
    }
}

QString QmlCodeConverter::buildCode(const QJSValue &value){
    if ( value.isArray() ){
        QString result;
        QJSValueIterator it(value);
        result = "[";
        bool first = true;
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                if ( first ){
                    first = false;
                } else {
                    result += ",";
                }
                result += buildCode(it.value());
            }
        }
        result += "]";
        return result;
    } else if ( value.isDate() ){
        QDateTime dt = value.toDateTime();

        QString year    = QString::number(dt.date().year()) + ",";
        QString month   = QString::number(dt.date().month()) + ",";
        QString day     = QString::number(dt.date().day()) + ",";
        QString hour    = QString::number(dt.time().hour()) + ",";
        QString minute  = QString::number(dt.time().minute()) + ",";
        QString second  = QString::number(dt.time().second()) + ",";
        QString msecond = QString::number(dt.time().msec());

        return "new Date(" + year + month + day + hour + minute + second + msecond + ")";

    } else if ( value.isObject() ){
        QString result = "{";
        bool first = true;
        QJSValueIterator it(value);
        while ( it.hasNext() ){
            it.next();
            if ( first ){
                first = false;
            } else {
                result += ",";
            }

            result += it.name() + ": " + buildCode(it.value());
        }

        return result + "}";
    } else if ( value.isString() ){
        return "'" + value.toString() + "'";
    } else if ( value.isBool() || value.isNumber() ){
        return value.toString();
    }
    return "";
}


}// namespace
