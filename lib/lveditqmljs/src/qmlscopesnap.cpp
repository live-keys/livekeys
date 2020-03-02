#include "qmlscopesnap_p.h"
#include "qmllibraryinfo_p.h"
#include "live/visuallogqt.h"

namespace lv{

// QmlScopeSnap::PropertyReference
// -----------------------------------------------------------------------------------------------

QmlScopeSnap::PropertyReference::PropertyReference()
    : property()
    , isPrimitive(false)
    , typeIsClassName(false)
{
}

QmlScopeSnap::PropertyReference::PropertyReference(
        const QmlPropertyInfo &propertyInfo,
        bool isClassName,
        const QmlInheritanceInfo &resultType,
        const QmlInheritanceInfo &classType)
    : property(propertyInfo)
    , isPrimitive(false)
    , typeIsClassName(isClassName)
    , resultTypePath(resultType)
    , classTypePath((classType))
{

}

QmlScopeSnap::PropertyReference::PropertyReference(
        const QmlFunctionInfo &p,
        bool isClassName,
        const QmlInheritanceInfo &propertyType,
        const QmlInheritanceInfo &classType)
    : functionInfo(p)
    , isPrimitive(false)
    , typeIsClassName(isClassName)
    , resultTypePath(propertyType)
    , classTypePath((classType))
{
}

// QmlScopeSnap::ExpressionChain
// -----------------------------------------------------------------------------------------------

QmlScopeSnap::ExpressionChain::ExpressionChain(const QStringList &e)
    : expression(e)
    , isClass(false)
    , isId(false)
    , isParent(false)
{
}

bool QmlScopeSnap::ExpressionChain::isValid() const{
    return expression.size() > 0;
}

bool QmlScopeSnap::ExpressionChain::hasNamespace() const{
    return !importAs.isEmpty();
}

bool QmlScopeSnap::ExpressionChain::hasClass() const{
    return isClass;
}

bool QmlScopeSnap::ExpressionChain::hasObject() const{
    return !isClass && !typeReference.isEmpty();
}

bool QmlScopeSnap::ExpressionChain::hasEnum() const{
    return !enumName.isEmpty();
}

bool QmlScopeSnap::ExpressionChain::hasProperty() const{
    return !propertyChain.isEmpty();
}

QmlScopeSnap::ExpressionChain::NodeType QmlScopeSnap::ExpressionChain::lastSegmentType() const{
    if ( hasProperty() ){
        return QmlScopeSnap::ExpressionChain::PropertyNode;
    } else if ( hasEnum() ){
        return QmlScopeSnap::ExpressionChain::EnumNode;
    } else if ( hasObject() ){
        return QmlScopeSnap::ExpressionChain::ObjectNode;
    } else if ( hasClass() ){
        return QmlScopeSnap::ExpressionChain::ClassNode;
    } else if ( hasNamespace() ){
        return QmlScopeSnap::ExpressionChain::NamespaceNode;
    } else
        return QmlScopeSnap::ExpressionChain::Invalid;
}

QString QmlScopeSnap::ExpressionChain::toString(bool detail) const{
    QString resultTitle;
    QString result;

    if ( hasNamespace() ){
        resultTitle += "<namespace>";
        result += "Namespace: " + importAs;
    }
    if ( isId ){
        resultTitle += "<id>";
        result += "\nId:\n" + typeReference.toString();
    } else if ( isParent ){
        resultTitle += "<parent>";
        result += "\nParent:\n" + typeReference.toString();
    } else if ( hasObject() ){
        resultTitle += "<object>";
        result += "\nObject:\n" + typeReference.toString();
    } else if ( isClass ){
        resultTitle += "<class>";
        result += "\nClass:\n" + typeReference.toString();
    }
    if ( hasEnum() ){
        resultTitle += "<enum>";
        result += "\nEnum:" + enumName;
    }
    for ( int i = 0; i < propertyChain.size(); ++i ){
        resultTitle += "<property>";
        result += "\nProperty:\n" + propertyChain[i].toString();
    }
    if (!detail)
        return resultTitle;

    return resultTitle + result;
}

// QmlScopeSnap
// -----------------------------------------------------------------------------------------------

QmlScopeSnap::QmlScopeSnap(const ProjectQmlScope::Ptr &p, const DocumentQmlScope::Ptr &d)
    : project(p)
    , document(d)
{
}

/// Retrieve a type from any available libraries to the document scope

QmlTypeInfo QmlScopeSnap::getType(const QString &name) const{
    QmlLibraryInfo::Ptr lib = project->implicitLibraries()->libraryInfo(document->path());
    QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
    if ( ev.isValid() ){
        return QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(imp.second);
        QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
        if ( ev.isValid() ){
            return QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
        }
    }

    foreach( const QString& defaultLibrary, project->defaultLibraries() ){
        QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(defaultLibrary);
        QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
        if ( ev.isValid() ){
            return QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
        }
    }

    return QmlTypeInfo();
}


/// Retrieve a type from a specified namespace. If the namespace is empty, the type is searched through
/// implicit libraries and default ones as well. Otherwise, only libraries with the imported namespace are
/// searched.

QmlTypeInfo QmlScopeSnap::getType(const QString &importNamespace, const QString &name) const{
    if ( importNamespace.isEmpty() ){
        QmlLibraryInfo::Ptr lib = project->implicitLibraries()->libraryInfo(document->path());
        QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
        if ( ev.isValid() ){
            return QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
        }

        foreach( const QString& defaultLibrary, project->defaultLibraries() ){
            QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(defaultLibrary);
            QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
            if ( ev.isValid() ){
                return QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
            }
        }
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == importNamespace ){
            QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(imp.second);
            QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
            if ( ev.isValid() ){
                return QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
            }
        }
    }

    return QmlTypeInfo();
}

QmlTypeInfo QmlScopeSnap::getType(const QStringList &typeAndImport){
    if ( typeAndImport.isEmpty() )
        return QmlTypeInfo();
    if ( typeAndImport.size() == 1 )
        return getType(typeAndImport[0]);
    return getType(typeAndImport[0], typeAndImport[1]);
}

QmlTypeInfo QmlScopeSnap::getTypeFromContextLibrary(
        const QString &typeName, const QString &libraryUri, QmlTypeReference::Language language) const
{
    QmlLibraryInfo::Reference libref = libraryFromUri(libraryUri);
    QmlLibraryInfo::Ptr libraryInfo = libref.lib;

    LanguageUtils::FakeMetaObject::ConstPtr superObject = libref.lib->findObject(typeName, language);

    if ( !superObject.isNull()  ){
        return QmlTypeInfoPrivate::fromMetaObject(superObject, libraryInfo->importNamespace());
    }

    ProjectQmlScopeContainer* globalLibs = project->globalLibraries();
    foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
        QmlLibraryInfo::Ptr currentLib = globalLibs->libraryInfo(libraryDependency);

        superObject = currentLib->findObject(typeName, language);
        if ( !superObject.isNull() ){
            return QmlTypeInfoPrivate::fromMetaObject(superObject, currentLib->importNamespace());
        }
    }

    foreach( const QString& defaultLibrary, project->defaultLibraries() ){
        QmlLibraryInfo::Ptr currentLib = globalLibs->libraryInfo(defaultLibrary);
        superObject = currentLib->findObject(typeName, language);
        if ( !superObject.isNull() ){
            return QmlTypeInfoPrivate::fromMetaObject(superObject, currentLib->importNamespace());
        }
    }

    return QmlTypeInfo();
}

QmlInheritanceInfo QmlScopeSnap::getTypePath(const QString &importAs, const QString &name) const{
    QmlInheritanceInfo typePath;

    QmlTypeInfo tr = getType(importAs, name);

    if ( tr.isValid() ){
        typePath.append(tr);
        typePath.join(generateTypePathFromObject(tr));
    }

    return typePath;
}

QmlInheritanceInfo QmlScopeSnap::getTypePath(const QString &name) const{
    QmlInheritanceInfo typePath;

    QmlTypeInfo tr = getType(name);
    if ( tr.isValid() ){
        typePath.append(tr);
        typePath.join(generateTypePathFromObject(tr));
    }
    return typePath;
}

QmlInheritanceInfo QmlScopeSnap::getTypePath(const QStringList& typeAndImport) const{
    if ( typeAndImport.isEmpty() )
        return QmlInheritanceInfo();
    if ( typeAndImport.size() == 1 )
        return getTypePath(typeAndImport[0]);
    return getTypePath(typeAndImport[0], typeAndImport[1]);
}

QmlInheritanceInfo QmlScopeSnap::getTypePath(const QmlTypeReference &languageType) const{
    QmlInheritanceInfo typePath;

    if ( languageType.language() == QmlTypeReference::Unknown )
        return getTypePath(languageType.name());

    QmlLibraryInfo::Reference pathAndlib = project->globalLibraries()->libraryInfoByNamespace(languageType.path());
    QmlLibraryInfo::Ptr lib = pathAndlib.lib;

    if ( !lib.isNull() ){
        QmlLibraryInfo::ExportVersion ev;

        if ( languageType.language() == QmlTypeReference::Qml ){
            ev = lib->findExport(languageType.name());
        } else {
            ev = lib->findExportByClassName(languageType.name());
        }

        if ( ev.isValid() ){
            QmlTypeInfo tr = QmlTypeInfoPrivate::fromMetaObject(ev.object, lib->importNamespace());
            typePath.append(tr);
            typePath.join(generateTypePathFromObject(tr));
        }
    }

    return typePath;
}

QmlLibraryInfo::Reference QmlScopeSnap::libraryFromUri(const QString &uri) const{
    QmlLibraryInfo::Reference linfo = project->globalLibraries()->libraryInfoByNamespace(uri);
    if ( linfo.isValid() )
        return linfo;
    return QmlLibraryInfo::Reference(uri, project->implicitLibraries()->libraryInfo(uri));
}

QmlLibraryInfo::Reference QmlScopeSnap::libraryFromType(const QmlTypeReference &tr) const{
    if ( tr.path().isEmpty() )
        return QmlLibraryInfo::Reference();
    return libraryFromUri(tr.path());
}

// resolvefromcontextlibrary

QmlInheritanceInfo QmlScopeSnap::generateTypePathFromObject(const QmlTypeInfo &tr) const{
    LanguageUtils::FakeMetaObject::ConstPtr object = QmlTypeInfoPrivate::typeObject(tr);

    QmlInheritanceInfo typePath;

    if ( !tr.inherits().isEmpty() && tr.inherits().name() != tr.prefereredType().name() ){
        QmlLibraryInfo::Reference ref = libraryFromType(tr.prefereredType());
        if ( !ref.isValid() )
            ref = QmlLibraryInfo::Reference(document->path(), project->implicitLibraries()->libraryInfo(document->path()));

        vlog_debug("editqmljs-codehandler", "Loooking up object \'" + tr.preferredType().name() + "\' from " + ref.path());

        if ( ref.isValid() ){
            QmlTypeInfo ti = getTypeFromContextLibrary(tr.inherits().name(), ref.lib->importNamespace(), tr.inherits().language());
            if ( ti.isValid() ){
                typePath.nodes.append(ti);
                typePath.join(generateTypePathFromObject(ti));
            }
        }
    }

    return typePath;
}

QmlInheritanceInfo QmlScopeSnap::generateTypePathFromClassName(const QString &typeName, QString typeLibrary) const{
    vlog_debug("editqmljs-codehandler", "Looking up type \'" + typeName + "\' from " + typeLibrary);
    QmlLibraryInfo::Ptr libraryInfo = (typeLibrary.isEmpty() || typeLibrary == document->path() )
            ? project->implicitLibraries()->libraryInfo(document->path())
            : project->globalLibraries()->libraryInfo(typeLibrary);

    LanguageUtils::FakeMetaObject::ConstPtr object = libraryInfo->findObjectByClassName(typeName);

    if ( object.isNull() ){
        foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
            libraryInfo = project->globalLibraries()->libraryInfo(libraryDependency);
            object = libraryInfo->findObjectByClassName(typeName);
            if ( !object.isNull() ){
                typeLibrary = libraryDependency;
                break;
            }
        }
    }

    QmlInheritanceInfo typePath;

    if ( !object.isNull() ){
        QmlTypeInfo tr = QmlTypeInfoPrivate::fromMetaObject(object, libraryInfo->importNamespace());
        typePath.nodes.append(tr);
        typePath.join(generateTypePathFromObject(tr));
    }

    return typePath;
}

QmlInheritanceInfo QmlScopeSnap::propertyTypePath(
        const QmlInheritanceInfo &classTypePath,
        const QString &propertyName) const
{
    for ( auto it = classTypePath.nodes.begin(); it != classTypePath.nodes.end(); ++it ){
        const QmlTypeInfo& currentType = *it;
        QmlPropertyInfo prop = currentType.propertyAt(propertyName);
        if ( prop.isValid() ){
            if ( prop.isPointer )
                return generateTypePathFromClassName(prop.typeName.name(), libraryFromUri(prop.typeName.path()).path);
            else {
                QmlInheritanceInfo ip;
                QmlTypeInfo tr;
                tr.setExportType(prop.typeName);
                ip.nodes.append(tr);
                return ip;
            }
        }
    }

    return QmlInheritanceInfo();
}

QmlScopeSnap::PropertyReference QmlScopeSnap::propertyInType(
        const QmlInheritanceInfo &classTypePath, const QString &propertyName) const
{
    for ( auto it = classTypePath.nodes.begin(); it != classTypePath.nodes.end(); ++it ){
        const QmlTypeInfo& currentType = *it;
        QmlPropertyInfo prop = currentType.propertyAt(propertyName);
        if ( prop.isValid() ){
            return QmlScopeSnap::PropertyReference(
                prop,
                prop.typeName.language() == QmlTypeReference::Cpp,
                prop.isPointer ? getTypePath(prop.typeName.name()) : QmlInheritanceInfo(),
                classTypePath
            );
        }

        if ( propertyName.indexOf('(') > 0 ){
            QmlFunctionInfo func = currentType.functionAt(propertyName.mid(0, propertyName.indexOf('(')));
            if ( func.isValid() ){
                return QmlScopeSnap::PropertyReference(
                    func,
                    false,
                    prop.isPointer ? getTypePath(func.returnType.name()) : QmlInheritanceInfo(),
                    classTypePath
                );
            }
        }

    }
    return QmlScopeSnap::PropertyReference();
}

QmlScopeSnap::PropertyReference QmlScopeSnap::getProperty(
        const QmlTypeReference &contextObject, const QString &propertyName, int position) const
{
    DocumentQmlInfo::ValueReference documentValue = document->info()->valueAtPosition(position);

    if ( !document->info()->isValueNull(documentValue) ){
        QmlTypeInfo valueObject = document->info()->extractValueObject(documentValue);
        QmlPropertyInfo prop = valueObject.propertyAt(propertyName);
        if ( prop.isValid() ){
            return QmlScopeSnap::PropertyReference(
                prop,
                false,
                prop.isPointer ? getTypePath(prop.typeName.name()) : QmlInheritanceInfo(),
                getTypePath(contextObject)
            );
        }

        if ( propertyName.indexOf('(') > 0 ){
            QmlFunctionInfo func = valueObject.functionAt(propertyName.mid(0, propertyName.indexOf('(')));
            if ( func.isValid() ){
                return QmlScopeSnap::PropertyReference(
                    func,
                    false,
                    prop.isPointer ? getTypePath(func.returnType.name()) : QmlInheritanceInfo(),
                    getTypePath(contextObject)
                );
            }
        }

    }

    QmlInheritanceInfo contextTypePath = getTypePath(contextObject);

    // find property in object
    for ( auto it = contextTypePath.nodes.begin(); it != contextTypePath.nodes.end(); ++it ){
        const QmlTypeInfo& currentType = *it;
        QmlPropertyInfo prop = currentType.propertyAt(propertyName);
        if ( prop.isValid() ){
            return QmlScopeSnap::PropertyReference(
                prop,
                prop.typeName.language() == QmlTypeReference::Cpp,
                prop.isPointer ? getTypePath(prop.typeName.name()) : QmlInheritanceInfo(),
                contextTypePath
            );
        }
    }

    return QmlScopeSnap::PropertyReference();
}

QmlFunctionInfo QmlScopeSnap::getSignal(
        const QmlTypeReference &contextObject, const QString &signalName, int position) const
{
    DocumentQmlInfo::ValueReference documentValue = document->info()->valueAtPosition(position);
    if ( !document->info()->isValueNull(documentValue) ){
        QmlTypeInfo valueObject = document->info()->extractValueObject(documentValue);
        QmlFunctionInfo fn = valueObject.functionAt(signalName);
        if ( fn.isValid() && fn.functionType == QmlFunctionInfo::Signal ){
            return fn;
        }
    }

    QmlInheritanceInfo contextTypePath = getTypePath(contextObject);

    // find property in object
    for ( auto it = contextTypePath.nodes.begin(); it != contextTypePath.nodes.end(); ++it ){
        const QmlTypeInfo& currentType = *it;
        QmlFunctionInfo fn = currentType.functionAt(signalName);
        if ( fn.isValid() && fn.functionType == QmlFunctionInfo::Signal ){
            return fn;
        }
    }

    return QmlFunctionInfo();
}

// Finds a list of property references within a property chain
// i.e. list.delegate.border.width would look into delegate.border.width
// and output a list of 3 properties with all available information

// The list will be shorter than the chain size if one of the properties cannot be found.

QList<QmlScopeSnap::PropertyReference> QmlScopeSnap::getProperties(
        const QmlTypeReference &context, const QStringList &propertyChain, int position) const
{
    if ( propertyChain.isEmpty() )
        return QList<QmlScopeSnap::PropertyReference>();

    QList<QmlScopeSnap::PropertyReference> result;

    QmlScopeSnap::PropertyReference propRef = getProperty(context, propertyChain.first(), position);
    if ( !propRef.isValid() )
        return result;

    result.append(propRef);

    QmlInheritanceInfo typePath = propRef.resultTypePath;
    if ( typePath.isEmpty() )
        return result;

    for ( int i = 1; i < propertyChain.size(); ++i ){

        QmlScopeSnap::PropertyReference pref = propertyInType(typePath, propertyChain[i]);
        if ( !pref.isValid() ){
            return result;
        }

        typePath = pref.resultTypePath;

        result.append(pref);

        if ( typePath.isEmpty() )
            return result;
    }

    return result;
}

QList<QmlScopeSnap::PropertyReference> QmlScopeSnap::getProperties(
        const QmlInheritanceInfo &typePath, const QStringList &propertyChain) const
{
    if ( propertyChain.isEmpty() )
        return QList<QmlScopeSnap::PropertyReference>();

    QList<QmlScopeSnap::PropertyReference> result;

    QmlInheritanceInfo tpath = typePath;

    for ( int i = 0; i < propertyChain.size(); ++i ){

        QmlScopeSnap::PropertyReference pref = propertyInType(tpath, propertyChain[i]);
        if ( !pref.isValid() ){
            return result;
        }

        result.append(pref);

        if ( tpath.isEmpty() )
            return result;

        tpath = pref.resultTypePath;
    }
    return  result;
}

QmlScopeSnap::ExpressionChain QmlScopeSnap::evaluateExpression(
        const QmlTypeReference &contextObject, const QStringList &expression, int position) const
{
    QmlScopeSnap::ExpressionChain result(expression);
    if ( expression.isEmpty() )
        return result;

    // check if is id
    DocumentQmlInfo::ValueReference documentValue = document->info()->valueForId(expression.first());
    if ( !document->info()->isValueNull(documentValue) ){
        QmlTypeInfo valueObj = document->info()->extractValueObject(documentValue);
        QString typeName = document->info()->extractTypeName(documentValue);
        if ( typeName != "" ){
            result.typeReference = getTypePath(typeName);
        }

        result.isId = true;
        result.documentValueObject = valueObj;

        QStringList expressionTail = expression;
        expressionTail.removeFirst();

        qDebug() << result.typeReference.languageType().join();
        qDebug() << expressionTail;

        result.propertyChain = getProperties(result.typeReference, expressionTail);
        return result;

    // check if is parent
    } else if ( expression.first() == "parent" ){
        result.isParent = true;

        DocumentQmlInfo::ValueReference documentValue = document->info()->valueAtPosition(position);
        if ( document->info()->isValueNull(documentValue) )
            return result;

        DocumentQmlInfo::ValueReference parentValue;
        document->info()->extractValueObject(documentValue, &parentValue);
        if ( document->info()->isValueNull(parentValue) )
            return result;

        result.documentValueObject = document->info()->extractValueObject(parentValue);

        QString typeName = document->info()->extractTypeName(documentValue);
        if ( typeName != "" ){
            result.typeReference = getTypePath(typeName);
        }

        QStringList expressionTail = expression;
        expressionTail.removeFirst();

        result.propertyChain = getProperties(result.typeReference, expressionTail);
        return result;
    } else {

        // check if it's a property
        if ( !contextObject.isEmpty() ){
            result.propertyChain = getProperties(contextObject, expression, position);
            if ( !result.propertyChain.isEmpty() )
                return result;
        }

        // check if it's a type
        QmlInheritanceInfo typeRef = getTypePath("", expression.first());

        if ( !typeRef.isEmpty() ){
            result.typeReference = typeRef;

            QStringList expressionTail = expression;
            expressionTail.removeFirst();

            if ( expressionTail.size() == 1 && isEnum(typeRef, expressionTail.first()) ){
                result.enumName = expressionTail.first();
                result.isClass = true;
                return result;
            } else {
                // in case of singleton
                result.propertyChain = getProperties(typeRef, expressionTail);
            }
        } else if ( isImport(expression.first()) ){

            QStringList expressionAfterImport = expression;
            expressionAfterImport.removeFirst();

            result.importAs = expression.first();

            if ( !expressionAfterImport.isEmpty() ){
                QmlInheritanceInfo typeRef = getTypePath(expression.first(), expressionAfterImport.first());

                if ( !typeRef.isEmpty() ){
                    result.typeReference = typeRef;

                    QStringList expressionTail = expression;
                    expressionTail.removeFirst();

                    result.isClass = true;

                    if ( expressionTail.size() == 1 && isEnum(typeRef, expressionTail.first()) ){
                        result.enumName = expressionTail.first();
                        result.isClass = true;
                        return result;
                    } else {
                        // in case of singleton
                        result.propertyChain = getProperties(typeRef, expressionTail);
                    }
                }
            }

        }
    }

    return result;
}

QString QmlScopeSnap::importToUri(const QString &name) const{
    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == name ){
            return imp.second;
        }
    }
    return "";
}

QmlTypeReference QmlScopeSnap::quickObjectDeclarationType(const QStringList &declaration) const{
    if ( declaration.isEmpty() )
        return QmlTypeReference();
    if ( declaration.size() == 1 ){
        return QmlTypeReference(QmlTypeReference::Unknown, declaration[0]);
    }
    return QmlTypeReference(QmlTypeReference::Qml, declaration[1], importToUri(declaration[0]));
}

bool QmlScopeSnap::isImport(const QString &name) const{
    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == name ){
            return true;
        }
    }
    return false;
}

bool QmlScopeSnap::isEnum(const QmlInheritanceInfo &classTypePath, const QString &name) const{
    for ( auto it = classTypePath.nodes.begin(); it != classTypePath.nodes.end(); ++it ){
        LanguageUtils::FakeMetaObject::ConstPtr object = QmlTypeInfoPrivate::typeObject(*it);

        for ( int i = 0; i < object->enumeratorCount(); ++i ){
            LanguageUtils::FakeMetaEnum e = object->enumerator(i);
            for ( int j = 0; j < e.keyCount(); ++j ){
                if ( e.key(j) == name ){
                    return true;
                }
            }
        }
    }
    return false;
}

bool QmlScopeSnap::PropertyReference::isValid() const{
    return property.isValid() || functionInfo.isValid();
}

QString QmlScopeSnap::PropertyReference::toString() const{
    QString result;
    if ( !isValid() )
        return "Invalid property";

    if ( property.isValid() ){
        result += "Property \'" + property.name + "\' of type: " + property.typeName.join();
    } else {
        result += "Function \'" + functionInfo.name + "\' of return type: " + functionInfo.returnType.join();
    }

    if ( !resultTypePath.isEmpty() )
        result += "\nWith TypePath:\n" + resultTypePath.toString();
    if ( !classTypePath.isEmpty() )
        result += "\nFrom Object:" + classTypePath.nodes.first().toString();

    return result;
}

QString QmlScopeSnap::PropertyReference::toString(const QList<QmlScopeSnap::PropertyReference> &propertyChain){
    QString result;

    for (const QmlScopeSnap::PropertyReference& pr : propertyChain ){
        result += pr.toString() + "\n";
    }

    return result;
}

QmlTypeReference QmlScopeSnap::PropertyReference::resultType() const{
    if ( resultTypePath.isEmpty() ){
        return property.typeName;
    } else {
        return resultTypePath.languageType();
    }
}

QmlTypeReference QmlScopeSnap::PropertyReference::propertyObjectType() const{
    return classTypePath.languageType();
}


}// namespace
