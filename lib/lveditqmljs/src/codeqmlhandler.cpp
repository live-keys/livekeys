/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "live/codeqmlhandler.h"
#include "qmljshighlighter_p.h"
#include "qmljssettings.h"
#include "qmljs/qmljsscanner.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"
#include "documentqmlobject.h"
#include "documentqmlobject_p.h"
#include "qmlcompletioncontextfinder.h"
#include "projectqmlscopecontainer_p.h"
#include "qmllibraryinfo_p.h"
#include "plugininfoextractor.h"
#include "plugintypesfacade.h"
#include "documentqmlvaluescanner_p.h"
#include "documentqmlvalueobjects.h"
#include "qmljsbuiltintypes_p.h"
#include "qmljshighlighter_p.h"
#include "bindingchannel.h"
#include "qmlcursorinfo.h"

#include "live/documenthandler.h"
#include "live/codecompletionsuggestion.h"
#include "live/projectfile.h"
#include "live/project.h"
#include "live/editorsettings.h"
#include "live/projectqmlextension.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/editorglobalobject.h"
#include "live/palettecontainer.h"
#include "qmlcodeconverter.h"
#include "qmladdcontainer.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QFileInfo>
#include <QDirIterator>
#include <QTextDocument>
#include <QQmlComponent>
#include <QStringList>
#include <QWaitCondition>

namespace lv{


// QmlScopeSnap
// ----------------------------------------------------------------------------

/// \private
class QmlScopeSnap{
public:
    QmlScopeSnap(const ProjectQmlScope::Ptr& project, const DocumentQmlScope::Ptr& document);

    ProjectQmlScope::Ptr  project;
    DocumentQmlScope::Ptr document;
};


QmlScopeSnap::QmlScopeSnap(const ProjectQmlScope::Ptr &pProject, const DocumentQmlScope::Ptr &pDocument)
    : project(pProject)
    , document(pDocument)
{
}


// CodeQmlHandlerPrivate
// ----------------------------------------------------------------------------

/// \private
class CodeQmlHandlerPrivate{

public:
    DocumentQmlScope::Ptr  documentScope;
    ProjectQmlExtension*   projectHandler;

    QmlScopeSnap snapScope(){ return QmlScopeSnap(projectHandler->scanMonitor()->projectScope(), documentScope); }
};


namespace qmlhandler_helpers{


    /// Retrieve a type from any available libraries to the document scope

    QmlLibraryInfo::ExportVersion getType(
            const QmlScopeSnap& scope,
            const QString& name,
            QString& libraryKey)
    {
        QmlLibraryInfo::ExportVersion ev =
            scope.project->implicitLibraries()->libraryInfo(scope.document->path())->findExport(name);
        if ( ev.isValid() ){
            libraryKey = "";
            return ev;
        }

        foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
            QmlLibraryInfo::ExportVersion ev = scope.project->globalLibraries()->libraryInfo(imp.second)->findExport(name);
            if ( ev.isValid() ){
                libraryKey = imp.second;
                return ev;
            }
        }

        foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
            QmlLibraryInfo::ExportVersion ev = scope.project->globalLibraries()->libraryInfo(defaultLibrary)->findExport(name);
            if ( ev.isValid() ){
                libraryKey = defaultLibrary;
                return ev;
            }
        }

        return QmlLibraryInfo::ExportVersion();
    }

    /// Retrieve a type from a specified namespace. If the namespace is empty, the type is searched through
    /// implicit libraries and default ones as well. Otherwise, only libraries with the imported namespace are
    /// searched.

    QmlLibraryInfo::ExportVersion getType(
            const QmlScopeSnap& scope,
            const QString& importNamespace,
            const QString& name,
            QString& libraryKey)
    {
        if ( importNamespace.isEmpty() ){
            QmlLibraryInfo::ExportVersion ev =
                scope.project->implicitLibraries()->libraryInfo(scope.document->path())->findExport(name);
            if ( ev.isValid() ){
                libraryKey = "";
                return ev;
            }

            foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
                QmlLibraryInfo::ExportVersion ev = scope.project->globalLibraries()->libraryInfo(defaultLibrary)->findExport(name);
                if ( ev.isValid() ){
                    libraryKey = defaultLibrary;
                    return ev;
                }
            }
        }

        foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
            if ( imp.first.as() == importNamespace ){
                QmlLibraryInfo::ExportVersion ev = scope.project->globalLibraries()->libraryInfo(imp.second)->findExport(name);
                if ( ev.isValid() ){
                    libraryKey = imp.second;
                    return ev;
                }
            }
        }

        return QmlLibraryInfo::ExportVersion();
    }

    void generateTypePathFromObject(
        const QmlScopeSnap& scope,
        LanguageUtils::FakeMetaObject::ConstPtr& object,
        QString typeLibrary,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath
    ){
        if ( !object.isNull() && object->superclassName() != "" && object->superclassName() != object->className() ){
            QString typeSuperClass = object->superclassName();
            vlog_debug("editqmljs-codehandler", "Loooking up object \'" + typeSuperClass + "\' from " + typeLibrary);

            // Slider -> Slider (Controls) -> Slider(Controls.Private) -> ... (the reason I can go into a loop is recursive library dependencies)
            // Avoid loop? -> keep track of all library dependencies and dont go back -> super type with the same name cannot be from the same library
            QmlLibraryInfo::Ptr libraryInfo = typeLibrary == ""
                    ? scope.project->implicitLibraries()->libraryInfo(scope.document->path())
                    : scope.project->globalLibraries()->libraryInfo(typeLibrary);

            LanguageUtils::FakeMetaObject::ConstPtr superObject = libraryInfo->findObjectByClassName(typeSuperClass);

            if ( superObject.isNull()  ){
                ProjectQmlScopeContainer* globalLibs = scope.project->globalLibraries();
                foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
                    superObject = globalLibs->libraryInfo(libraryDependency)->findObjectByClassName(typeSuperClass);
                    if ( !superObject.isNull() ){
                        typeLibrary = libraryDependency;
                        break;
                    }
                }

                if ( superObject.isNull() ){
                    foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
                        superObject = globalLibs->libraryInfo(defaultLibrary)->findObjectByClassName(typeSuperClass);
                        if ( !superObject.isNull() ){
                            typeLibrary = defaultLibrary;
                            break;
                        }
                    }
                }
            }

            if ( !superObject.isNull() ){
                typePath.append(superObject);
                generateTypePathFromObject(scope, superObject, typeLibrary, typePath);
            }
        }
    }

    void generateTypePathFromClassName(
        const QmlScopeSnap& scope,
        const QString& typeName,
        QString typeLibrary,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath
    ){
        vlog_debug("editqmljs-codehandler", "Looking up type \'" + typeName + "\' from " + typeLibrary);
        QmlLibraryInfo::Ptr libraryInfo = typeLibrary == ""
                ? scope.project->implicitLibraries()->libraryInfo(scope.document->path())
                : scope.project->globalLibraries()->libraryInfo(typeLibrary);

        LanguageUtils::FakeMetaObject::ConstPtr object = libraryInfo->findObjectByClassName(typeName);

        if ( object.isNull() ){
            foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
                object = scope.project->globalLibraries()->libraryInfo(libraryDependency)->findObjectByClassName(typeName);
                if ( !object.isNull() ){
                    typeLibrary = libraryDependency;
                    break;
                }
            }
        }

        if ( !object.isNull() ){
            typePath.append(object);
            generateTypePathFromObject(scope, object, typeLibrary, typePath);
        }
    }

    void getTypePath(
        const QmlScopeSnap& scope,
        const QString& importAs,
        const QString& name,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey
    ){
        QmlLibraryInfo::ExportVersion ev = getType(scope, importAs, name, libraryKey);

        if ( ev.isValid() ){
            typePath.append(ev.object);

            qmlhandler_helpers::generateTypePathFromObject(
                scope,
                ev.object,
                libraryKey,
                typePath
            );
        }
    }

    void getTypePath(
        const QmlScopeSnap& scope,
        const QString& name,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey
    ){
        QmlLibraryInfo::ExportVersion ev = getType(scope, name, libraryKey);

        if ( ev.isValid() ){
            typePath.append(ev.object);

            qmlhandler_helpers::generateTypePathFromObject(
                scope,
                ev.object,
                libraryKey,
                typePath
            );
        }
    }

    void evaluatePropertyClass(
        const QmlScopeSnap& scope,
        const QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        const QString& typeLibraryKey,
        const QString& propertyName,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& propertyTypePath
    ){
        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, typePath ){
            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( object->property(i).name() == propertyName ){
                    if ( object->property(i).isPointer() ){
                        generateTypePathFromClassName(
                            scope,
                            object->property(i).typeName(),
                            typeLibraryKey,
                            propertyTypePath
                        );
                    }
                    return;
                }
            }
        }
    }

    /**
     * @brief Checks wether value is a valid id in the given document scope.
     * Outputs the object path and object value if true.
     */
    bool isId(
        const QmlScopeSnap& scope,
        const QString& str,
        DocumentQmlInfo::ValueReference& documentValue,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& typeLibraryKey
    ){
        documentValue = scope.document->info()->valueForId(str);
        if ( scope.document->info()->isValueNull(documentValue) )
            return false;

        QString typeName = scope.document->info()->extractTypeName(documentValue);
        if ( typeName != "" ){
            getTypePath(scope, typeName, typePath, typeLibraryKey);
        }

        return true;
    }

    /**
     * @brief Checks wether value is a parent in the given document scope.
     * Outputs the object path and value if true.
     */
    bool isParent(
        const QmlScopeSnap& scope,
        const QString& str,
        int position,
        DocumentQmlInfo::ValueReference& parentValue,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& typeLibraryKey
    ){
        if ( str != "parent" )
            return false;

        DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(position);
        if ( scope.document->info()->isValueNull(documentValue) )
            return true;

        scope.document->info()->extractValueObject(documentValue, &parentValue);
        if ( scope.document->info()->isValueNull(parentValue) )
            return true;

        QString typeName = scope.document->info()->extractTypeName(parentValue);
        if ( typeName != "" ){
            getTypePath(scope, typeName, typePath, typeLibraryKey);
        }

        return true;
    }


    LanguageUtils::FakeMetaProperty getPropertyInObject(
        const QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        const QString& propertyName
    )
    {
        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, typePath ){
            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( object->property(i).name() == propertyName ){
                    return object->property(i);
                }
            }
        }
        return LanguageUtils::FakeMetaProperty("", "", false, false, false, -1);
    }


    /**
     * @brief Finds the property propertyName for the contextObject in the document
     * @return
     */
    LanguageUtils::FakeMetaProperty getPropertyInDocument(
        const QmlScopeSnap& scope,
        const QStringList &contextObject,
        const QString &propertyName,
        int propertyPosition,
        bool& isClassName,
        QString &contextObjectLibrary)
    {
        DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(propertyPosition);
        if ( !scope.document->info()->isValueNull(documentValue) ){
            DocumentQmlObject valueObject = scope.document->info()->extractValueObject(documentValue);

            for (
                QMap<QString, QString>::const_iterator it = valueObject.memberProperties().begin();
                it != valueObject.memberProperties().end();
                ++it )
            {
                if ( it.key() == propertyName ){
                    isClassName = false;
                    return LanguageUtils::FakeMetaProperty(
                        it.key(),
                        it.value(),
                        false,
                        true,
                        DocumentQmlInfo::isObject(it.value()), 0
                    );
                }
            }
        }

        QString type = contextObject.size() > 0 ? contextObject[0] : "";
        if ( type == "" )
            return LanguageUtils::FakeMetaProperty("", "", false, false, false, -1);;
        QString typeNamespace = contextObject.size() > 1 ? contextObject[1] : "";

        QList<LanguageUtils::FakeMetaObject::ConstPtr> contextTypePath;
        getTypePath(scope, typeNamespace, type, contextTypePath, contextObjectLibrary);

        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, contextTypePath ){
            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( object->property(i).name() == propertyName ){
                    isClassName = true;
                    return object->property(i);
                }
            }
        }
        return LanguageUtils::FakeMetaProperty("", "", false, false, false, -1);
    }

    /**
     * @brief Finds the property in the propertyChain for the contextObject in the document
     * @return
     */
    LanguageUtils::FakeMetaProperty getPropertyInDocument(
        const QmlScopeSnap& scope,
        const QStringList &contextObject,
        const QStringList &propertyChain,
        int propertyPosition,
        bool& isClassName,
        QString &contextObjectLibrary)
    {
        if ( propertyChain.isEmpty() )
            return LanguageUtils::FakeMetaProperty("", "", false, false, false, -1);

        LanguageUtils::FakeMetaProperty property(qmlhandler_helpers::getPropertyInDocument(
             scope,
             contextObject,
             propertyChain[0],
             propertyPosition,
             isClassName,
             contextObjectLibrary
        ));

        for ( int i = 1; i < propertyChain.size(); ++i ){
            if ( property.revision() == -1 )
                return property;

            QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
            if ( property.isPointer() ){
                if ( isClassName ){
                    qmlhandler_helpers::generateTypePathFromClassName(
                        scope, property.typeName(), contextObjectLibrary, typePath
                    );
                } else {
                    qmlhandler_helpers::getTypePath(
                        scope, property.typeName(), typePath, contextObjectLibrary
                    );
                    isClassName = true;
                }

                property = qmlhandler_helpers::getPropertyInObject(typePath, propertyChain[i]);

            // if the property is not of pointer type, there cannot be anymore chaining after it
            } else {
                return LanguageUtils::FakeMetaProperty("", "", false, false, false, -1);
            }
        }

        return property;
    }

    /**
     * @brief Checks wether str is a property for the context object.
     */
    bool getProperty(
        const QmlScopeSnap& scope,
        const QStringList& contextObject,
        const QString& str,
        int position,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        bool& isPointer,
        QString& libraryKey
    ){
        DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(position);
        if ( !scope.document->info()->isValueNull(documentValue) ){
            DocumentQmlObject valueObject = scope.document->info()->extractValueObject(documentValue);

            for (
                QMap<QString, QString>::const_iterator it = valueObject.memberProperties().begin();
                it != valueObject.memberProperties().end();
                ++it )
            {
                if ( it.key() == str ){
                    QString propertyType = it.value();
                    isPointer = DocumentQmlInfo::isObject(propertyType);
                    if ( isPointer ){
                        getTypePath(scope, propertyType, typePath, libraryKey);
                    }
                    return true;
                }
            }
        }

        QString type = contextObject.size() > 0 ? contextObject[0] : "";
        if ( type == "" )
            return false;
        QString typeNamespace = contextObject.size() > 1 ? contextObject[1] : "";

        QList<LanguageUtils::FakeMetaObject::ConstPtr> contextTypePath;
        getTypePath(scope, typeNamespace, type, contextTypePath, libraryKey);

        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, contextTypePath ){
            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( object->property(i).name() == str ){
                    QString propertyType = object->property(i).typeName();
                    isPointer = object->property(i).isPointer();
                    if ( isPointer ){
                        generateTypePathFromClassName(scope, propertyType, libraryKey, typePath);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief isType
     */
    bool isType(
        const QmlScopeSnap& scope,
        const QString& importAs,
        const QString& str,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey)
    {
        QmlLibraryInfo::ExportVersion ev = getType(
            scope, importAs, str, libraryKey
        );

        if ( ev.isValid() ){
            typePath.append(ev.object);

            qmlhandler_helpers::generateTypePathFromObject(
                scope,
                ev.object,
                libraryKey,
                typePath
            );
            return true;
        }
        return false;
    }

    /**
     * @brief isNamespace
     */
    bool isNamespace(DocumentQmlScope::Ptr documentScope, const QString& str){
        foreach( const DocumentQmlScope::ImportEntry& imp, documentScope->imports() ){
            if ( imp.first.as() == str ){
                return true;
            }
        }
        return false;
    }


    /**
     * @brief Create suggestions from an object type path
     */
    void suggestionsForObjectPath(
        const QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        bool suggestProperties,
        bool suggestMethods,
        bool suggestSignals,
        bool suggestEnums,
        bool suggestGeneratedSlots,
        const QString& suffix,
        QList<CodeCompletionSuggestion>& suggestions
    ){
        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, typePath ){
            QList<CodeCompletionSuggestion> localSuggestions;

            QString objectTypeName = "";
            if ( object->exports().size() > 0 )
                objectTypeName = object->exports().first().type;

            if ( suggestProperties ){
                for ( int i = 0; i < object->propertyCount(); ++i ){
                    if ( !object->property(i).name().startsWith("__") ){
                        localSuggestions << CodeCompletionSuggestion(
                            object->property(i).name(),
                            object->property(i).typeName(),
                            objectTypeName,
                            object->property(i).name() + (object->property(i).isPointer() ? "" : suffix)
                        );
                    }
                }
            }
            for ( int i = 0; i < object->methodCount(); ++i ){
                LanguageUtils::FakeMetaMethod m = object->method(i);
                if ( (m.methodType() == LanguageUtils::FakeMetaMethod::Method ||
                      m.methodType() == LanguageUtils::FakeMetaMethod::Slot) && suggestMethods ){
                    QString completion = m.methodName() + "(";// + m.parameterNames().join(", ") + ")";
                    localSuggestions << CodeCompletionSuggestion(
                        m.methodName() + "()", "function", objectTypeName, completion
                    );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestSignals ){
                    QString completion = m.methodName() + "(" + m.parameterNames().join(", ") + ")";
                    localSuggestions << CodeCompletionSuggestion(
                        m.methodName() + "()", "signal", objectTypeName, completion
                    );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestGeneratedSlots ){
                    QString methodName = m.methodName();
                    if ( methodName.size() > 0 )
                        methodName[0] = methodName[0].toUpper();
                    localSuggestions << CodeCompletionSuggestion(
                        "on" + methodName, "slot", objectTypeName, "on" + methodName + suffix
                    );
                }
            }
            if ( suggestEnums ){
                for ( int i = 0; i < object->enumeratorCount(); ++i ){
                    LanguageUtils::FakeMetaEnum e = object->enumerator(i);
                    for ( int j = 0; j < e.keyCount(); ++j ){
                        localSuggestions << CodeCompletionSuggestion(
                            e.key(j),
                            e.name(),
                            objectTypeName,
                            e.key(j)
                        );
                    }
                }
            }
            if ( suggestGeneratedSlots ){
                for ( int i = 0; i < object->propertyCount(); ++i ){
                    if ( !object->property(i).name().startsWith("__") ){
                        QString propertyName = object->property(i).name();
                        if ( propertyName.size() > 0 )
                            propertyName[0] = propertyName[0].toUpper();

                        localSuggestions << CodeCompletionSuggestion(
                            "on" + propertyName + "Changed",
                            "slot",
                            objectTypeName,
                            "on" + propertyName + "Changed" + suffix
                        );
                    }
                }
            }

            std::sort(localSuggestions.begin(), localSuggestions.end(), &CodeCompletionSuggestion::compare);
            suggestions << localSuggestions;
        }
    }
}

// CodeQmlHandler implementation
// ----------------------------------------------------------------------------

/**
 * \class lv::CodeQmlHandler
 * \ingroup lveditqmljs
 * \brief Main code handler for the qml extension.
 *
 *
 * Handles code completion, palette control, property and item searching.
 */

/**
 * \brief CodeQmlHandler constructor
 *
 * This is called through the ProjectQmlHandler.
 */
CodeQmlHandler::CodeQmlHandler(
        ViewEngine *engine,
        Project *,
        QmlJsSettings *settings,
        ProjectQmlExtension *projectHandler,
        DocumentHandler *handler)
    : AbstractCodeHandler(handler)
    , m_target(0)
    , m_highlighter(new QmlJsHighlighter(settings, handler, 0))
    , m_settings(settings)
    , m_engine(engine->engine())
    , m_completionContextFinder(new QmlCompletionContextFinder)
    , m_newScope(false)
    , m_editingFragment(nullptr)
    , d_ptr(new CodeQmlHandlerPrivate)
{
    Q_D(CodeQmlHandler);
    d->projectHandler = projectHandler;

    m_scopeTimer.setInterval(1000);
    m_scopeTimer.setSingleShot(true);
    connect(&m_scopeTimer, SIGNAL(timeout()), this, SLOT(updateScope()));

    d->projectHandler->addCodeQmlHandler(this);
    d->projectHandler->scanMonitor()->addScopeListener(this);
}

/**
 * \brief CodeQmlHandler destructor
 */
CodeQmlHandler::~CodeQmlHandler(){
    Q_D(CodeQmlHandler);
    cancelEdit();

    if ( d->projectHandler ){
        d->projectHandler->removeCodeQmlHandler(this);
        d->projectHandler->scanMonitor()->removeScopeListener(this);
    }

    delete m_completionContextFinder;
}

/**
 * \brief Implementation of code completion assist.
 *
 * Handles bracket and paranthesis auto-completion together with suggestions population.
 */
void CodeQmlHandler::assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange)
{
    if ( !m_target )
        return;

    if ( insertion != QChar() && !manuallyTriggered ){
        if ( insertion == '{' ){
            cursorChange = cursor;
            cursorChange.beginEditBlock();
            cursorChange.insertText("}");
            cursorChange.endEditBlock();
            cursorChange.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
            return;
        } else if ( insertion.category() == QChar::Separator_Line || insertion.category() == QChar::Separator_Paragraph){
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Up);
            QString text = cursorChange.block().text();
            QString indent = "";
            for ( QString::const_iterator it = text.begin(); it != text.end(); ++it ){
                if ( !it->isSpace() )
                    break;
                indent += *it;
            }
            bool isOpenBrace = false;
            for ( QString::const_reverse_iterator it = text.rbegin(); it != text.rend(); ++it ){
                if ( *it == QChar('{') )
                    isOpenBrace = true;
                if ( !it->isSpace() )
                    break;
            }
            cursorChange.movePosition(QTextCursor::Down);
            cursorChange.movePosition(QTextCursor::StartOfLine);

            if ( isOpenBrace ){
                text = cursorChange.block().text();
                if ( text.trimmed().startsWith("}") ){
                    cursorChange.beginEditBlock();
                    cursorChange.insertText(indent + "    \n" + indent);
                    cursorChange.endEditBlock();
                    cursorChange.movePosition(QTextCursor::Up);
                    cursorChange.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
                    return;
                }
                indent += "    ";
            }

            cursorChange.beginEditBlock();
            cursorChange.insertText(indent);
            cursorChange.endEditBlock();
            return;
        } else if ( insertion == '\"' ) {
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Left);
            QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
            if ( !(ctx->context() & QmlCompletionContext::InStringLiteral) ){
                cursorChange.beginEditBlock();
                cursorChange.insertText("\"");
                cursorChange.endEditBlock();
            } else {
                cursorChange.movePosition(QTextCursor::Right);
            }
            return;
        } else if ( insertion == '\'' ) {
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Left);
            QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
            if ( !(ctx->context() & QmlCompletionContext::InStringLiteral) ){
                cursorChange.beginEditBlock();
                cursorChange.insertText("\'");
                cursorChange.endEditBlock();
            } else {
                cursorChange.movePosition(QTextCursor::Right);
            }
            return;

        } else if ( insertion != QChar('.') && !insertion.isLetterOrNumber() ){
            model->disable();
            return;
        }
    } else if ( !manuallyTriggered ){
        model->disable();
        return;
    }

    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);
    QString filter = ctx->expressionPath().size() > 0 ? ctx->expressionPath().last() : "";

    model->setCompletionPosition(
        cursor.position() -
        (ctx->expressionPath().size() > 0 ? ctx->expressionPath().last().length() : 0)
    );

    if( filter == "" && insertion.isNumber() ){
        model->setFilter(insertion);
        return;
    }
    if ( model->completionContext() ){
        QmlCompletionContext* modelCtx = static_cast<QmlCompletionContext*>(model->completionContext());
        if ( modelCtx && *modelCtx == *ctx && !m_newScope ){
            model->setFilter(filter);
            if ( model->rowCount() )
                model->enable();
            return;
        }
        model->removeCompletionContext();
    }
    m_newScope = false;

    QList<CodeCompletionSuggestion> suggestions;
    if ( ctx->context() & QmlCompletionContext::InStringLiteral ){
        if ( ctx->context() & QmlCompletionContext::InImport ){
            suggestionsForStringImport(extractQuotedString(cursor), suggestions, filter);
            model->setSuggestions(suggestions, filter);
        } else {
            model->setSuggestions(suggestions, filter);
        }
    } else if ( ctx->context() & QmlCompletionContext::InImport ){
        if ( ctx->context() & QmlCompletionContext::InImportVersion ){
            model->setSuggestions(suggestions, filter);
        } else {
            suggestionsForImport(*ctx, suggestions);
            model->setSuggestions(suggestions, filter);
        }
    } else if ( ctx->context() & QmlCompletionContext::InAfterOnLhsOfBinding ){
        suggestionsForLeftSignalBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ){
        suggestionsForLeftBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding ){
        suggestionsForRightBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else {
        suggestionsForGlobalQmlContext(*ctx, suggestions);
        suggestionsForNamespaceTypes(ctx->expressionPath().size() > 1 ? ctx->expressionPath().first() : "", suggestions);
        model->setSuggestions(suggestions, filter);
    }

    if ( model->rowCount() )
        model->enable();
}

/**
 * \brief Assints the document that will be used by this class.
 */
void CodeQmlHandler::setDocument(ProjectDocument *document){
    Q_D(CodeQmlHandler);
    m_document      = document;
    m_target        = document->textDocument();
    m_highlighter->setTarget(m_target);
    d->documentScope = DocumentQmlScope::createEmptyScope(d->projectHandler->scanMonitor()->projectScope());

    if ( m_document ){
        auto it = m_edits.begin();
        while( it != m_edits.end() ){
            QmlEditFragment* edit = *it;
            it = m_edits.erase(it);
            edit->emitRemoval();
            edit->deleteLater();
        }
    }

    if ( d->projectHandler->scanMonitor()->hasProjectScope() && document != 0 ){
        d->projectHandler->scanMonitor()->scanNewDocumentScope(document->file()->path(), document->content(), this);
        d->projectHandler->scanner()->queueProjectScan();
    }
}

/**
 * \brief DocumentContentsChanged handler
 */
void CodeQmlHandler::documentContentsChanged(int position, int, int){
    if ( !m_document->editingStateIs(ProjectDocument::Silent) ){
        if ( m_editingFragment ){
            if ( position < m_editingFragment->valuePosition() ||
                 position > m_editingFragment->valuePosition() + m_editingFragment->valueLength() )
            {
                cancelEdit();
            }
        }

        m_scopeTimer.start();
    }
}

/**
 * \brief Called when a new scope is available from the scanMonitor
 */
void CodeQmlHandler::updateScope(){
    Q_D(CodeQmlHandler);
    if ( d->projectHandler->scanMonitor()->hasProjectScope() && m_document )
        d->projectHandler->scanMonitor()->queueNewDocumentScope(m_document->file()->path(), m_document->content(), this);
}

void CodeQmlHandler::rehighlightSection(int start, int end){
    QTextBlock bl = m_target->findBlock(start);
    while ( bl.isValid() ){
        rehighlightBlock(bl);
        if (bl.position() > end )
            break;
        bl = bl.next();
    }
}

void CodeQmlHandler::resetProjectQmlExtension(){
    Q_D(CodeQmlHandler);
    d->projectHandler = nullptr;
}

/**
 * \brief Calls a new rehighlight on the specified block
 */
void CodeQmlHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_highlighter ){
        m_highlighter->rehighlightBlock(block);
    }
}

/**
 * \brief Get a list of declarations from a specific cursor
 */
QList<QmlDeclaration::Ptr> CodeQmlHandler::getDeclarations(const QTextCursor& cursor){
    Q_D(CodeQmlHandler);

    QList<QmlDeclaration::Ptr> properties;
    int length = cursor.selectionEnd() - cursor.selectionStart();

    if ( length == 0 ){
        QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

        QStringList expression;
        int propertyPosition = ctx->propertyPosition();
        int propertyLength   = 0;

        QChar expressionEndDelimiter;

        if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ){
            expression = ctx->expressionPath();

            if ( propertyPosition == -1 ){
                if ( m_target->characterAt(cursor.position()).isSpace() ){
                    return properties;
                }
                else
                    propertyPosition = cursor.position();
            }

            int advancedLength = DocumentQmlValueScanner::getExpressionExtent(
                m_target, cursor.position(), &expression, &expressionEndDelimiter
            );
            propertyLength = (cursor.position() - propertyPosition) + advancedLength;

        } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding ){
            expression     = ctx->propertyPath();
            propertyLength = DocumentQmlValueScanner::getExpressionExtent(m_target, ctx->propertyPosition());
        }

        if ( expression.size() > 0 ){
            if ( expressionEndDelimiter == QChar('{') ){ // dealing with an object declaration ( 'Object{' )

                properties.append(QmlDeclaration::create(
                    QStringList(),
                    expression.last(),
                    QStringList(),
                    propertyPosition,
                    0,
                    m_document
                ));

            } else { // dealing with a property declaration
                QmlScopeSnap scope = d->snapScope();
                bool isClassName = false;
                QString typeLibraryKey;
                LanguageUtils::FakeMetaProperty property(qmlhandler_helpers::getPropertyInDocument(
                     scope,
                     ctx->objectTypePath(),
                     expression,
                     cursor.position(),
                     isClassName,
                     typeLibraryKey
                ));

                // If property is retrieved by class name (eg. QQuickItem), convert it to its export name(Item)
                QString propertyType;

                if ( isClassName && !typeLibraryKey.isEmpty() ){
                    QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
                    qmlhandler_helpers::generateTypePathFromClassName(
                        scope, property.typeName(), typeLibraryKey, typePath
                    );

                    if ( typePath.size() > 0 ){
                        QList<LanguageUtils::FakeMetaObject::Export> exports = typePath.first()->exports();
                        if ( exports.size() > 0 )
                            propertyType = exports.first().type;
                    } else {
                        propertyType = QmlJsBuiltinTypes::nameFromCpp(property.typeName());
                    }
                }

                if ( propertyType == "" ){
                    propertyType = property.typeName();
                }

                if ( property.revision() != -1 ){
                    properties.append(QmlDeclaration::create(
                        expression, propertyType, ctx->objectTypePath(), propertyPosition, propertyLength, m_document
                    ));
                }
            }
        }

    } else { // multiple properties were selected

        DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
        docinfo->parse(m_target->toPlainText());

        DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

        QList<DocumentQmlValueObjects::RangeProperty*> rangeProperties = objects->propertiesBetween(
            cursor.selectionStart(), cursor.selectionEnd()
        );

        if ( rangeProperties.size() > 0 ){

            QmlScopeSnap scope = d->snapScope();

            for( QList<DocumentQmlValueObjects::RangeProperty*>::iterator it = rangeProperties.begin();
                 it != rangeProperties.end();
                 ++it )
            {
                DocumentQmlValueObjects::RangeProperty* rp = *it;
                QString propertyType = rp->type();

                if ( propertyType.isEmpty() ){

                    bool isClassName = false;
                    QString typeLibraryKey;
                    LanguageUtils::FakeMetaProperty property(qmlhandler_helpers::getPropertyInDocument(
                         scope,
                         rp->object(),
                         rp->name(),
                         rp->begin,
                         isClassName,
                         typeLibraryKey
                    ));

                    // If property is retrieved by class name (eg. QQuickItem), convert it to its export name(Item)
                    if ( isClassName && !typeLibraryKey.isEmpty() ){
                        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
                        qmlhandler_helpers::generateTypePathFromClassName(
                            scope, property.typeName(), typeLibraryKey, typePath
                        );
                        if ( typePath.size() > 0 ){
                            QList<LanguageUtils::FakeMetaObject::Export> exports = typePath.first()->exports();
                            if ( exports.size() > 0 )
                                propertyType = exports.first().type;
                        }
                    }

                    if ( propertyType == "" ){
                        propertyType = property.typeName();
                    }
                }

                if ( !propertyType.isEmpty() ){
                    //TODO: Find parentType
                    properties.append(QmlDeclaration::create(
                        rp->name(), propertyType, QStringList(), rp->begin, rp->propertyEnd - rp->begin, m_document
                    ));
                }
            }
        }
    }

    return properties;
}

/**
 * \brief Given a declaration identifier \p position and \p lenght, get the \p valuePosition and \p valueEnd
 * for that declaration
 */
bool CodeQmlHandler::findDeclarationValue(int position, int length, int &valuePosition, int &valueEnd){
    if ( length > 0 ){
        DocumentQmlValueScanner vs(m_document, position, length);
        if ( vs() ){
            valuePosition = vs.valuePosition();
            valueEnd      = vs.valueEnd();
            return true;
        } else {
            valuePosition = -1;
            valueEnd      = -1;
            return false;
        }
    } else {
        DocumentQmlValueScanner vs(m_document, position, length);
        int valueLength = vs.getBlockExtent(position);
        if ( valueLength != -1 ){
            valuePosition = position;
            valueEnd      = valueLength + position;
            return true;
        } else {
            valuePosition = -1;
            valueEnd      = -1;
            return false;
        }
    }
}

/**
 * \brief Creates an injection channel between a declaration and the runtime
 *
 * This method will find all binding channels available by parsing the contents
 * of the code structure, creating a set of binding pahts that describe where
 * each component resides, and use those binding paths to connect the position of the
 * code structure to the same values within the runtime.
 */
QmlEditFragment *CodeQmlHandler::createInjectionChannel(
        QmlDeclaration::Ptr declaration,
        QObject *runtime)
{
    if ( m_document && m_document->isActive() ){

        BindingPath* bp = DocumentQmlInfo::findDeclarationPath(m_target->toPlainText(), m_document, declaration);
        if ( !bp )
            return nullptr;

        DocumentQmlInfo::traverseBindingPath(bp, runtime);
        if ( !bp->hasConnection() ){
            delete bp;
            return nullptr;
        }

        QmlEditFragment* ef = new QmlEditFragment(declaration);
        ef->bindingChannel()->setExpressionPath(bp);
        return ef;
    }

    return nullptr;
}

/**
 * \brief Returns the block starting position and end position
 */
QPair<int, int> CodeQmlHandler::contextBlock(int position){
    DocumentQmlValueScanner vs(m_document, position, 0);
    int start = vs.getBlockStart(position);
    int end   = vs.getBlockEnd(start + 1);
    return QPair<int, int>(start, end);
}

/**
 * \brief Adds an editing fragment to the current document
 */
bool CodeQmlHandler::addEditingFragment(QmlEditFragment *edit){
    auto it = m_edits.begin();
    while ( it != m_edits.end() ){
        QmlEditFragment* itEdit = *it;

        if ( itEdit->declaration()->position() < edit->declaration()->position() ){
            break;

        } else if ( itEdit->declaration()->position() == edit->declaration()->position() ){
            qWarning("Multiple editing fragments at the same position: %d", edit->position());
            return false;
        }
        ++it;
    }

    m_edits.insert(it, edit);

    return true;
}

/**
 * \brief Removes an editing fragment from this document
 */
void CodeQmlHandler::removeEditingFragment(QmlEditFragment *edit){
    if (!edit)
        return;

    QObject* parent = edit->parent();
    if ( parent == this ){
        auto it = m_edits.begin();

        while( it != m_edits.end() ){
            QmlEditFragment* itEdit = *it;

            if ( itEdit->declaration()->position() < edit->declaration()->position() )
                break;

            if ( itEdit == edit ){
                m_edits.erase(it);

                if ( m_editingFragment == edit ){
                    m_document->removeEditingState(ProjectDocument::Overlay);
                    m_editingFragment = nullptr;
                }
                edit->emitRemoval();
                edit->deleteLater();
                return;
            }

            ++it;
        }
    } else {
        QmlEditFragment* parentEf = static_cast<QmlEditFragment*>(parent);
        if ( parentEf ){
            parentEf->removeChildFragment(edit);
        }
    }
}

QmlEditFragment *CodeQmlHandler::findEditFragment(CodePalette *palette){
    auto it = m_edits.begin();
    while( it != m_edits.end() ){
        QmlEditFragment* edit = *it;
        for ( auto it = edit->begin(); it != edit->end(); ++it ){
            CodePalette* cp = *it;
            if ( cp == palette )
                return edit;
        }

        QmlEditFragment* recurseEdit = findEditFragmentIn(edit, palette);
        if ( recurseEdit )
            return recurseEdit;

        ++it;

    }
    return nullptr;
}

QmlEditFragment *CodeQmlHandler::findEditFragmentIn(QmlEditFragment* parent, CodePalette *palette){
    for ( auto it = parent->childFragments().begin(); it != parent->childFragments().end(); ++it ){
        QmlEditFragment* edit = *it;
        for ( auto paIt = edit->begin(); paIt != edit->end(); ++paIt ){
            if ( *paIt == palette )
                return edit;
        }

        QmlEditFragment* childEdit = findEditFragmentIn(edit, palette);
        if ( childEdit )
            return childEdit;
    }
    return nullptr;
}

QmlEditFragment *CodeQmlHandler::openConnection(int position, QObject* currentApp){
    if ( !m_document )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    if ( properties.isEmpty() )
        return nullptr;

    QmlDeclaration::Ptr declaration = properties.first();

    for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
        QmlEditFragment* edit = *it;
        if ( edit->declaration()->position() == declaration->position() )
            return edit;
    }

    QmlEditFragment* ef = createInjectionChannel(declaration, currentApp);
    if ( !ef ){
        return nullptr;
    }

    QTextCursor codeCursor(m_target);
    codeCursor.setPosition(ef->valuePosition());
    codeCursor.setPosition(ef->valuePosition() + ef->valueLength(), QTextCursor::KeepAnchor);

    ef->declaration()->setSection(m_document->createSection(
        QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
    ));
    ef->declaration()->section()->setUserData(ef);
    ef->declaration()->section()->onTextChanged(
                [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
    {
        auto projectDocument = section->document();
        auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditingFragment(editingFragment);
        } else {
            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        }
    });

    BindingPath* mainPath = ef->bindingChannel()->expressionPath();
    if ( mainPath->listIndex() == -1 ){
        mainPath->property().connectNotifySignal(ef, SLOT(updateValue()));
    }

    addEditingFragment(ef);
    ef->setParent(this);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

QmlEditFragment *CodeQmlHandler::openNestedConnection(QmlEditFragment* editParent, int position, QObject *currentApp){
    if ( !m_document || !editParent )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    if ( properties.isEmpty() )
        return nullptr;

    QmlDeclaration::Ptr declaration = properties.first();

    for ( auto it = editParent->childFragments().begin(); it != editParent->childFragments().end(); ++it ){
        QmlEditFragment* edit = *it;
        if ( edit->declaration()->position() == declaration->position() )
            return edit;
    }

    QmlEditFragment* ef = createInjectionChannel(declaration, currentApp);
    if ( !ef ){
        return nullptr;
    }

    QTextCursor codeCursor(m_target);
    codeCursor.setPosition(ef->valuePosition());
    codeCursor.setPosition(ef->valuePosition() + ef->valueLength(), QTextCursor::KeepAnchor);

    ef->declaration()->setSection(m_document->createSection(
        QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
    ));
    ef->declaration()->section()->setUserData(ef);
    ef->declaration()->section()->onTextChanged(
                [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
    {
        auto projectDocument = section->document();
        auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditingFragment(editingFragment);
        } else {
            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        }
    });

    BindingPath* mainPath = ef->bindingChannel()->expressionPath();
    if ( mainPath->listIndex() == -1 ){
        mainPath->property().connectNotifySignal(ef, SLOT(updateValue()));
    }

    editParent->addChildFragment(ef);
    ef->setParent(editParent);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

void CodeQmlHandler::removeConnection(QmlEditFragment *edit){
    removeEditingFragment(edit);
}

/**
 * \brief Finds the available list of palettes at the current \p cursor position
 *
 * The \p unrepeated parameter makes sure the palettes that are found at that
 * \p position are not already opened.
 */
lv::PaletteList* CodeQmlHandler::findPalettes(int position, bool unrepeated){
    Q_D(CodeQmlHandler);
    if ( !m_document )
        return nullptr;

    cancelEdit();

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    if ( properties.isEmpty() )
        return nullptr;

    QmlDeclaration::Ptr declaration = properties.first();

    PaletteList* lpl = d->projectHandler->paletteContainer()->findPalettes("qml/" + declaration->type());
    lpl->setPosition(declaration->position());
    if ( unrepeated ){
        for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
            QmlEditFragment* edit = *it;
            if ( edit->declaration()->position() < declaration->position() ){
                break;
            } else if ( edit->declaration()->position() == declaration->position()  ){

                for ( auto it = edit->begin(); it != edit->end(); ++it ){
                    CodePalette* loadedPalette = *it;

                    for ( int i = 0; i < lpl->size(); ++i ){
                        PaletteLoader* loader = lpl->loaderAt(i);
                        if ( PaletteContainer::palettePath(loader) == loadedPalette->path() ){
                            lpl->remove(loader);
                        }
                    }
                }

            }
        }
    }
    return lpl;
}

/**
 * \brief Opens the palette \p index from a given \p paletteList
 */
lv::CodePalette* CodeQmlHandler::openPalette(lv::QmlEditFragment* edit, lv::PaletteList *paletteList, int index){
    if ( !m_document || !edit || !paletteList )
        return nullptr;

    // check if duplicate
    PaletteLoader* paletteLoader = paletteList->loaderAt(index);

    for ( auto it = edit->begin(); it != edit->end(); ++it ){
        CodePalette* loadedPalette = *it;
        if ( loadedPalette->path() == PaletteContainer::palettePath(paletteLoader) ){
            return loadedPalette;
        }
    }

    if ( edit->bindingPalette() && edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) ){
        edit->addPalette(edit->bindingPalette());
        return edit->bindingPalette();
    }

    CodePalette* palette = paletteList->loadAt(index);
    QmlCodeConverter* cvt = new QmlCodeConverter(edit, palette);
    palette->setExtension(cvt, true);
    edit->addPalette(palette);
    edit->updatePaletteValue(palette);


    connect(palette, &CodePalette::valueChanged, cvt, &QmlCodeConverter::updateFromPalette);

    rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(edit->valuePosition());

    return palette;
}

bool CodeQmlHandler::isForAnObject(lv::QmlEditFragment *ef){
    return DocumentQmlInfo::isObject(ef->declaration()->type());
}

/**
 * \brief Removes a palette given its container object.
 */
lv::QmlEditFragment *CodeQmlHandler::removePalette(lv::CodePalette *palette){
    if ( !palette )
        return nullptr;

    lv::QmlEditFragment* edit = findEditFragment(palette);
    if ( edit ){
        edit->removePalette(palette);
    }
    return edit;
}

/**
 * \brief Open a specific binding palette for \p edit fragment given a \p paletteList and an \p index
 *
 * \returns A pointer to the opened lv::CodePalette.
 */
CodePalette *CodeQmlHandler::openBinding(QmlEditFragment *edit, PaletteList *paletteList, int index){
    if ( !m_document || !edit || !paletteList )
        return nullptr;

    // check if duplicate
    PaletteLoader* paletteLoader = paletteList->loaderAt(index);
    if ( edit->bindingPalette() ){
        if ( edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) )
            return edit->bindingPalette();
    }
    CodePalette* cp = edit->palette(PaletteContainer::palettePath(paletteLoader));
    if ( cp ){
        edit->setBindingPalette(cp);
        return cp;
    }

    CodePalette* palette = paletteList->loadAt(index);
    QmlCodeConverter* cvt = new QmlCodeConverter(edit, palette);
    palette->setExtension(cvt, true);
    edit->setBindingPalette(palette);
    edit->updatePaletteValue(palette);

    connect(palette, &CodePalette::valueChanged, cvt, &QmlCodeConverter::updateFromPalette);

    rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(edit->valuePosition());

    return palette;

}

/**
 * \brief Integrates a given palette box within the editor
 */
void CodeQmlHandler::frameEdit(QQuickItem *box, lv::QmlEditFragment *edit){
    if (!edit)
        return;

    connect(box, &QQuickItem::heightChanged, [this, box](){
        resizedEditFrame(box);
    });


    connect(box, &QQuickItem::destroyed, [this, box](){
        removeEditFrame(box);
    });

    int pos = edit->declaration()->position();
    QTextBlock tb = m_document->textDocument()->findBlock(pos);
    QTextBlock tbend = m_document->textDocument()->findBlock(pos + edit->declaration()->length());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    dh->lineBoxAdded(tb.blockNumber() + 1, tbend.blockNumber() + 1, box->height(), box);
}

void CodeQmlHandler::removeEditFrame(QQuickItem *box)
{
    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    dh->lineBoxRemoved(box);
}

void CodeQmlHandler::resizedEditFrame(QQuickItem *box)
{
    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    dh->lineBoxResized(box, box->height());
}

/**
 * \brief Receive different qml based information about a given cursor position
 */
QmlCursorInfo *CodeQmlHandler::cursorInfo(int position, int length){
    Q_D(CodeQmlHandler);
    bool canBind = false, canUnbind = false, canEdit = false, canAdjust = false, canShape = false;

    if ( !m_document )
        return new QmlCursorInfo(canBind, canUnbind, canEdit, canAdjust, canShape);

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    if ( properties.isEmpty() )
        return new QmlCursorInfo(canBind, canUnbind, canEdit, canAdjust, canShape);

    if ( properties.size() == 1 ){
        QmlDeclaration::Ptr firstdecl = properties.first();

        if ( DocumentQmlInfo::isObject(firstdecl->type()) )
            canShape = true;

        canEdit = true;

        int paletteCount = d->projectHandler->paletteContainer()->countPalettes("qml/" + firstdecl->type());
        if ( paletteCount > 0 ){
            int totalLoadedPalettes = 0;
            canBind = true;
            canShape = true;

            for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
                QmlEditFragment* edit = *it;
                if ( edit->declaration()->position() == firstdecl->position() ){
                    if ( edit->bindingPalette() ){
                        canBind = false;
                        canUnbind = true;
                    }

                    for ( auto paIt = edit->begin(); paIt != edit->end(); ++paIt ){
                        CodePalette* cp = *paIt;
                        if ( cp->type() == "edit/qml" )
                            canEdit = false;
                        else
                            ++totalLoadedPalettes;
                    }
                }
            }

            if ( totalLoadedPalettes < paletteCount ){
                canAdjust = true;
            }
        }
    }
    return new QmlCursorInfo(canBind, canUnbind, canEdit, canAdjust, canShape);
}

/**
 * \brief Closes the bindings between the given position and length
 */
void CodeQmlHandler::closeBinding(int position, int length){
    if ( !m_document )
        return;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    for ( QList<QmlDeclaration::Ptr>::iterator it = properties.begin(); it != properties.end(); ++it ){
        int position = (*it)->position();

        auto editIt = m_edits.begin();
        while ( editIt != m_edits.end() ){
            QmlEditFragment* edit = *editIt;

            if ( edit->declaration()->position() == position ){
                if ( edit->bindingPalette() ){

                    edit->removeBindingPalette();

                    if ( edit->totalPalettes() == 0 ){
                        editIt = m_edits.erase(editIt);
                        edit->emitRemoval();
                        edit->deleteLater();
                    }
                    break;
                }
            } else {
                ++editIt;
            }
        }
    }

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->rehighlightSection(position, length);
}

/**
 * \brief Opens an editing palette for the given \p edit fragment
 */
lv::CodePalette* CodeQmlHandler::edit(lv::QmlEditFragment *edit){
    Q_D(CodeQmlHandler);
    if ( !m_document )
        return nullptr;

    QList<QmlEditFragment*> toRemove;

    for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
        QmlEditFragment* curentEdit = *it;
        if ( curentEdit->declaration()->position() < edit->declaration()->position() ){
            break;
        } else if ( curentEdit->declaration()->position() == edit->declaration()->position() ){
            if ( curentEdit->totalPalettes() == 1 && (*curentEdit->begin())->type() == "edit/qml" )
                 return nullptr;

            vlog("editqmljs-codehandler").v() <<
                "Removing editing frag  with no of palettes \'" << curentEdit->totalPalettes() << "\' at " <<
                edit->declaration()->position() << " due to edit.";

            if ( curentEdit != edit )
                toRemove.append(curentEdit);
        }
    }


    for ( auto it = toRemove.begin(); it != toRemove.end(); ++it ){
        removeEditingFragment(*it);
    }
    if ( m_editingFragment && m_editingFragment != edit )
        removeEditingFragment(m_editingFragment);

    PaletteLoader* loader = d->projectHandler->paletteContainer()->findPalette("edit/qml");
    CodePalette* palette = d->projectHandler->paletteContainer()->createPalette(loader);

    edit->addPalette(palette);

    edit->declaration()->section()->onTextChanged([](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText){
        auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        int length = editingFragment->declaration()->valueLength();
        editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
    });

    palette->setExtension(new QmlCodeConverter(edit, this), true);

    connect(palette, &CodePalette::valueChanged, [this, edit](){
        if ( edit->totalPalettes() > 0 ){
            CodePalette* cp = *edit->begin();
            edit->bindingChannel()->commit(cp->value());
        }
        m_document->removeEditingState(ProjectDocument::Overlay);
        removeEditingFragment(edit);
    });

    m_document->addEditingState(ProjectDocument::Overlay);
    m_editingFragment = edit;

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh ){
        dh->rehighlightSection(edit->valuePosition(), edit->valueLength());
        dh->requestCursorPosition(edit->valuePosition());
    }

    return palette;
}

/**
 * \brief Cancels the current editing palette
 */
void CodeQmlHandler::cancelEdit(){
    m_document->removeEditingState(ProjectDocument::Overlay);
    if ( m_editingFragment ){
        removeEditingFragment(m_editingFragment);
    }
}

/**
 * \brief Get the insertion options at the given \p position
 *
 * Returns an lv::QmlAddContainer for all the options
 */
QmlAddContainer *CodeQmlHandler::getAddOptions(int position){
    Q_D(CodeQmlHandler);
    if ( !m_document || !m_target )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

    ctx->expressionPath();

    QStringList expression;
    int propertyPosition = -1;
    int propertyLength   = 0;

    QChar expressionEndDelimiter;

    if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ||
         ctx->context() & QmlCompletionContext::InAfterOnLhsOfBinding){
        expression = ctx->expressionPath();

        int advancedLength = DocumentQmlValueScanner::getExpressionExtent(
            m_target, cursor.position(), &expression, &expressionEndDelimiter
        );
        propertyLength = (cursor.position() - propertyPosition) + advancedLength;
        propertyPosition = cursor.position();

    } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding )
    {
        expression     = ctx->propertyPath();
        propertyLength = DocumentQmlValueScanner::getExpressionExtent(m_target, ctx->propertyPosition());
        propertyPosition = ctx->propertyPosition();
    }

    if ( propertyPosition == -1 )
        return nullptr;

    QmlAddContainer* addContainer = new QmlAddContainer(propertyPosition, ctx->objectTypePath());
    QmlScopeSnap scope = d->snapScope();

    if ( !ctx->objectTypePath().empty() ){
        QString type = ctx->objectTypeName();
        QString typeNamespace = ctx->objectTypePath().size() > 1 ? ctx->objectTypePath()[0] : "";
        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
        QString libraryKey;
        qmlhandler_helpers::getTypePath(scope, typeNamespace, type, typePath, libraryKey);

        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, typePath ){
            QString objectTypeName = "";
            if ( object->exports().size() > 0 )
                objectTypeName = object->exports().first().type;

            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( !object->property(i).name().startsWith("__") ){
                    addContainer->propertyModel()->addItem(QmlPropertyModel::PropertyData(
                        object->property(i).name(),
                        objectTypeName,
                        object->property(i).typeName(),
                        "", //TODO: Find library path
                        object->property(i).name())
                    );
                }
            }
            addContainer->propertyModel()->updateFilters();
        }
    }

    // import global objects


    QStringList exports;
    scope.project->implicitLibraries()->libraryInfo(scope.document->path())->listExports(&exports);

    foreach( const QString& e, exports ){
        if ( e != scope.document->componentName() ){
            addContainer->itemModel()->addItem(
                QmlItemModel::ItemData(e, "implicit", scope.document->path(), e)
            );
        }
    }

    foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
        QStringList exports;
        scope.project->globalLibraries()->libraryInfo(defaultLibrary)->listExports(&exports);
        foreach( const QString& e, exports ){
            addContainer->itemModel()->addItem(
                QmlItemModel::ItemData(e, "QtQml", "QtQml", e)
            );
        }
    }

    // import namespace objects

    QSet<QString> imports;

    foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
        if ( imp.first.as() != "" ){
            imports.insert(imp.first.as());
        }
    }
    imports.insert("");

    for ( QSet<QString>::iterator it = imports.begin(); it != imports.end(); ++it ){

        foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
            if ( imp.first.as() == *it ){
                QStringList exports;
                scope.project->globalLibraries()->libraryInfo(imp.second)->listExports(&exports);
                int segmentPosition = imp.second.lastIndexOf('/');
                QString libraryName = imp.second.mid(segmentPosition + 1);

                foreach( const QString& e, exports ){
                    addContainer->itemModel()->addItem(
                        QmlItemModel::ItemData(e, libraryName, libraryName, e)
                    );
                }
            }
        }
    }

    addContainer->itemModel()->updateFilters();

    return addContainer;
}

/**
 * \brief Add a property given the \p addText at the specified \p position
 */
int CodeQmlHandler::addProperty(
        int position,
        const QString &object,
        const QString &type,
        const QString &name,
        bool assignDefault)
{
    DocumentQmlValueScanner qvs(m_document, position, 1);
    int blockStart = qvs.getBlockStart(position) + 1;
    int blockEnd = qvs.getBlockEnd(position);

    QTextBlock tbStart = m_target->findBlock(blockStart);
    QTextBlock tbEnd   = m_target->findBlock(blockEnd);

    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    // Check wether the property is already added

    QTextCursor sourceSelection(m_target);
    sourceSelection.setPosition(blockStart);
    sourceSelection.setPosition(blockEnd, QTextCursor::KeepAnchor);

    QString source = object + "{" + sourceSelection.selectedText().replace(QChar(QChar::ParagraphSeparator), "\n") + "}";

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(m_document->file()->path());
    if ( docinfo->parse(source) ){
        lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
        lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

        for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
            lv::DocumentQmlValueObjects::RangeProperty* p = *it;
            QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);

            if ( propertyName == name ){ // property already exists, simply position the cursor accordingly
                lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());

                int sourceOffset = blockStart - 1 - object.size();

                if ( dh ){
                    dh->requestCursorPosition(sourceOffset + p->valueBegin);
                }
                return sourceOffset + p->begin;
            }
        }
    }

    // Check where to insert the property

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + name + ": " + (assignDefault ? DocumentQmlInfo::typeDefaultValue(type) : "");
        cursorPosition = insertionPosition + name.size() + 4;
    } else { // multiline object declaration
        QString indent = getBlockIndent(tbStart);
        insertionPosition = tbEnd.position();
        QTextBlock tbIt = tbEnd.previous();
        while ( tbIt != tbStart && tbIt.isValid() ){
            if ( !isBlockEmptySpace(tbIt) ){
                indent = getBlockIndent(tbIt);
                insertionPosition = tbIt.position() + tbIt.length();
                break;
            }
            tbIt = tbIt.previous();
        }

        insertionText = indent + name + ": " + (assignDefault ? DocumentQmlInfo::typeDefaultValue(type) : "") + "\n";
        cursorPosition = insertionPosition + indent.size() + name.size() + 2;
    }


    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor cs(m_target);
    cs.setPosition(insertionPosition);
    cs.beginEditBlock();
    cs.insertText(insertionText);
    cs.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);

    m_scopeTimer.start();

    lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh ){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - name.size() - 2;
}

/**
 * \brief Adds an item given the \p addText at the specitied \p position
 */
int CodeQmlHandler::addItem(int position, const QString &, const QString &type){
    DocumentQmlValueScanner qvs(m_document, position, 1);
    int blockStart = qvs.getBlockStart(position) + 1;
    int blockEnd = qvs.getBlockEnd(position);

    QTextBlock tbStart = m_target->findBlock(blockStart);
    QTextBlock tbEnd   = m_target->findBlock(blockEnd);

    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    // Check where to insert the item

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + type + "{} ";
        cursorPosition = insertionPosition + type.size() + 3;
    } else { // multiline object declaration
        QString indent = getBlockIndent(tbStart);
        insertionPosition = tbEnd.position();
        QTextBlock tbIt = tbEnd.previous();
        while ( tbIt != tbStart && tbIt.isValid() ){
            if ( !isBlockEmptySpace(tbIt) ){
                indent = getBlockIndent(tbIt);
                insertionPosition = tbIt.position() + tbIt.length();
                break;
            }
            tbIt = tbIt.previous();
        }

        insertionText = indent + type + "{\n" + indent + "}\n";
        cursorPosition = insertionPosition + indent.size() + type.size() + 1;
    }

    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor cs(m_target);
    cs.setPosition(insertionPosition);
    cs.beginEditBlock();
    cs.insertText(insertionText);
    cs.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);

    m_scopeTimer.start();

    lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if (dh){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - 1 - type.size();
}

void CodeQmlHandler::addItemToRuntime(QmlEditFragment *edit, const QString &type, QObject *currentApp){
    Q_D(CodeQmlHandler);
    if ( !edit || !currentApp )
        return;

    BindingPath* bp = edit->bindingChannel()->expressionPath();
    QQmlProperty& p = bp->property();

    QObject* result = QmlCodeConverter::create(
        *d->documentScope, type + "{}", "temp"
    );

    if ( p.propertyTypeCategory() == QQmlProperty::List ){
        QQmlListReference ppref = qvariant_cast<QQmlListReference>(p.read());
        QObject* obat = ppref.at(bp->listIndex());

        QQmlProperty assignmentProperty(obat);
        if ( assignmentProperty.propertyTypeCategory() == QQmlProperty::List ){
            QQmlListReference assignmentList = qvariant_cast<QQmlListReference>(assignmentProperty.read());
            vlog("editqmljs-codehandler").v() <<
                "Adding : " << result->metaObject()->className() << " to " << obat->metaObject()->className() << ", "
                "property " << assignmentProperty.name();
            if ( assignmentList.canAppend() ){
                assignmentList.append(result);
                return;
            }
        } else {
            vlog("editqmljs-codehandler").v() <<
                "Assigning : " << result->metaObject()->className() << " to " << obat->metaObject()->className() << ", "
                "property " << assignmentProperty.name();
            assignmentProperty.write(QVariant::fromValue(result));
            return;
        }
    } else {
        //TODO: Property based asignment
    }
}

/**
 * \brief Update palette binding channels for a new runtime root.
 */
void CodeQmlHandler::updateRuntimeBindings(QObject *runtime){
    QString source = m_target->toPlainText();
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
    docinfo->parse(source);
    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

    QList<QmlEditFragment*> toRemove;

    auto it = m_edits.begin();
    while ( it != m_edits.end() ){
        QmlEditFragment* ef = *it;

        BindingPath* bp = DocumentQmlInfo::findDeclarationPath(objects->root(), ef->declaration());
        if ( !bp ){
            vlog("editqmljs-codehandler").v() <<
                "Disconnecting \'" + ef->bindingChannel()->expressionPath()->toString() << "\' due to lack of binding path.";
            toRemove.append(ef);

        } else if ( *(ef->bindingChannel()->expressionPath()) != *bp ){
            vlog("editqmljs-codehandler").v() <<
                "Disconnecting \'" + ef->bindingChannel()->expressionPath()->toString() << "\' due to change of binding path.";
            toRemove.append(ef);

        } else {
            delete bp;
            DocumentQmlInfo::traverseBindingPath(ef->bindingChannel()->expressionPath(), runtime);
            if ( !ef->bindingChannel()->expressionPath()->hasConnection() ){
                vlog("editqmljs-codehandler").v() <<
                    "Disconnecting \'" + ef->bindingChannel()->expressionPath()->toString() << "\' due to missing root path.";
                toRemove.append(ef);
            } else {
                vlog("editqmljs-codehandler").v() <<
                    "Updating \'" + ef->bindingChannel()->expressionPath()->toString() << "\' connection.";

                BindingPath* mainPath = ef->bindingChannel()->expressionPath();
                if ( mainPath->listIndex() == -1 ){
                    mainPath->property().connectNotifySignal(ef, SLOT(updateValue()));
                }
                for ( auto palIt = ef->begin(); palIt != ef->end(); ++palIt ){
                    ef->updatePaletteValue(*palIt);
                }
            }
        }

         ++it;
     }

    for ( auto it = toRemove.begin(); it != toRemove.end(); ++it ){
        removeEditingFragment(*it);
    }
}

/**
 * \brief Handler for when a new document scope is ready
 */
void CodeQmlHandler::newDocumentScopeReady(const QString &, lv::DocumentQmlScope::Ptr documentScope){
    Q_D(CodeQmlHandler);
    d->documentScope = documentScope;
    m_newScope = true;
}

/**
 * \brief Handler for when a new project scope is ready
 */
void CodeQmlHandler::newProjectScopeReady(){
    m_newScope = true;
}

void CodeQmlHandler::suggestionsForGlobalQmlContext(
        const QmlCompletionContext &,
        QList<CodeCompletionSuggestion> &suggestions
){
    suggestions << CodeCompletionSuggestion("import", "", "", "import ");
    suggestions << CodeCompletionSuggestion("pragma singleton", "", "", "pragma singleton");
}

void CodeQmlHandler::suggestionsForImport(
        const QmlCompletionContext& context,
        QList<CodeCompletionSuggestion> &suggestions)
{

    foreach (const QString& importPath, m_engine->importPathList()){
        suggestionsForRecursiveImport(0, importPath, context.expressionPath(), suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &CodeCompletionSuggestion::compare);
}

void CodeQmlHandler::suggestionsForStringImport(
        const QString& enteredPath,
        QList<CodeCompletionSuggestion> &suggestions,
        QString& filter)
{
    if ( m_document ){
        QStringList enteredPathSegments = enteredPath.split('/');
        filter = enteredPathSegments.size() > 0 ? enteredPathSegments.last() : "";
        QString path = m_document->file()->path();
        QString dirPath = QFileInfo(path).path();
        suggestionsForRecursiveImport(0, dirPath, enteredPathSegments, suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &CodeCompletionSuggestion::compare);
}

void CodeQmlHandler::suggestionsForRecursiveImport(
        int index,
        const QString& dir,
        const QStringList& expression,
        QList<CodeCompletionSuggestion> &suggestions)
{
    if ( index >= expression.size() - 1 ){
        QDirIterator dit(dir);
        while( dit.hasNext() ){
            dit.next();
            if ( dit.fileInfo().isDir() && dit.fileName() != "." && dit.fileName() != ".."){
                suggestions << CodeCompletionSuggestion(dit.fileName(), "", "", dit.fileName());
            }
        }
    } else {
        QString path = dir + (dir.endsWith("/") ? "" : "/") + expression[index];
        ++index;
        suggestionsForRecursiveImport(index, path, expression, suggestions);
    }
}

void CodeQmlHandler::suggestionsForValueObject(
        const DocumentQmlObject &object,
        QList<CodeCompletionSuggestion> &suggestions,
        bool extractProperties,
        bool extractFunctions,
        bool extractSlots,
        bool extractSignals)
{
    if ( extractProperties ){
        for (
            QMap<QString, QString>::const_iterator it = object.memberProperties().begin();
            it != object.memberProperties().end();
            ++it )
        {
            suggestions << CodeCompletionSuggestion(it.key(), it.value(), object.typeName(), it.key());
        }
    }
    if ( extractFunctions ){
        for (
            QMap<QString, DocumentQmlObject::FunctionValue>::const_iterator it = object.memberFunctions().begin();
            it != object.memberFunctions().end();
            ++it
        )
        {
            QString completion = it.value().name + "(";
            for (
                QList<QPair<QString, QString> >::const_iterator argit = it.value().arguments.begin();
                argit != it.value().arguments.end();
                ++argit
            ){
                if ( completion.at(completion.length() - 1) != QChar('(') )
                    completion += ", ";
                completion += argit->first;
            }
            completion += ")";

            suggestions << CodeCompletionSuggestion(it.value().name + "()", "function", object.typeName(), completion);
        }
    }
    if ( extractSignals ){
        for (
            QMap<QString, DocumentQmlObject::FunctionValue>::const_iterator it = object.memberSignals().begin();
            it != object.memberSignals().end();
            ++it
        )
        {
            QString completion = it.value().name + "(";
            for (
                QList<QPair<QString, QString> >::const_iterator argit = it.value().arguments.begin();
                argit != it.value().arguments.end();
                ++argit
            ){
                if ( completion.at(completion.length() - 1) != QChar('(') )
                    completion += ", ";
                completion += argit->first;
            }
            completion += ")";

            suggestions << CodeCompletionSuggestion(it.value().name + "()", "signal", object.typeName(), completion);
        }
    }
    if ( extractSlots ){
        for (
            QMap<QString, QString>::const_iterator it = object.memberSlots().begin();
            it != object.memberSlots().end();
            ++it )
        {
            suggestions << CodeCompletionSuggestion(it.key(), "slot", object.typeName(), it.key());
        }
    }
}

void CodeQmlHandler::suggestionsForNamespaceTypes(
    const QString &typeNameSpace,
    QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();

    if ( typeNameSpace.isEmpty() ){
        QStringList exports;
        scope.project->implicitLibraries()->libraryInfo(scope.document->path())->listExports(&exports);

        foreach( const QString& e, exports ){
            if ( e != scope.document->componentName() )
                suggestions << CodeCompletionSuggestion(e, "", "implicit", e);
        }


        foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
            QStringList exports;
            scope.project->globalLibraries()->libraryInfo(defaultLibrary)->listExports(&exports);
            foreach( const QString& e, exports ){
                suggestions << CodeCompletionSuggestion(e, "", "QtQml", e);
            }
        }
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
        if ( imp.first.as() == typeNameSpace ){
            QStringList exports;
            scope.project->globalLibraries()->libraryInfo(imp.second)->listExports(&exports);
            int segmentPosition = imp.second.lastIndexOf('/');
            QString libraryName = imp.second.mid(segmentPosition + 1);
            
            foreach( const QString& e, exports ){
                suggestions << CodeCompletionSuggestion(e, "", libraryName, e);
            }
        }
    }
}

void CodeQmlHandler::suggestionsForNamespaceImports(QList<CodeCompletionSuggestion> &suggestions){
    Q_D(CodeQmlHandler);
    QMap<QString, QString> imports;
    DocumentQmlScope::Ptr document = d->documentScope;

    QList<CodeCompletionSuggestion> localSuggestions;

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() != "" ){
            imports[imp.first.as()] = imp.first.path();
        }
    }
    for ( QMap<QString, QString>::iterator it = imports.begin(); it != imports.end(); ++it ){
        localSuggestions << CodeCompletionSuggestion(it.key(), "import", it.value(), it.key());
    }
    std::sort(localSuggestions.begin(), localSuggestions.end(), &CodeCompletionSuggestion::compare);

    suggestions << localSuggestions;
}

void CodeQmlHandler::suggestionsForDocumentsIds(QList<CodeCompletionSuggestion> &suggestions){
    Q_D(CodeQmlHandler);
    QStringList ids = d->documentScope->info()->extractIds();
    ids.sort();
    foreach( const QString& id, ids ){
        suggestions << CodeCompletionSuggestion(id, "id", "", id);
    }
}

void CodeQmlHandler::suggestionsForLeftBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);
    if ( context.objectTypePath().size() == 0 )
        return;
    
    QmlScopeSnap scope = d->snapScope();

    if ( context.expressionPath().size() > 1 ){
        QString firstSegment = context.expressionPath()[0];
        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
        bool isPointer = false;
        QString typeLibraryKey;
        if ( qmlhandler_helpers::getProperty(
                 scope,
                 context.objectTypePath(),
                 firstSegment,
                 cursorPosition,
                 typePath,
                 isPointer,
                 typeLibraryKey
        ) ){
            if ( isPointer ){
                for ( int i = 1; i < context.expressionPath().size() - 1; ++i ){
                    QList<LanguageUtils::FakeMetaObject::ConstPtr> newTypePath;

                    qmlhandler_helpers::evaluatePropertyClass(
                        scope,
                        typePath,
                        typeLibraryKey,
                        context.expressionPath()[i],
                        newTypePath
                    );
                    typePath = newTypePath;
                }
                qmlhandler_helpers::suggestionsForObjectPath(typePath, true, false, false, false, false, ": ", suggestions);
            }
        } else if ( qmlhandler_helpers::isNamespace(scope.document, firstSegment) ){
            if ( context.expressionPath().size() == 2 )
                suggestionsForNamespaceTypes(firstSegment, suggestions);
        }

    } else {
        suggestions << CodeCompletionSuggestion("function", "", "", "function ");
        suggestions << CodeCompletionSuggestion("property", "", "", "property ");
        QString type = context.objectTypePath().size() > 0 ? context.objectTypePath()[0] : "";
        if ( type != "" ){
            QString typeNamespace = context.objectTypePath().size() > 1 ? context.objectTypePath()[1] : "";
            QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
            QString libraryKey;
            qmlhandler_helpers::getTypePath(scope, typeNamespace, type, typePath, libraryKey);
            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, false, false, false, false, ": ", suggestions);
        }
        suggestionsForNamespaceTypes("", suggestions);
        suggestionsForNamespaceImports(suggestions);
    }
}

void CodeQmlHandler::suggestionsForRightBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

    if ( context.expressionPath().size() > 1 ){
        QString firstSegment = context.expressionPath()[0];
        DocumentQmlInfo::ValueReference value;
        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
        QString typeLibraryKey;

//        QString propertyType;
        bool isPointer = false;

        if (qmlhandler_helpers::isId(scope, firstSegment, value, typePath, typeLibraryKey)){

            DocumentQmlObject valueObj = scope.document->info()->extractValueObject(value);
            int startSegment = 1;

            if ( startSegment < context.expressionPath().size() - 1 ){
                if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
                    QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
                    if ( DocumentQmlInfo::isObject(valueType) ){
                        typePath.clear();
                        qmlhandler_helpers::getTypePath(
                            scope,
                            valueType,
                            typePath,
                            typeLibraryKey
                        );
                    }
                    ++startSegment;
                }
            } else {
                suggestionsForValueObject(valueObj, suggestions, true, true, false, true);
            }

            for ( int i = startSegment; i < context.expressionPath().size() - 1; ++i ){
                QList<LanguageUtils::FakeMetaObject::ConstPtr> newTypePath;

                qmlhandler_helpers::evaluatePropertyClass(
                    scope,
                    typePath,
                    typeLibraryKey,
                    context.expressionPath()[i],
                    newTypePath
                );
                typePath = newTypePath;
            }

            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);

        } else if (qmlhandler_helpers::isParent(
                       scope,
                       firstSegment,
                       cursorPosition,
                       value,
                       typePath,
                       typeLibraryKey))
        {
            DocumentQmlObject valueObj = scope.document->info()->extractValueObject(value);
            int startSegment = 1;

            if ( startSegment < context.expressionPath().size() - 1 ){
                if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
                    QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
                    if ( DocumentQmlInfo::isObject(valueType) ){
                        typePath.clear();
                        qmlhandler_helpers::getTypePath(
                            scope,
                            valueType,
                            typePath,
                            typeLibraryKey
                        );
                    }
                    ++startSegment;
                }
            } else {
                suggestionsForValueObject(valueObj, suggestions, true, true, false, true);
            }

            for ( int i = 1; i < context.expressionPath().size() - 1; ++i ){
                QList<LanguageUtils::FakeMetaObject::ConstPtr> newTypePath;

                qmlhandler_helpers::evaluatePropertyClass(
                    scope,
                    typePath,
                    typeLibraryKey,
                    context.expressionPath()[i],
                    newTypePath
                );
                typePath = newTypePath;
            }

            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);

        } else if (qmlhandler_helpers::getProperty(
                       scope,
                       context.objectTypePath(),
                       firstSegment,
                       cursorPosition,
                       typePath,
                       isPointer,
                       typeLibraryKey))
        {
            if ( isPointer ){
                for ( int i = 1; i < context.expressionPath().size() - 1; ++i ){
                    QList<LanguageUtils::FakeMetaObject::ConstPtr> newTypePath;

                    qmlhandler_helpers::evaluatePropertyClass(
                        scope,
                        typePath,
                        typeLibraryKey,
                        context.expressionPath()[i],
                        newTypePath
                    );
                    typePath = newTypePath;
                }
                qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);
            }


        } else if (qmlhandler_helpers::isType(
                       scope,
                       "",
                       firstSegment,
                       typePath,
                       typeLibraryKey ) )
        {
            qmlhandler_helpers::suggestionsForObjectPath(
                typePath, false, false, false, true, false, "", suggestions
            );
        } else if ( qmlhandler_helpers::isNamespace(scope.document, firstSegment) )
        {
            if ( context.expressionPath().size() == 3 ){

                /// get type and enums
                QmlLibraryInfo::ExportVersion ev = qmlhandler_helpers::getType(
                    scope, context.expressionPath()[1], firstSegment, typeLibraryKey
                );
                if ( ev.isValid() ){
                    typePath.append(ev.object);

                    qmlhandler_helpers::generateTypePathFromObject(
                        scope,
                        ev.object,
                        typeLibraryKey,
                        typePath
                    );

                    qmlhandler_helpers::suggestionsForObjectPath(
                        typePath, false, false, false, true, false, "", suggestions
                    );
                }
            } else if ( context.expressionPath().size() == 2 ){

                /// get namespace types
                suggestionsForNamespaceTypes(firstSegment, suggestions);
            }
        }

    } else {
        /// single expression path
        if ( context.propertyName() == "id" ){

            /// special case for id
            QString type = context.objectTypePath().size() > 0 ? context.objectTypePath().last() : "";
            if ( type.isEmpty() )
                return;
            type[0] = type[0].toLower();
            suggestions << CodeCompletionSuggestion(type, "id", "", type);
        } else {

            /// other cases
            suggestions << CodeCompletionSuggestion("parent", "id", "", "parent");
            suggestionsForDocumentsIds(suggestions);
            DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(cursorPosition);
            if ( !scope.document->info()->isValueNull(documentValue) )
                suggestionsForValueObject(
                    scope.document->info()->extractValueObject(documentValue),
                    suggestions,
                    true,
                    true,
                    false,
                    true
                );

            QString type = context.objectTypePath().size() > 0 ? context.objectTypePath().last() : "";
            if ( type == "" )
                return;
            QString typeNamespace = context.objectTypePath().size() > 1 ? context.objectTypePath().first() : "";
            QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
            QString libraryKey;
            qmlhandler_helpers::getTypePath(scope, typeNamespace, type, typePath, libraryKey);
            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);
            suggestionsForNamespaceTypes("", suggestions);
        }
    }
}

void CodeQmlHandler::suggestionsForLeftSignalBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();

    DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(cursorPosition);
    if ( !scope.document->info()->isValueNull(documentValue) )
        suggestionsForValueObject(
            scope.document->info()->extractValueObject(documentValue),
            suggestions,
            true,
            true,
            true,
            true
        );

    QString type = context.objectTypePath().size() > 0 ? context.objectTypePath()[0] : "";
    if ( type == "" )
        return;
    QString typeNamespace = context.objectTypePath().size() > 1 ? context.objectTypePath()[1] : "";
    QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
    QString libraryKey;
    qmlhandler_helpers::getTypePath(scope, typeNamespace, type, typePath, libraryKey);
    qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, true, true, true, ": ", suggestions);
}

QString CodeQmlHandler::extractQuotedString(const QTextCursor &cursor) const{
    QTextBlock block = cursor.block();
    int localCursorPosition = cursor.positionInBlock();
    QString blockString = block.text();

    int startOfQuote = blockString.lastIndexOf('\"', localCursorPosition - 1);
    if ( startOfQuote != -1 )
        return blockString.mid(startOfQuote + 1, localCursorPosition - startOfQuote - 1);
    else
        return blockString.mid(0, localCursorPosition);
}

QString CodeQmlHandler::getBlockIndent(const QTextBlock &bl){
    QString blockText = bl.text();

    for ( int i = 0; i < blockText.length(); ++i ){
        if ( !blockText[i].isSpace() )
            return blockText.mid(0, i);
    }
    return blockText;
}

bool CodeQmlHandler::isBlockEmptySpace(const QTextBlock &bl){
    QString blockText = bl.text();
    for ( auto it = blockText.begin(); it != blockText.end(); ++it )
        if ( !it->isSpace() )
            return false;

    return true;
}

}// namespace
