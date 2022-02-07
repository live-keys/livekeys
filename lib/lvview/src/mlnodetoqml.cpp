/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#include "mlnodetoqml.h"
#include <QJSValue>
#include <QJSValueIterator>
#include <QJSEngine>
#include <QDateTime>

namespace lv{
namespace ml{

/**
* \brief Simple function that outputs the given MLNode in the form of a Qml object
*
* Since MLNodes can contain other MLNodes, this can be done recursively
* \ingroup lvview
*/
void toQml(const MLNode &n, QJSValue &result, QJSEngine *engine){
    switch( n.type() ){
    case MLNode::Type::Object: {
        result = engine->newObject();
        for ( auto it = n.begin(); it != n.end(); ++it ){
            QJSValue valueResult;
            toQml(it.value(), valueResult, engine);
            result.setProperty(QString::fromStdString(it.key()), valueResult);
        }
        break;
    }
    case MLNode::Type::Array:{
        result = engine->newArray(n.size());
        int index = 0;
        for ( auto it = n.begin(); it != n.end(); ++it ){
            QJSValue valueResult;
            toQml(it.value(), valueResult, engine);
            result.setProperty(index, valueResult);
            ++index;
        }
        break;
    }
    case MLNode::Type::Bytes:{
        result = QString(n.asBytes().toBase64());
        break;
    }
    case MLNode::Type::String:{
        result = QString::fromStdString(n.asString());
        break;
    }
    case MLNode::Type::Boolean:{
        result = n.asBool();
        break;
    }
    case MLNode::Type::Integer:{
        result = n.asInt();
        break;
    }
    case MLNode::Type::Float:{
        result = n.asFloat();
        break;
    }
    default:
        result = QJSValue(QJSValue::NullValue);
        break;
    }
}

/**
* \brief Simple function that converts the given Qml object into a LiveKeys-friendly MLNode shape
* \ingroup lvview
*/
void fromQml(const QJSValue &value, MLNode &n){
    if ( value.isArray() ){
       QJSValueIterator it(value);
       n = MLNode(MLNode::Type::Array);
       while ( it.hasNext() ){
           it.next();
           if ( it.name() != "length" ){
               MLNode result;
               fromQml(it.value(), result);
               n.append(result);
           }
       }
    } else if ( value.isDate() ){
        n = value.toDateTime().toString(Qt::DateFormat::ISODate).toStdString();
    } else if ( value.isObject() ){
        QJSValueIterator it(value);
        n = MLNode(MLNode::Type::Object);
        while ( it.hasNext() ){
            it.next();
            MLNode result;
            fromQml(it.value(), result);
            n[it.name().toStdString()] = result;
        }
    } else if ( value.isString() ){
        n = value.toString().toStdString();
    } else if ( value.isBool() ){
        n = value.toBool();
    } else if ( value.isNumber() ){
        double vd = value.toNumber();
        MLNode::IntType vi = static_cast<MLNode::IntType>(vd);
        if ( vi == vd )
            n = MLNode(vi);
        else
            n = MLNode(vd);
    } else {
        n = MLNode();
    }
}


}// namespace ml
}// namespace
