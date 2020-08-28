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

#include "qmljs/parser/qmldirparser_p.h"

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
    if ( m_language == QmlTypeReference::Qml )
        return "qml";
    if ( m_language == QmlTypeReference::Cpp )
        return "cpp";
    return "u";
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
        QString langSeg     = id.mid(0, languageSplit);
        QString nameSegment = id.mid(nameSplit == -1 ? languageSplit + 1 : nameSplit + 1);
        QString pathSegment = nameSplit == -1 ? "" : id.mid(languageSplit + 1, nameSplit - (languageSplit + 1));

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
{
}

QmlTypeInfo::Ptr QmlTypeInfo::create(){
    return QmlTypeInfo::Ptr(new QmlTypeInfo);
}

QmlTypeInfo::Ptr QmlTypeInfo::clone(const QmlTypeInfo::ConstPtr &other){
    QmlTypeInfo::Ptr cl = QmlTypeInfo::Ptr(new QmlTypeInfo);
    cl->m_exportType = other->m_exportType;
    cl->m_classType = other->m_classType;
    cl->m_inherits = other->m_inherits;
    cl->m_document = other->m_document;
    cl->m_isSingleton = other->m_isSingleton;
    cl->m_isCreatable = other->m_isCreatable;
    cl->m_isComposite = other->m_isComposite;
    cl->m_properties = other->m_properties;
    cl->m_methods = other->m_methods;
    cl->m_enums = other->m_enums;
    return cl;
}

QmlTypeInfo::~QmlTypeInfo(){
}

void QmlTypeInfo::setExportType(const QmlTypeReference &exportType){
    m_exportType = exportType;
}

void QmlTypeInfo::setClassType(const QmlTypeReference &classType){
    m_classType = classType;
}

void QmlTypeInfo::setInheritanceType(const QmlTypeReference &inheritsType){
    m_inherits = inheritsType;
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

const QmlDocumentReference &QmlTypeInfo::document() const{
    return m_document;
}

void QmlTypeInfo::setDocument(const QmlDocumentReference &doc){
    m_document = doc;
}

int QmlTypeInfo::totalProperties() const{
    return m_properties.size();
}

QmlPropertyInfo QmlTypeInfo::propertyAt(int index) const{
    return m_properties.at(index);
}

QmlPropertyInfo QmlTypeInfo::propertyAt(const QString &name) const{
    for ( auto prop : m_properties ){
        if ( prop.name == name )
            return prop;
    }
    return QmlPropertyInfo();
}

void QmlTypeInfo::appendProperty(const QmlPropertyInfo &prop){
    m_properties.append(prop);
}

int QmlTypeInfo::totalFunctions() const{
    return m_methods.size();
}

QmlFunctionInfo QmlTypeInfo::functionAt(int index) const{
    return m_methods.at(index);
}

QmlFunctionInfo QmlTypeInfo::functionAt(const QString &name) const{
    for ( auto m : m_methods ){
        if ( m.name == name )
            return m;
    }
    return QmlFunctionInfo();
}

void QmlTypeInfo::appendFunction(const QmlFunctionInfo &function){
    m_methods.append(function);
}

int QmlTypeInfo::totalEnums() const{
    return m_enums.size();
}

QmlEnumInfo QmlTypeInfo::enumAt(int index) const{
    return m_enums.at(index);
}

QmlEnumInfo QmlTypeInfo::enumAt(const QString &name) const{
    for ( auto enumInfo : m_enums ){
        if ( enumInfo.name == name )
            return enumInfo;
    }
    return QmlEnumInfo();
}

bool QmlTypeInfo::isValid() const{
    return !m_exportType.isEmpty() || !m_classType.isEmpty();
}

QString QmlTypeInfo::toString() const{
    QString result;

    QString ref;
    if ( m_document.isValid() ){
        int index = m_document.path.lastIndexOf("/");
        if ( index == -1 ){
            ref = "file \'" + m_document.path + "\'";
        } else {
            ref = "file \'" + m_document.path.mid(index) + "\'";
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
    return m_document.isValid();
}

bool QmlTypeInfo::isDeclaredInCpp() const{
    return !isDeclaredInQml();
}

bool QmlTypeInfo::isSingleton() const{
    return m_isSingleton;
}

bool QmlTypeInfo::isComposite() const{
    return m_isComposite;
}

bool QmlTypeInfo::isCreatable() const{
    return m_isCreatable;
}

/**
 * \brief Check wether a given type is an object or not
 * \returns true if \p typeString is an object, false otherwise
 */
bool QmlTypeInfo::isObject(const QString &typeString){
    if ( typeString == "bool" || typeString == "double" || typeString == "enumeration" ||
         typeString == "int" || typeString == "list" || typeString == "real" ||
         typeString == "color" || typeString == "QColor" ||
         typeString == "string" || typeString == "QString" || typeString == "url" || typeString == "var" || typeString == "QUrl" ||
         typeString == "uint" || typeString == "size" || typeString == "QStringList" || typeString == "QSize")
        return false;
    return true;
}

bool QmlTypeInfo::isQmlBasicType(const QString &typeString){
    if ( typeString == "bool" || typeString == "double" || typeString == "enumeration" ||
         typeString == "int" || typeString == "list" || typeString == "real" ||
         typeString == "color" || typeString == "string" || typeString == "url" || typeString == "var" ||
         typeString == "object" || typeString == "Array" || typeString == "size" || typeString == "QStringList")
        return true;
    return false;
}

QmlTypeReference QmlTypeInfo::toQmlPrimitive(const QmlTypeReference &cppPrimitive){
    if ( cppPrimitive.language() == QmlTypeReference::Cpp ){
        if ( cppPrimitive.name() == "QColor" )
            return QmlTypeReference(QmlTypeReference::Qml, "color");
        else if ( cppPrimitive.name() == "QUrl" )
            return QmlTypeReference(QmlTypeReference::Qml, "url");
        else if ( cppPrimitive.name() == "QString")
            return QmlTypeReference(QmlTypeReference::Qml, "string");
        else if ( cppPrimitive.name() == "QSize")
            return QmlTypeReference(QmlTypeReference::Qml, "size");
        else if ( cppPrimitive.name() == "QPoint" || cppPrimitive.name() == "QPointF" )
            return QmlTypeReference(QmlTypeReference::Qml, "point");
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
        return "\"0,0,0x0\"";
    else if ( typeString == "var" )
        return "undefined";
    else
        return "null";
}


// QmlTypeInfoPrivate
// ----------------------------------------------------------------------------

QmlTypeInfo::Ptr QmlTypeInfoPrivate::fromMetaObject(LanguageUtils::FakeMetaObject::ConstPtr fmo,
        const QString& libraryUri)
{
    QmlTypeInfo::Ptr qti = QmlTypeInfo::create();

    QString foundPackage;

    if ( fmo ){
        QList<LanguageUtils::FakeMetaObject::Export> exports = fmo->exports();
        for ( auto it = exports.begin(); it != exports.end(); ++it ){
            const LanguageUtils::FakeMetaObject::Export& e = *it;
            if ( e.package == libraryUri || libraryUri.isEmpty() ){
                qti->m_exportType = QmlTypeReference(QmlTypeReference::Qml, e.type, e.package);
                foundPackage = e.package;
                break;
            }
        }
    }

    if ( !fmo->className().startsWith("qml/") ){
        qti->m_classType = QmlTypeReference(QmlTypeReference::Cpp, fmo->className(), libraryUri);
    } else {
        qti->m_classType = QmlTypeReference(QmlTypeReference::Qml, fmo->className().mid(4), libraryUri);
    }
    if ( fmo->superclassName().startsWith("qml/") ){
        qti->m_inherits = QmlTypeReference(QmlTypeReference::Unknown, fmo->superclassName().mid(4), "");
    } else {
        qti->m_inherits = QmlTypeReference(QmlTypeReference::Cpp, fmo->superclassName());
    }

    for ( int i = 0; i < fmo->methodCount(); ++i ){
        qti->m_methods.append(QmlTypeInfoPrivate::fromMetaMethod(*qti, fmo->method(i)));
    }
    for ( int i = 0; i < fmo->propertyCount(); ++i ){
        qti->m_properties.append(QmlTypeInfoPrivate::fromMetaProperty(*qti, fmo->property(i)));
    }
    for ( int i = 0; i < fmo->enumeratorCount(); ++i ){
        qti->m_enums.append(QmlTypeInfoPrivate::fromMetaEnum(fmo->enumerator(i)));
    }
    qti->m_isComposite = fmo->isComposite();
    qti->m_isCreatable = fmo->isCreatable();
    qti->m_isSingleton = fmo->isSingleton();

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
        auto typeName = prop.typeName();
        if (typeName == "QString"){
            typeName = "string";
        } else if (typeName == "QUrl"){
            typeName = "url";
        } else if (typeName == "QColor"){
            typeName = "color";
        } else if (typeName == "QFont"){
            typeName = "font";
        } else if (typeName == "QDateTime"){
            typeName = "date";
        } else if (typeName == "QPoint" || typeName == "QPointF"){
            typeName = "point";
        } else if (typeName == "QSize" || typeName == "QSizeF"){
            typeName = "size";
        } else if (typeName == "QRect" || typeName == "QRectF"){
            typeName = "rect";
        } else if (typeName == "QMatrix4x4"){
            typeName = "matrix4x4";
        } else if (typeName == "QQuaternion"){
            typeName = "quaternion";
        } else if (typeName == "QVector2D" || typeName == "QVector3D" || typeName == "QVector4D"){
            typeName = "vector" + typeName[7] + "d";
        } else if (typeName == "uint"){
            typeName = "int";
        }

        qpi.typeName = QmlTypeReference(QmlTypeReference::Qml, typeName);
    }
    qpi.isList = prop.isList();
    qpi.isPointer = prop.isPointer();
    qpi.isWritable = prop.isWritable();
    return qpi;
}

QmlEnumInfo QmlTypeInfoPrivate::fromMetaEnum(const LanguageUtils::FakeMetaEnum &e){
    QmlEnumInfo qei;
    qei.name = e.name();
    qei.keys = e.keys();
    qei.values = e.values();
    return qei;
}

// QmlInheritanceInfo
// ----------------------------------------------------------------------------

QString QmlInheritanceInfo::toString() const{
    QString result;

    for ( auto it = nodes.begin(); it != nodes.end(); ++it ){
        result += (*it)->toString() + "\n";
    }

    return result;
}

void QmlInheritanceInfo::join(const QmlInheritanceInfo &path){
    for ( auto it = path.nodes.begin(); it != path.nodes.end(); ++it ){
        nodes.append(*it);
    }
}

void QmlInheritanceInfo::append(const QmlTypeInfo::Ptr &tr){
    if ( tr )
        nodes.append(tr);
}

bool QmlInheritanceInfo::isEmpty() const{
    return nodes.isEmpty();
}

QmlTypeReference QmlInheritanceInfo::languageType() const{
    if ( isEmpty() )
        return QmlTypeReference();

    return nodes.first()->prefereredType();
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


// QmlLibraryInfo
// -----------------------------------------------------------------------------------------------

QmlLibraryInfo::QmlLibraryInfo()
    : m_importVersionMajor(0)
    , m_importVersionMinor(0)
    , m_status(QmlLibraryInfo::NotScanned)
{
}

QmlLibraryInfo::QmlLibraryInfo(const QmlDirParser &parser)
    : m_importVersionMajor(0)
    , m_importVersionMinor(0)
    , m_status(QmlLibraryInfo::NotScanned)
{
    m_uri = parser.typeNamespace();
}

QmlLibraryInfo::Ptr QmlLibraryInfo::clone(const QmlLibraryInfo::ConstPtr &linfo){
    QmlLibraryInfo::Ptr cl = QmlLibraryInfo::Ptr(new QmlLibraryInfo);
    cl->m_dependencies = linfo->dependencies();
    cl->m_path = linfo->m_path;
    cl->m_uri = linfo->m_uri;
    cl->m_importVersionMajor = linfo->m_importVersionMajor;
    cl->m_importVersionMinor = linfo->m_importVersionMinor;
    cl->m_status = linfo->m_status;

    for ( auto it = linfo->m_exports.begin(); it != linfo->m_exports.end(); ++it ){
        cl->m_exports.insert(it.key(), QmlTypeInfo::clone(it.value()));
    }
    for ( auto it = linfo->m_internals.begin(); it != linfo->m_internals.end(); ++it ){
        cl->m_internals.append(QmlTypeInfo::clone(*it));
    }

    return cl;
}

QmlLibraryInfo::~QmlLibraryInfo(){
}

QStringList QmlLibraryInfo::listExports() const{
    return m_exports.keys();
}

QmlTypeInfo::Ptr QmlLibraryInfo::typeInfoByName(const QString &exportName){
    auto it = m_exports.find(exportName);
    if ( it != m_exports.end() )
        return it.value();
    return nullptr;
}

QmlTypeInfo::Ptr QmlLibraryInfo::typeInfoByClassName(const QString &className){
    for ( auto it = m_exports.begin(); it != m_exports.end(); ++it ){
        QmlTypeInfo::Ptr tr = it.value();
        if ( tr->classType().name() == className )
            return tr;
    }
    for ( auto it = m_internals.begin(); it != m_internals.end(); ++it ){
        QmlTypeInfo::Ptr tr = *it;
        if ( tr->classType().name() == className )
            return tr;
    }
    return nullptr;
}

QmlTypeInfo::Ptr QmlLibraryInfo::typeInfo(const QString &name, QmlTypeReference::Language language){
    if ( language == QmlTypeReference::Cpp ){
        return typeInfoByClassName(name);
    } else if ( language == QmlTypeReference::Qml || language == QmlTypeReference::Unknown ){
        return typeInfoByName(name);
    }
    return nullptr;
}

void QmlLibraryInfo::updateImportInfo(int versionMajor, int versionMinor){
    m_importVersionMajor = versionMajor;
    m_importVersionMinor = versionMinor;
}

void QmlLibraryInfo::addType(const QmlTypeInfo::Ptr &typeExport){
    if ( !typeExport->exportType().isEmpty() ){
        m_exports[typeExport->exportType().name()] = typeExport;
    } else {
        m_internals.append(typeExport);
    }
}

void QmlLibraryInfo::addDependency(const QString &dependency){
    if ( !m_dependencies.contains(dependency) )
        m_dependencies.append(dependency);
}

void QmlLibraryInfo::addDependencies(const QStringList &dependencies){
    for ( const QString& dep : dependencies ){
        addDependency(dep);
    }
}

QmlLibraryInfo::Ptr QmlLibraryInfo::create(const QString &uri){
    QmlLibraryInfo::Ptr c(new QmlLibraryInfo);
    c->m_uri = uri;
    return c;
}

QmlLibraryInfo::Ptr lv::QmlLibraryInfo::create(const QmlDirParser &parser){
    return QmlLibraryInfo::Ptr(new QmlLibraryInfo(parser));
}

const QMap<QString, QmlTypeInfo::Ptr> &QmlLibraryInfo::exports() const{
    return m_exports;
}

QmlLibraryInfo::ScanStatus QmlLibraryInfo::status() const{
    return m_status;
}

void QmlLibraryInfo::setStatus(QmlLibraryInfo::ScanStatus status){
    m_status = status;
}

QString QmlLibraryInfo::statusString() const{
    if ( m_status == QmlLibraryInfo::NotScanned ){
        return "NotScanned";
    } else if ( m_status == QmlLibraryInfo::WaitingOnProcess ){
        return "WaitingOnProcess";
    } else if ( m_status == QmlLibraryInfo::ScanError ){
        return "ScanError";
    } else if ( m_status == QmlLibraryInfo::NoPrototypeLink ){
        return "NoPrototypeLink";
    } else if ( m_status == QmlLibraryInfo::RequiresDependency ){
        return "RequiresDependency";
    } else {
        return "Done";
    }
}

void QmlLibraryInfo::setPath(const QString &path){
    m_path = path;
}

const QString &QmlLibraryInfo::path() const{
    return m_path;
}

const QString &QmlLibraryInfo::uri() const{
    return m_uri;
}

QStringList QmlLibraryInfo::uriSegments() const{
    if ( m_path != m_uri )
        return m_uri.split(".");
    return QStringList() << m_uri;
}

QString QmlLibraryInfo::importStatement() const{
    return m_uri + " " + QString::number(m_importVersionMajor) + "." + QString::number(m_importVersionMinor);
}

int QmlLibraryInfo::importVersionMinor() const{
    return m_importVersionMinor;
}

int QmlLibraryInfo::importVersionMajor() const{
    return m_importVersionMajor;
}

const QList<QString> &QmlLibraryInfo::dependencies() const{
    return m_dependencies;
}

void QmlLibraryInfo::updateUri(const QString &uri){
    m_uri = uri;
}

QString QmlLibraryInfo::toString() const{
    QString result = "Library: " + m_uri;
    result += "\nDependencies: " + m_dependencies.join(",") + "\nExports:\n";

    for ( auto it = m_exports.begin(); it != m_exports.end(); ++it ){
        result += "  " + it.value()->toString() + "\n";
    }

    result += "Internals:\n";
    for ( auto it = m_internals.begin(); it != m_internals.end(); ++it ){
        result += "  " + (*it)->toString() + "\n";
    }

    return result;
}

} // namespace
