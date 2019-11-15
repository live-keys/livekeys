#include "qmlscopesnap_p.h"
#include "qmllibraryinfo_p.h"

#include "live/visuallogqt.h"

namespace lv{

// QmlScopeSnap::LibraryReference
// -----------------------------------------------------------------------------------------------

QmlScopeSnap::LibraryReference::LibraryReference()
    : versionMajor(-1)
    , versionMinor(-1)
{
}

QmlScopeSnap::LibraryReference::LibraryReference(const QString &libPath, QmlLibraryInfo::Ptr libInfo){
    importUri    = libInfo->importNamespace();
    versionMajor = libInfo->importVersionMajor();
    versionMinor = libInfo->importVersionMinor();
    path         = libPath;
}

bool QmlScopeSnap::LibraryReference::isValid() const {
    return versionMajor > 0 && versionMinor >= 0;
}

// QmlScopeSnap::TypeReference
// -----------------------------------------------------------------------------------------------

QmlScopeSnap::TypeReference::TypeReference(){
}

QmlScopeSnap::TypeReference::TypeReference(const LibraryReference &lib, LanguageUtils::FakeMetaObject::ConstPtr obj)
    : library(lib)
    , object(obj)
{
    if ( object ){
        QList<LanguageUtils::FakeMetaObject::Export> exports = object->exports();
        for ( auto it = exports.begin(); it != exports.end(); ++it ){
            const LanguageUtils::FakeMetaObject::Export& e = *it;
            if ( e.package == library.importUri ){
                typeName = e.type;
                break;
            }
        }
    }
}

bool QmlScopeSnap::TypeReference::isValid() const{
    return library.isValid();
}

QString QmlScopeSnap::TypeReference::toString() const{
    QString result;

    QString ref;
    if ( object.isNull() ){
        int index = qmlFile.lastIndexOf("/");
        if ( index == -1 ){
            ref = "file \'" + qmlFile + "\'";
        } else {
            ref = "file \'" + qmlFile.mid(index) + "\'";
        }
    } else {
        ref = "object " + object->className();
    }

    result = "Type:" + typeName + " in " + ref + " in [\'" + library.importUri + "\' " +
            QString::number(library.versionMajor) + "." + QString::number(library.versionMinor) + "]";

    return result;
}

// QmlScopeSnap::InheritancePath
// -----------------------------------------------------------------------------------------------

QString QmlScopeSnap::InheritancePath::toString() const{
    QString result;

    for ( auto it = nodes.begin(); it != nodes.end(); ++it ){
        result += it->toString() + "\n";
    }

    return result;
}

void QmlScopeSnap::InheritancePath::join(const QmlScopeSnap::InheritancePath &path){
    for ( auto it = path.nodes.begin(); it != path.nodes.end(); ++it ){
        nodes.append(*it);
    }
}

void QmlScopeSnap::InheritancePath::append(const QmlScopeSnap::TypeReference &tr){
    nodes.append(tr);
}

bool QmlScopeSnap::InheritancePath::isEmpty() const{
    return nodes.isEmpty();
}

// QmlScopeSnap::PropertyReference
// -----------------------------------------------------------------------------------------------

QmlScopeSnap::PropertyReference::PropertyReference()
    : property(LanguageUtils::FakeMetaProperty("", "", false, false, false, -1))
    , isPrimitive(false)
    , typeIsClassName(false)
{
}

QmlScopeSnap::PropertyReference::PropertyReference(
        const LanguageUtils::FakeMetaProperty& p,
        bool isClassName,
        const InheritancePath& propertyType,
        const InheritancePath& classType)
    : property(p)
    , isPrimitive(false)
    , typeIsClassName(isClassName)
    , propertyTypePath(propertyType)
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

QmlScopeSnap::TypeReference QmlScopeSnap::getType(const QString &name) const{
    QmlLibraryInfo::Ptr lib = project->implicitLibraries()->libraryInfo(document->path());
    QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
    if ( ev.isValid() ){
        return TypeReference(LibraryReference(document->path(), lib), ev.object);
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(imp.second);
        QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
        if ( ev.isValid() ){
            return TypeReference(LibraryReference(imp.second, lib), ev.object);
        }
    }

    foreach( const QString& defaultLibrary, project->defaultLibraries() ){
        QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(defaultLibrary);
        QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
        if ( ev.isValid() ){
            return TypeReference(LibraryReference(defaultLibrary, lib), ev.object);
        }
    }

    return TypeReference();
}

QmlScopeSnap::InheritancePath QmlScopeSnap::getTypePath(const QString &name) const{
    InheritancePath typePath;

    TypeReference tr = getType(name);
    if ( tr.isValid() ){
        typePath.append(tr);
        typePath.join(generateTypePathFromObject(tr));
    }
    return typePath;
}

QmlScopeSnap::InheritancePath QmlScopeSnap::getTypePath(const QStringList& typeAndImport) const{
    if ( typeAndImport.isEmpty() )
        return InheritancePath();
    if ( typeAndImport.size() == 1 )
        return getTypePath(typeAndImport[0]);
    return getTypePath(typeAndImport[0], typeAndImport[1]);
}

/// Retrieve a type from a specified namespace. If the namespace is empty, the type is searched through
/// implicit libraries and default ones as well. Otherwise, only libraries with the imported namespace are
/// searched.

QmlScopeSnap::TypeReference QmlScopeSnap::getType(const QString &importNamespace, const QString &name) const{
    if ( importNamespace.isEmpty() ){
        QmlLibraryInfo::Ptr lib = project->implicitLibraries()->libraryInfo(document->path());
        QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
        if ( ev.isValid() ){
            return TypeReference(LibraryReference(document->path(), lib), ev.object);
        }

        foreach( const QString& defaultLibrary, project->defaultLibraries() ){
            QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(defaultLibrary);
            QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
            if ( ev.isValid() ){
                return TypeReference(LibraryReference(defaultLibrary, lib), ev.object);
            }
        }
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == importNamespace ){
            QmlLibraryInfo::Ptr lib = project->globalLibraries()->libraryInfo(imp.second);
            QmlLibraryInfo::ExportVersion ev = lib->findExport(name);
            if ( ev.isValid() ){
                return TypeReference(LibraryReference(imp.second, lib), ev.object);
            }
        }
    }

    return TypeReference();
}

QmlScopeSnap::InheritancePath QmlScopeSnap::getTypePath(const QString &importAs, const QString &name) const{
    InheritancePath typePath;

    TypeReference tr = getType(importAs, name);

    if ( tr.isValid() ){
        typePath.append(tr);
        typePath.join(generateTypePathFromObject(tr));
    }

    return typePath;
}

QmlScopeSnap::InheritancePath QmlScopeSnap::generateTypePathFromObject(const QmlScopeSnap::TypeReference &tr) const{
    LanguageUtils::FakeMetaObject::ConstPtr object = tr.object;
    QString typeLibrary = tr.library.path;

    QmlScopeSnap::InheritancePath typePath;

    if ( !object.isNull() && object->superclassName() != "" && object->superclassName() != object->className() ){
        QString typeSuperClass = object->superclassName();
        vlog_debug("editqmljs-codehandler", "Loooking up object \'" + typeSuperClass + "\' from " + typeLibrary);

        // Slider -> Slider (Controls) -> Slider(Controls.Private) -> ... (the reason I can go into a loop is recursive library dependencies)
        // Avoid loop? -> keep track of all library dependencies and dont go back -> super type with the same name cannot be from the same library
        QmlLibraryInfo::Ptr libraryInfo;
        LibraryReference libRef;
        if ( typeLibrary.isEmpty() || typeLibrary == document->path() ){
            libraryInfo = project->implicitLibraries()->libraryInfo(document->path());
            libRef = LibraryReference(document->path(), libraryInfo);
        } else {
            libraryInfo = project->globalLibraries()->libraryInfo(typeLibrary);
            libRef = LibraryReference(typeLibrary, libraryInfo);
        }

        LanguageUtils::FakeMetaObject::ConstPtr superObject = libraryInfo->findObjectByClassName(typeSuperClass);

        if ( superObject.isNull()  ){
            ProjectQmlScopeContainer* globalLibs = project->globalLibraries();
            foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
                QmlLibraryInfo::Ptr currentLib = globalLibs->libraryInfo(libraryDependency);
                superObject = currentLib->findObjectByClassName(typeSuperClass);
                if ( !superObject.isNull() ){
                    libRef = LibraryReference(libraryDependency, currentLib);
                    typeLibrary = libraryDependency;
                    break;
                }
            }

            if ( superObject.isNull() ){
                foreach( const QString& defaultLibrary, project->defaultLibraries() ){
                    QmlLibraryInfo::Ptr currentLib = globalLibs->libraryInfo(defaultLibrary);
                    superObject = currentLib->findObjectByClassName(typeSuperClass);
                    if ( !superObject.isNull() ){
                        libRef = LibraryReference(defaultLibrary, currentLib);
                        typeLibrary = defaultLibrary;
                        break;
                    }
                }
            }
        }

        if ( !superObject.isNull() ){
            TypeReference tr(libRef, superObject);
            typePath.nodes.append(tr);
            typePath.join(generateTypePathFromObject(tr));
        }
    }

    return typePath;
}

QmlScopeSnap::InheritancePath QmlScopeSnap::generateTypePathFromClassName(const QString &typeName, QString typeLibrary) const{
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

    InheritancePath typePath;

    if ( !object.isNull() ){
        TypeReference tr(LibraryReference(typeLibrary, libraryInfo), object);
        typePath.nodes.append(tr);
        typePath.join(generateTypePathFromObject(tr));
    }

    return typePath;
}

QmlScopeSnap::InheritancePath QmlScopeSnap::propertyTypePath(
        const QmlScopeSnap::InheritancePath &classTypePath,
        const QString &propertyName) const
{
    for ( auto it = classTypePath.nodes.begin(); it != classTypePath.nodes.end(); ++it ){
        LanguageUtils::FakeMetaObject::ConstPtr object = it->object;
        for ( int i = 0; i < object->propertyCount(); ++i ){
            if ( object->property(i).name() == propertyName ){
                if ( DocumentQmlInfo::isObject(object->property(i).typeName()) ){
                    return generateTypePathFromClassName(
                        object->property(i).typeName(),
                        it->library.path
                    );
                } else {
                    InheritancePath ip;
                    TypeReference tr;
                    tr.typeName = object->property(i).typeName();
                    ip.nodes.append(tr);
                    return ip;
                }
            }
        }
    }
    return InheritancePath();
}

QmlScopeSnap::PropertyReference QmlScopeSnap::propertyInType(
        const QmlScopeSnap::InheritancePath &classTypePath, const QString &propertyName) const
{
    for ( auto it = classTypePath.nodes.begin(); it != classTypePath.nodes.end(); ++it ){
        LanguageUtils::FakeMetaObject::ConstPtr object = it->object;
        for ( int i = 0; i < object->propertyCount(); ++i ){
            if ( object->property(i).name() == propertyName ){
                if ( DocumentQmlInfo::isObject(object->property(i).typeName()) ){
                    return QmlScopeSnap::PropertyReference(
                        object->property(i),
                        true,
                        generateTypePathFromClassName(
                            object->property(i).typeName(),
                            it->library.path
                        ),
                        classTypePath
                    );
                } else {
                    QmlScopeSnap::PropertyReference pref(
                        LanguageUtils::FakeMetaProperty(
                            object->property(i).name(),
                            DocumentQmlInfo::toQmlPrimitive(object->property(i).typeName()),
                            object->property(i).isList(),
                            object->property(i).isWritable(),
                            object->property(i).isPointer(),
                            object->property(i).revision()
                        ),
                        false,
                        InheritancePath(),
                        classTypePath
                    );
                    pref.isPrimitive = true;
                    return pref;
                }
            }
        }
    }
    return QmlScopeSnap::PropertyReference();
}

LanguageUtils::FakeMetaProperty QmlScopeSnap::getPropertyInObject(
        const QmlScopeSnap::InheritancePath &typePath,
        const QString &propertyName) const
{
    for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
        LanguageUtils::FakeMetaObject::ConstPtr object = it->object;
        for ( int i = 0; i < object->propertyCount(); ++i ){
            if ( object->property(i).name() == propertyName ){
                return object->property(i);
            }
        }
    }

    return LanguageUtils::FakeMetaProperty("", "", false, false, false, -1);
}

QmlScopeSnap::PropertyReference QmlScopeSnap::getProperty(
        const QStringList &contextObject, const QString &propertyName, int position) const
{
    DocumentQmlInfo::ValueReference documentValue = document->info()->valueAtPosition(position);
    if ( !document->info()->isValueNull(documentValue) ){
        DocumentQmlObject valueObject = document->info()->extractValueObject(documentValue);

        for (
            QMap<QString, QString>::const_iterator it = valueObject.memberProperties().begin();
            it != valueObject.memberProperties().end();
            ++it )
        {
            if ( it.key() == propertyName ){
                QString propertyType = it.value();
                bool isPointer = DocumentQmlInfo::isObject(propertyType);

                return QmlScopeSnap::PropertyReference(
                    LanguageUtils::FakeMetaProperty(propertyName, propertyType, false, true, isPointer, 0),
                    false,
                    isPointer ? getTypePath(propertyType) : InheritancePath(),
                    getTypePath(contextObject)
                );
            }
        }
    }

    QmlScopeSnap::InheritancePath contextTypePath = getTypePath(contextObject);

    // find property in object
    for ( auto it = contextTypePath.nodes.begin(); it != contextTypePath.nodes.end(); ++it ){
        LanguageUtils::FakeMetaObject::ConstPtr object =  it->object;
        for ( int i = 0; i < object->propertyCount(); ++i ){
            if ( object->property(i).name() == propertyName ){
                QString propertyType = object->property(i).typeName();

                if ( DocumentQmlInfo::isObject(propertyType) ){
                    bool isPointer = object->property(i).isPointer();

                    return QmlScopeSnap::PropertyReference(
                        object->property(i),
                        true,
                        isPointer ?
                                generateTypePathFromClassName(propertyType, it->library.path) : InheritancePath(),
                        contextTypePath
                    );
                } else {
                    QmlScopeSnap::PropertyReference pref(
                        LanguageUtils::FakeMetaProperty(
                            object->property(i).name(),
                            DocumentQmlInfo::toQmlPrimitive(object->property(i).typeName()),
                            object->property(i).isList(),
                            object->property(i).isWritable(),
                            object->property(i).isPointer(),
                            object->property(i).revision()
                        ),
                        false,
                        InheritancePath(),
                        contextTypePath
                    );
                    pref.isPrimitive = true;
                    return pref;
                }
            }
        }
    }

    return QmlScopeSnap::PropertyReference();
}

// Finds a list of property references within a property chain
// i.e. list.delegate.border.width would look into delegate.border.width
// and output a list of 3 properties with all available information

// The list will be shorter than the chain size if one of the properties cannot be found.

QList<QmlScopeSnap::PropertyReference> QmlScopeSnap::getProperties(
        const QStringList &context, const QStringList &propertyChain, int position) const
{
    if ( propertyChain.isEmpty() )
        return QList<QmlScopeSnap::PropertyReference>();

    QList<QmlScopeSnap::PropertyReference> result;

    QmlScopeSnap::PropertyReference propRef = getProperty(context, propertyChain.first(), position);
    if ( propRef.property.revision() == -1 )
        return result;


    result.append(propRef);

    QmlScopeSnap::InheritancePath typePath = propRef.propertyTypePath;
    if ( typePath.isEmpty() )
        return result;

    for ( int i = 1; i < propertyChain.size(); ++i ){

        QmlScopeSnap::PropertyReference pref = propertyInType(typePath, propertyChain[i]);
        if ( !pref.isValid() ){
            return result;
        }

        typePath = pref.propertyTypePath;

        result.append(pref);

        if ( typePath.isEmpty() )
            return result;
    }

    return result;
}

QList<QmlScopeSnap::PropertyReference> QmlScopeSnap::getProperties(
        const QmlScopeSnap::InheritancePath &typePath, const QStringList &propertyChain) const
{
    if ( propertyChain.isEmpty() )
        return QList<QmlScopeSnap::PropertyReference>();

    QList<QmlScopeSnap::PropertyReference> result;

    QmlScopeSnap::InheritancePath tpath = typePath;

    for ( int i = 0; i < propertyChain.size(); ++i ){

        QmlScopeSnap::PropertyReference pref = propertyInType(tpath, propertyChain[i]);
        if ( !pref.isValid() ){
            return result;
        }

        result.append(pref);

        if ( tpath.isEmpty() )
            return result;

        tpath = pref.propertyTypePath;
    }
    return  result;
}

QmlScopeSnap::ExpressionChain QmlScopeSnap::evaluateExpression(
        const QStringList &contextObject, const QStringList &expression, int position) const
{
    QmlScopeSnap::ExpressionChain result(expression);
    if ( expression.isEmpty() )
        return result;

    // check if is id
    DocumentQmlInfo::ValueReference documentValue = document->info()->valueForId(expression.first());
    if ( !document->info()->isValueNull(documentValue) ){
        DocumentQmlObject valueObj = document->info()->extractValueObject(documentValue);
        QString typeName = document->info()->extractTypeName(documentValue);
        if ( typeName != "" ){
            result.typeReference = getTypePath(typeName);
        }
//        int startSegment = 1;
//        if ( startSegment < context.expressionPath().size() - 1 ){
//            if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
//                QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
//                if ( DocumentQmlInfo::isObject(valueType) ){
//                    typePath = scope.getTypePath(valueType);
//                }
//                ++startSegment;
//            }
//        }

        result.isId = true;
        result.documentValueObject = valueObj;

        QStringList expressionTail = expression;
        expressionTail.removeFirst();

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
        QmlScopeSnap::InheritancePath typeRef = getTypePath("", expression.first());

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
                QmlScopeSnap::InheritancePath typeRef = getTypePath(expression.first(), expressionAfterImport.first());

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

bool QmlScopeSnap::isImport(const QString &name) const{
    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == name ){
            return true;
        }
    }
    return false;
}

bool QmlScopeSnap::isEnum(const QmlScopeSnap::InheritancePath &classTypePath, const QString &name) const{
    for ( auto it = classTypePath.nodes.begin(); it != classTypePath.nodes.end(); ++it ){
        LanguageUtils::FakeMetaObject::ConstPtr object = it->object;

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
    return !property.name().isEmpty();
}

QString QmlScopeSnap::PropertyReference::toString() const{
    QString result;
    if ( property.revision() == -1 || property.name().isEmpty() )
        return "Invalid property";

    result += "Property \'" + property.name() + "\' of type: " + property.typeName();

    if ( !propertyTypePath.isEmpty() )
        result += "\nWith TypePath:\n" + propertyTypePath.toString();
    if ( !classTypePath.isEmpty() )
        result += "From Object:" + classTypePath.nodes.first().toString();

    return result;
}

QString QmlScopeSnap::PropertyReference::toString(const QList<QmlScopeSnap::PropertyReference> &propertyChain){
    QString result;

    for (const QmlScopeSnap::PropertyReference& pr : propertyChain ){
        result += pr.toString() + "\n";
    }

    return result;
}


}// namespace
