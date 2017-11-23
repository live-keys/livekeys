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

#include "mlnodetojson.h"
#include "live/exception.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace lv{
namespace ml{

void toJson(const MLNode &n, QJsonValue &result){
    switch( n.type() ){
    case MLNode::Type::Object: {
        QJsonObject jsonobj;
        for ( auto it = n.begin(); it != n.end(); ++it ){
            QJsonValue valueResult;
            toJson(it.value(), valueResult);
            jsonobj[QString::fromStdString(it.key())] = valueResult;
        }
        result = jsonobj;
        break;
    }
    case MLNode::Type::Array:{
        QJsonArray jsonarr;
        for ( auto it = n.begin(); it != n.end(); ++it ){
            QJsonValue valueResult;
            toJson(it.value(), valueResult);
            jsonarr.append(valueResult);
        }
        result = jsonarr;
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
        break;
    }
}

void fromJson(const QJsonValue &value, MLNode &n){
    switch(value.type()){
    case QJsonValue::Object:{
        QJsonObject vo = value.toObject();
        n = MLNode(MLNode::Type::Object);
        for ( auto it = vo.begin(); it != vo.end(); ++it ){
            MLNode result;
            fromJson(*it, result);
            n[it.key().toStdString()] = result;
        }
        break;
    }
    case QJsonValue::Array:{
        QJsonArray va = value.toArray();
        n = MLNode(MLNode::Type::Array);
        for ( auto it = va.begin(); it != va.end(); ++it ){
            MLNode result;
            fromJson(*it, result);
            n.append(result);
        }
        break;
    }
    case QJsonValue::String:{
        n = MLNode(value.toString().toStdString());
        break;
    }
    case QJsonValue::Double:{
        double vd = value.toDouble();
        MLNode::IntType vi = static_cast<MLNode::IntType>(vd);
        if ( vi == vd )
            n = MLNode(vi);
        else
            n = MLNode(vd);
        break;
    }
    case QJsonValue::Bool:{
        n = value.toBool();
        break;
    }
    default:
        n = MLNode();
    }
}

// TODO: These implementations are currently safe but not very performance efficient
// since conversion is made through qt's json structures. They should be replaced
// by string conversions
void toJson(const lv::MLNode &n, QByteArray &result){
    QJsonValue jsonresult;
    toJson(n, jsonresult);
    if ( jsonresult.isObject() ){
        result = QJsonDocument(jsonresult.toObject()).toJson(QJsonDocument::Compact);
    } else if ( jsonresult.isArray() ){
        result = QJsonDocument(jsonresult.toObject()).toJson(QJsonDocument::Compact);
    } else {
        result = jsonresult.toString().toUtf8();
    }
}

void fromJson(const QByteArray &data, MLNode &n){
    QJsonParseError pe;
    QJsonDocument jsondoc = QJsonDocument::fromJson(data, &pe);
    if ( jsondoc.isNull() ){
        THROW_EXCEPTION(lv::Exception, pe.errorString(), 0);
    }
    if ( jsondoc.isArray() )
        fromJson(jsondoc.array(), n);
    else if ( jsondoc.isObject() )
        fromJson(jsondoc.object(), n);
}


}// namespace ml
}// namespace

