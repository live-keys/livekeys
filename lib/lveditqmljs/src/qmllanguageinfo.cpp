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

#include "qmllanguageinfo.h"
#include "qmllanguageinfo_p.h"

#include "live/visuallogqt.h"

namespace lv{

// QmlTypeReference
// ----------------------------------------------------------------------------

const QString &QmlTypeReference::name() const{
    return m_name;
}

const QString &QmlTypeReference::path() const{
    return m_path;
}

QmlTypeReference::Language QmlTypeReference::language() const{
    return m_language;
}

QString QmlTypeReference::languageString() const{
    return m_language == QmlTypeReference::Qml ? "qml" : QmlTypeReference::Cpp ? "cpp" : "u";
}

bool QmlTypeReference::isEmpty() const{
    return m_name.isEmpty();
}

QmlTypeReference::QmlTypeReference(lv::QmlTypeReference::Language lang, const QString &name, const QString &path)
    : m_language(lang)
    , m_path(path)
    , m_name(name)
{
}

QmlTypeReference QmlTypeReference::split(const QString &id){
    int languageSplit = id.indexOf('/');
    int nameSplit     = id.lastIndexOf('#');
    if ( languageSplit != -1 ){
        QString langSeg = id.mid(0, languageSplit);
        QString nameSegment     = id.mid(nameSplit == -1 ? languageSplit + 1 : nameSplit + 1);
        QString pathSegment     = nameSplit == -1 ? "" : id.mid(languageSplit + 1, nameSplit);

        QmlTypeReference::Language lang = langSeg == "cpp"
                ? QmlTypeReference::Cpp
                : langSeg == "qml" ? QmlTypeReference::Qml : QmlTypeReference::Unknown;

        return QmlTypeReference(lang, nameSegment, pathSegment);
    } else {
        return QmlTypeReference(QmlTypeReference::Unknown, id.mid(nameSplit + 1), nameSplit == -1 ? "" : id.mid(0, nameSplit));
    }
}

QString QmlTypeReference::join() const{
    QString p = path();
    return languageString() + "/" + (p.isEmpty() ? "" : p + "#") + name();
}

// QmlFunctionInfo
// ----------------------------------------------------------------------------

QmlFunctionInfo::QmlFunctionInfo()
    : accessType(QmlFunctionInfo::Public)
    , functionType(QmlFunctionInfo::Method)
{
}

QString QmlFunctionInfo::definition() const{
    QString result;
    if ( !returnType.isEmpty() ){
        result += returnType.name();
    }
    result += name + "(";
    for ( auto it = parameters.begin(); it != parameters.end(); ++it ){
        if ( it != parameters.begin() )
            result += ",";
        result += it->second.name();
    }
    return result + ")";
}

bool QmlFunctionInfo::isValid() const{
    return !name.isEmpty();
}

void QmlFunctionInfo::addParameter(const QString &str, const QmlTypeReference &tr){
    parameters.append(QPair<QString, QmlTypeReference>(str, tr));
}

QString lv::QmlFunctionInfo::toString() const{
    QString result;
    result += "Function \'" + name;

    result += "Params: ";
    for ( auto it = parameters.begin(); it != parameters.end(); ++it ){
        result += (it != parameters.begin() ? "," : "") + it->second.join();
    }

    if ( !objectType.isEmpty() )
        result += "From Object:" + objectType.join();

    return result;
}


// QmlTypeInfo
// ----------------------------------------------------------------------------

QmlTypeInfo::QmlTypeInfo()
    : m_d(new QmlTypeInfoPrivate)
{
    m_d->object.reset(new LanguageUtils::FakeMetaObject());
}

QmlTypeInfo::QmlTypeInfo(const lv::QmlTypeInfo &other)
    : m_d(new QmlTypeInfoPrivate)
{
    m_exportType = other.m_exportType;
    m_classType  = other.m_classType;
    m_inherits   = other.m_inherits;
    m_document   = other.m_document;
    m_d->object  = other.m_d->object;
}

QmlTypeInfo::~QmlTypeInfo(){
    delete m_d;
}

QmlTypeInfo &lv::QmlTypeInfo::operator =(const QmlTypeInfo &other){
    m_exportType = other.m_exportType;
    m_classType  = other.m_classType;
    m_inherits   = other.m_inherits;
    m_document   = other.m_document;
    m_d->object  = other.m_d->object;
    return *this;
}

void QmlTypeInfo::setExportType(const QmlTypeReference &exportType){
    m_exportType = exportType;
}

const QmlTypeReference &QmlTypeInfo::exportType() const{
    return m_exportType;
}

const QmlTypeReference &lv::QmlTypeInfo::classType() const{
    return m_classType;
}

const QmlTypeReference &lv::QmlTypeInfo::inherits() const{
    return m_inherits;
}

const QmlTypeReference &QmlTypeInfo::prefereredType() const{
    if ( m_exportType.isEmpty() )
        return m_classType;
    return m_exportType;
}

const QString &QmlTypeInfo::document() const{
    return m_document;
}

int QmlTypeInfo::totalProperties() const{
    return m_d->object->propertyCount();
}

QmlPropertyInfo QmlTypeInfo::propertyAt(int index) const{
    return QmlTypeInfoPrivate::fromMetaProperty(*this, m_d->object->property(index));
}

QmlPropertyInfo QmlTypeInfo::propertyAt(const QString &name) const{
    for ( int i = 0; i < m_d->object->propertyCount(); ++i ){
        if ( m_d->object->property(i).name() == name ){
            return QmlTypeInfoPrivate::fromMetaProperty(*this, m_d->object->property(i));
        }
    }
    return QmlPropertyInfo();
}

int QmlTypeInfo::totalFunctions() const{
    return m_d->object->methodCount();
}

QmlFunctionInfo QmlTypeInfo::functionAt(int index) const{
    return QmlTypeInfoPrivate::fromMetaMethod(*this, m_d->object->method(index));
}

QmlFunctionInfo QmlTypeInfo::functionAt(const QString &name) const{
    for ( int i = 0; i < m_d->object->methodCount(); ++i ){
        if ( m_d->object->method(i).methodName() == name ){
            return QmlTypeInfoPrivate::fromMetaMethod(*this, m_d->object->method(i));
        }
    }
    return QmlFunctionInfo();
}

bool QmlTypeInfo::isValid() const{
    return !m_exportType.isEmpty() || !m_classType.isEmpty();
}

QString QmlTypeInfo::toString() const{
    QString result;

    QString ref;
    if ( !m_document.isEmpty() ){
        int index = m_document.lastIndexOf("/");
        if ( index == -1 ){
            ref = "file \'" + m_document + "\'";
        } else {
            ref = "file \'" + m_document.mid(index) + "\'";
        }
    }

    if ( !m_exportType.isEmpty() ){
        result += "Type (" + m_exportType.join() + ") ";
    }
    if ( !m_classType.isEmpty() ){
        result += "Class (" + m_classType.join() + ") ";
    }
    if ( !m_inherits.isEmpty() ){
        result += "Inherits (" + m_inherits.join() + ") ";
    }

    return result;
}

bool QmlTypeInfo::isDeclaredInQml() const{
    return !m_document.isEmpty();
}

bool QmlTypeInfo::isDeclaredInCpp() const{
    return !isDeclaredInQml();
}

/**
 * \brief Check wether a given type is an object or not
 * \returns true if \p typeString is an object, false otherwise
 */
bool QmlTypeInfo::isObject(const QString &typeString){
if ( typeString == "bool" || typeString == "double" || typeString == "enumeration" ||
     typeString == "int" || typeString == "list" || typeString == "real" ||
     typeString == "color" || typeString == "QColor" ||
     typeString == "string" || typeString == "QString" || typeString == "url" || typeString == "var" || typeString == "QUrl" )
    return false;
return true;
}

QmlTypeReference QmlTypeInfo::toQmlPrimitive(const QmlTypeReference &cppPrimitive){
    if ( cppPrimitive.language() == QmlTypeReference::Cpp ){
        if ( cppPrimitive.name() == "QColor" )
            return QmlTypeReference(QmlTypeReference::Qml, "color");
        else if ( cppPrimitive.name() == "QUrl" )
            return QmlTypeReference(QmlTypeReference::Qml, "url");
        else if ( cppPrimitive.name() == "QString")
            return QmlTypeReference(QmlTypeReference::Qml, "string");
    }
    return cppPrimitive;
}

/**
 * \brief Returns the default value to be assigned for a given qml type
 */
QString QmlTypeInfo::typeDefaultValue(const QString &typeString){
    if ( typeString == "bool" )
        return "false";
    else if ( typeString == "double" || typeString == "int" || typeString == "enumeration" || typeString == "real" )
        return "0";
    else if ( typeString == "list" )
        return "[]";
    else if ( typeString == "string" || typeString == "url " || typeString == "QUrl" || typeString == "QString" )
        return "\"\"";
    else if ( typeString == "color" || typeString == "QColor" )
        return "\"transparent\"";
    else if ( typeString == "point" || typeString == "QPoint" )
        return "\"0x0\"";
    else if ( typeString == "size" || typeString == "QSize" )
        return "\"0x0\"";
    else if ( typeString == "rect" || typeString == "QRect" )
        return "\50,50,100x100\"";
    else if ( typeString == "var" )
        return "undefined";
    else
        return "null";
}


// QmlTypeInfoPrivate
// ----------------------------------------------------------------------------

QmlTypeInfo QmlTypeInfoPrivate::fromMetaObject(LanguageUtils::FakeMetaObject::ConstPtr fmo,
        const QString& libraryUri)
{
    QmlTypeInfo qti;
    qti.m_d->object = fmo;

    QString foundPackage;

    if ( fmo ){
        QList<LanguageUtils::FakeMetaObject::Export> exports = fmo->exports();
        for ( auto it = exports.begin(); it != exports.end(); ++it ){
            const LanguageUtils::FakeMetaObject::Export& e = *it;
            if ( e.package == libraryUri || libraryUri.isEmpty() ){
                qti.m_exportType = QmlTypeReference(QmlTypeReference::Qml, e.type, e.package);
                foundPackage = e.package;
                break;
            }
        }
    }

    qti.m_classType = QmlTypeReference(QmlTypeReference::Cpp, fmo->className(), foundPackage);
    qti.m_inherits = QmlTypeReference(QmlTypeReference::Cpp, fmo->superclassName());

    return qti;
}

QmlFunctionInfo QmlTypeInfoPrivate::fromMetaMethod(
        const QmlTypeInfo& parent,
        const LanguageUtils::FakeMetaMethod& method)
{
    QmlFunctionInfo fi;
    fi.accessType = method.access();
    fi.functionType = method.methodType();
    fi.name = method.methodName();
    fi.objectType = parent.prefereredType();
    if ( !method.returnType().isEmpty() ){
        if ( QmlTypeInfo::isObject(method.returnType()) ){
            fi.returnType = QmlTypeReference(QmlTypeReference::Unknown, method.returnType());
        } else {
            fi.returnType = QmlTypeReference(QmlTypeReference::Qml, method.returnType());
        }
    }

    for ( int i = 0; i < method.parameterNames().size(); ++i ){
        QString pname = method.parameterNames()[i];
        QString ptype = method.parameterTypes()[i];
        if ( QmlTypeInfo::isObject(ptype) ){
            fi.parameters.append(QPair<QString, QmlTypeReference>(
                pname, QmlTypeReference(QmlTypeReference::Unknown, ptype))
            );
        } else {
            fi.parameters.append(QPair<QString, QmlTypeReference>(
                pname, QmlTypeReference(QmlTypeReference::Qml, ptype))
            );
        }
    }

    return fi;
}

QmlPropertyInfo QmlTypeInfoPrivate::fromMetaProperty(const QmlTypeInfo &parent, const LanguageUtils::FakeMetaProperty &prop){
    QmlPropertyInfo qpi(prop.name(), QmlTypeReference(), parent.prefereredType());
    if ( QmlTypeInfo::isObject(prop.typeName()) ){
        qpi.typeName = QmlTypeReference(QmlTypeReference::Unknown, prop.typeName());
    } else {
        qpi.typeName = QmlTypeReference(QmlTypeReference::Qml, prop.typeName());
    }
    qpi.isList = prop.isList();
    qpi.isPointer = prop.isPointer();
    qpi.isWritable = prop.isWritable();
    return qpi;
}

const LanguageUtils::FakeMetaObject::ConstPtr QmlTypeInfoPrivate::typeObject(const QmlTypeInfo &ti){
    return ti.m_d->object;
}

void QmlTypeInfoPrivate::appendProperty(LanguageUtils::FakeMetaObject::Ptr object, const QmlPropertyInfo &prop){
    LanguageUtils::FakeMetaProperty fmp(
        prop.name, prop.typeName.name(), prop.isList, prop.isWritable, prop.isPointer, 0
    );
    object->addProperty(fmp);
}

void QmlTypeInfoPrivate::appendFunction(LanguageUtils::FakeMetaObject::Ptr object, const QmlFunctionInfo &finfo){
    LanguageUtils::FakeMetaMethod fmm(finfo.name);
    fmm.setMethodType(finfo.functionType);
    fmm.setReturnType(finfo.returnType.name());

    for ( auto it = finfo.parameters.begin(); it != finfo.parameters.end(); ++it ){
        fmm.addParameter(it->first, it->second.name());
    }
    object->addMethod(fmm);
}

// QmlInheritanceInfo
// ----------------------------------------------------------------------------

QString QmlInheritanceInfo::toString() const{
    QString result;

    for ( auto it = nodes.begin(); it != nodes.end(); ++it ){
        result += it->toString() + "\n";
    }

    return result;
}

void QmlInheritanceInfo::join(const QmlInheritanceInfo &path){
    for ( auto it = path.nodes.begin(); it != path.nodes.end(); ++it ){
        nodes.append(*it);
    }
}

void QmlInheritanceInfo::append(const QmlTypeInfo &tr){
    nodes.append(tr);
}

bool QmlInheritanceInfo::isEmpty() const{
    return nodes.isEmpty();
}

QmlTypeReference QmlInheritanceInfo::languageType() const{
    if ( isEmpty() )
        return QmlTypeReference();

    return nodes.first().exportType();
}



QmlPropertyInfo::QmlPropertyInfo()
    : isWritable(true)
    , isList(false)
    , isPointer(false)
{
}

QmlPropertyInfo::QmlPropertyInfo(const QString &n, const QmlTypeReference &tn, const QmlTypeReference &object)
    : isWritable(true)
    , isList(false)
    , isPointer(false)
    , name(n)
    , typeName(tn)
    , objectType(object)
{
}

bool QmlPropertyInfo::isValid() const{
    return !name.isEmpty();
}

} // namespace
