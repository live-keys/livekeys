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

#include "live/codecompletionsuggestion.h"
#include "live/projectfile.h"
#include "live/documentqmlfragment.h"
#include "live/project.h"
#include "live/editorsettings.h"
#include "live/projectqmlextension.h"
#include "live/visuallog.h"

#include <QQmlEngine>
#include <QFileInfo>
#include <QDirIterator>
#include <QTextDocument>
#include <QQmlComponent>
#include <QStringList>
#include <QWaitCondition>

namespace lv{

namespace qmlhandler_helpers{

    class ScopeCopy{
    public:
        ScopeCopy(const ProjectQmlScope::Ptr& pProject, const DocumentQmlScope::Ptr& pDocument)
            : project(pProject)
            , document(pDocument)
        {}

        ProjectQmlScope::Ptr  project;
        DocumentQmlScope::Ptr document;
    };

    /// Retrieve a type from any available libraries to the document scope

    QmlLibraryInfo::ExportVersion getType(
            DocumentQmlScope::Ptr document,
            ProjectQmlScope::Ptr project,
            const QString& name,
            QString& libraryKey)
    {
        QmlLibraryInfo::ExportVersion ev =
            project->implicitLibraries()->libraryInfo(document->path())->findExport(name);
        if ( ev.isValid() ){
            libraryKey = "";
            return ev;
        }

        foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
            QmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(imp.second)->findExport(name);
            if ( ev.isValid() ){
                libraryKey = imp.second;
                return ev;
            }
        }

        foreach( const QString& defaultLibrary, project->defaultLibraries() ){
            QmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(defaultLibrary)->findExport(name);
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
            DocumentQmlScope::Ptr document,
            ProjectQmlScope::Ptr project,
            const QString& importNamespace,
            const QString& name,
            QString& libraryKey)
    {
        if ( importNamespace.isEmpty() ){
            QmlLibraryInfo::ExportVersion ev =
                project->implicitLibraries()->libraryInfo(document->path())->findExport(name);
            if ( ev.isValid() ){
                libraryKey = "";
                return ev;
            }

            foreach( const QString& defaultLibrary, project->defaultLibraries() ){
                QmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(defaultLibrary)->findExport(name);
                if ( ev.isValid() ){
                    libraryKey = defaultLibrary;
                    return ev;
                }
            }
        }

        foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
            if ( imp.first.as() == importNamespace ){
                QmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(imp.second)->findExport(name);
                if ( ev.isValid() ){
                    libraryKey = imp.second;
                    return ev;
                }
            }
        }

        return QmlLibraryInfo::ExportVersion();
    }

    void generateTypePathFromObject(
        DocumentQmlScope::Ptr documentScope,
        ProjectQmlScope::Ptr project,
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
                    ? project->implicitLibraries()->libraryInfo(documentScope->path())
                    : project->globalLibraries()->libraryInfo(typeLibrary);

            LanguageUtils::FakeMetaObject::ConstPtr superObject = libraryInfo->findObjectByClassName(typeSuperClass);

            if ( superObject.isNull()  ){
                ProjectQmlScopeContainer* globalLibs = project->globalLibraries();
                foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
                    superObject = globalLibs->libraryInfo(libraryDependency)->findObjectByClassName(typeSuperClass);
                    if ( !superObject.isNull() ){
                        typeLibrary = libraryDependency;
                        break;
                    }
                }

                if ( superObject.isNull() ){
                    foreach( const QString& defaultLibrary, project->defaultLibraries() ){
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
                generateTypePathFromObject(documentScope, project, superObject, typeLibrary, typePath);
            }
        }
    }

    void generateTypePathFromClassName(
        DocumentQmlScope::Ptr documentScope,
        ProjectQmlScope::Ptr project,
        const QString& typeName,
        QString typeLibrary,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath
    ){
        vlog_debug("editqmljs-codehandler", "Looking up type \'" + typeName + "\' from " + typeLibrary);
        QmlLibraryInfo::Ptr libraryInfo = typeLibrary == ""
                ? project->implicitLibraries()->libraryInfo(documentScope->path())
                : project->globalLibraries()->libraryInfo(typeLibrary);

        LanguageUtils::FakeMetaObject::ConstPtr object = libraryInfo->findObjectByClassName(typeName);

        if ( object.isNull() ){
            foreach( const QString& libraryDependency, libraryInfo->dependencyPaths() ){
                object = project->globalLibraries()->libraryInfo(libraryDependency)->findObjectByClassName(typeName);
                if ( !object.isNull() ){
                    typeLibrary = libraryDependency;
                    break;
                }
            }
        }

        if ( !object.isNull() ){
            typePath.append(object);
            generateTypePathFromObject(documentScope, project, object, typeLibrary, typePath);
        }
    }

    void getTypePath(
        DocumentQmlScope::Ptr documentScope,
        ProjectQmlScope::Ptr project,
        const QString& importAs,
        const QString& name,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey
    ){
        QmlLibraryInfo::ExportVersion ev = getType(
            documentScope, project, importAs, name, libraryKey
        );

        if ( ev.isValid() ){
            typePath.append(ev.object);

            qmlhandler_helpers::generateTypePathFromObject(
                documentScope,
                project,
                ev.object,
                libraryKey,
                typePath
            );
        }
    }

    void getTypePath(
        DocumentQmlScope::Ptr documentScope,
        ProjectQmlScope::Ptr project,
        const QString& name,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey
    ){
        QmlLibraryInfo::ExportVersion ev = getType(
            documentScope, project, name, libraryKey
        );

        if ( ev.isValid() ){
            typePath.append(ev.object);

            qmlhandler_helpers::generateTypePathFromObject(
                documentScope,
                project,
                ev.object,
                libraryKey,
                typePath
            );
        }
    }

    void evaluatePropertyClass(
        DocumentQmlScope::Ptr documentScope,
        ProjectQmlScope::Ptr projectScope,
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
                            documentScope,
                            projectScope,
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
        const QString& str,
        DocumentQmlScope::Ptr scope,
        ProjectQmlScope::Ptr projectScope,
        DocumentQmlInfo::ValueReference& documentValue,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& typeLibraryKey
    ){
        documentValue = scope->info()->valueForId(str);
        if ( scope->info()->isValueNull(documentValue) )
            return false;

        QString typeName = scope->info()->extractTypeName(documentValue);
        if ( typeName != "" ){
            getTypePath(scope, projectScope, typeName, typePath, typeLibraryKey);
        }

        return true;
    }

    /**
     * @brief Checks wether value is a parent in the given document scope.
     * Outputs the object path and value if true.
     */
    bool isParent(
        const QString& str,
        int position,
        DocumentQmlScope::Ptr scope,
        ProjectQmlScope::Ptr projectScope,
        DocumentQmlInfo::ValueReference& parentValue,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& typeLibraryKey
    ){
        if ( str != "parent" )
            return false;

        DocumentQmlInfo::ValueReference documentValue = scope->info()->valueAtPosition(position);
        if ( scope->info()->isValueNull(documentValue) )
            return true;

        scope->info()->extractValueObject(documentValue, &parentValue);
        if ( scope->info()->isValueNull(parentValue) )
            return true;

        QString typeName = scope->info()->extractTypeName(parentValue);
        if ( typeName != "" ){
            getTypePath(scope, projectScope, typeName, typePath, typeLibraryKey);
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
        const ScopeCopy& scope,
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
        getTypePath(scope.document, scope.project, typeNamespace, type, contextTypePath, contextObjectLibrary);

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
        const ScopeCopy& scope,
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
                        scope.document, scope.project, property.typeName(), contextObjectLibrary, typePath
                    );
                } else {
                    qmlhandler_helpers::getTypePath(
                        scope.document, scope.project, property.typeName(), typePath, contextObjectLibrary
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
        const QStringList& contextObject,
        const QString& str,
        int position,
        DocumentQmlScope::Ptr scope,
        ProjectQmlScope::Ptr projectScope,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        bool& isPointer,
        QString& libraryKey
    ){
        DocumentQmlInfo::ValueReference documentValue = scope->info()->valueAtPosition(position);
        if ( !scope->info()->isValueNull(documentValue) ){
            DocumentQmlObject valueObject = scope->info()->extractValueObject(documentValue);

            for (
                QMap<QString, QString>::const_iterator it = valueObject.memberProperties().begin();
                it != valueObject.memberProperties().end();
                ++it )
            {
                if ( it.key() == str ){
                    QString propertyType = it.value();
                    isPointer = DocumentQmlInfo::isObject(propertyType);
                    if ( isPointer ){
                        getTypePath(scope, projectScope, propertyType, typePath, libraryKey);
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
        getTypePath(scope, projectScope, typeNamespace, type, contextTypePath, libraryKey);

        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, contextTypePath ){
            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( object->property(i).name() == str ){
                    QString propertyType = object->property(i).typeName();
                    isPointer = object->property(i).isPointer();
                    if ( isPointer ){
                        generateTypePathFromClassName(scope, projectScope, propertyType, libraryKey, typePath);
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
        const QString& importAs,
        const QString& str,
        DocumentQmlScope::Ptr documentScope,
        ProjectQmlScope::Ptr projectScope,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey)
    {
        QmlLibraryInfo::ExportVersion ev = getType(
            documentScope, projectScope, importAs, str, libraryKey
        );

        if ( ev.isValid() ){
            typePath.append(ev.object);

            qmlhandler_helpers::generateTypePathFromObject(
                documentScope,
                projectScope,
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
    bool isNamespace(const QString& str, DocumentQmlScope::Ptr documentScope){
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
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Method && suggestMethods ){
                    QString completion =
                        m.methodName() + "(" + m.parameterNames().join(", ") + ")";
                        localSuggestions << CodeCompletionSuggestion(
                            m.methodName() + "()", "function", objectTypeName, completion
                        );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestSignals ){
                    QString completion =
                        m.methodName() + "(" + m.parameterNames().join(", ") + ")";
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

// QDocumentQmlHandler implementation
// ----------------------------------

CodeQmlHandler::CodeQmlHandler(
        Engine *engine,
        Project *,
        QmlJsSettings *settings,
        ProjectQmlExtension *projectHandler,
        QObject *parent)
    : AbstractCodeHandler(parent)
    , m_target(0)
    , m_highlighter(new QmlJsHighlighter(settings, 0))
    , m_settings(settings)
    , m_engine(engine->engine())
    , m_completionContextFinder(new QmlCompletionContextFinder)
    , m_documentScope(0)
    , m_projectHandler(projectHandler)
    , m_newScope(false)
{
    m_projectHandler->scanMonitor()->addScopeListener(this);
}

CodeQmlHandler::~CodeQmlHandler(){
    m_projectHandler->scanMonitor()->removeScopeListener(this);
    delete m_completionContextFinder;
}

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

void CodeQmlHandler::setTarget(QTextDocument *target){
    m_target      = target;
    m_highlighter->setTarget(m_target);
}

void CodeQmlHandler::setDocument(ProjectDocument *document){
    m_document      = document;
    m_documentScope = DocumentQmlScope::createEmptyScope(m_projectHandler->scanMonitor()->projectScope());

    if ( m_projectHandler->scanMonitor()->hasProjectScope() && document != 0 ){
        m_projectHandler->scanMonitor()->scanNewDocumentScope(document->file()->path(), document->content(), this);
        m_projectHandler->scanner()->queueProjectScan();
    }
}

void CodeQmlHandler::updateScope(const QString& data){
    if ( m_projectHandler->scanMonitor()->hasProjectScope() && m_document )
        m_projectHandler->scanMonitor()->queueNewDocumentScope(m_document->file()->path(), data, this);
}

void CodeQmlHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_highlighter ){
        m_highlighter->rehighlightBlock(block);
    }
}

QList<CodeDeclaration::Ptr> CodeQmlHandler::getDeclarations(const QTextCursor& cursor){

    QList<CodeDeclaration::Ptr> properties;
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

                properties.append(CodeDeclaration::create(
                    QStringList(),
                    expression.last(),
                    QStringList(),
                    propertyPosition,
                    0,
                    m_document
                ));

            } else { // dealing with a property declaration
                qmlhandler_helpers::ScopeCopy scope(m_projectHandler->scanMonitor()->projectScope(), m_documentScope);
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
                        scope.document, scope.project, property.typeName(), typeLibraryKey, typePath
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
                    properties.append(CodeDeclaration::create(
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

            qmlhandler_helpers::ScopeCopy scope(m_projectHandler->scanMonitor()->projectScope(), m_documentScope);

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
                            scope.document, scope.project, property.typeName(), typeLibraryKey, typePath
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
                    properties.append(CodeDeclaration::create(
                        rp->name(), propertyType, QStringList(), rp->begin, rp->propertyEnd - rp->begin, m_document
                    ));
                }
            }
        }
    }

    return properties;
}

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

void CodeQmlHandler::connectBindings(QList<CodeRuntimeBinding *> bindings, QObject *root){
    if ( m_document && m_document->isActive() )
        DocumentQmlInfo::syncBindings(m_target->toPlainText(), m_document, bindings, root);
}

DocumentEditFragment *CodeQmlHandler::createInjectionChannel(
        CodeDeclaration::Ptr declaration,
        QObject *runtime,
        CodeConverter* converter)
{
    if ( m_document && m_document->isActive() ){

        int listIndex = -1;
        QQmlProperty foundProperty(DocumentQmlInfo::findRuntimeMatchingDeclaration(
            m_target->toPlainText(), m_document, declaration, runtime, listIndex
        ));

        if ( foundProperty.isValid() ){
            return new DocumentQmlFragment(
                declaration,
                converter,
                foundProperty,
                listIndex
            );
        }
    }

    return 0;
}

QPair<int, int> CodeQmlHandler::contextBlock(int position){
    DocumentQmlValueScanner vs(m_document, position, 0);
    int start = vs.getBlockStart(position);
    int end   = vs.getBlockEnd(start + 1);
    return QPair<int, int>(start, end);
}


void CodeQmlHandler::newDocumentScopeReady(const QString &, lv::DocumentQmlScope::Ptr documentScope){
    m_documentScope = documentScope;
    m_newScope = true;
}

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
    ProjectQmlScope::Ptr projectScope = m_projectHandler->scanMonitor()->projectScope();
    DocumentQmlScope::Ptr document    = m_documentScope;

    if ( typeNameSpace.isEmpty() ){
        QStringList exports;
        projectScope->implicitLibraries()->libraryInfo(document->path())->listExports(&exports);

        foreach( const QString& e, exports ){
            if ( e != document->componentName() )
                suggestions << CodeCompletionSuggestion(e, "", "implicit", e);
        }


        foreach( const QString& defaultLibrary, projectScope->defaultLibraries() ){
            QStringList exports;
            projectScope->globalLibraries()->libraryInfo(defaultLibrary)->listExports(&exports);
            foreach( const QString& e, exports ){
                suggestions << CodeCompletionSuggestion(e, "", "QtQml", e);
            }
        }
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == typeNameSpace ){
            QStringList exports;
            projectScope->globalLibraries()->libraryInfo(imp.second)->listExports(&exports);
            int segmentPosition = imp.second.lastIndexOf('/');
            QString libraryName = imp.second.mid(segmentPosition + 1);
            
            foreach( const QString& e, exports ){
                suggestions << CodeCompletionSuggestion(e, "", libraryName, e);
            }
        }
    }
}

void CodeQmlHandler::suggestionsForNamespaceImports(QList<CodeCompletionSuggestion> &suggestions){
    QMap<QString, QString> imports;
    DocumentQmlScope::Ptr document = m_documentScope;

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
    QStringList ids = m_documentScope->info()->extractIds();
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
    if ( context.objectTypePath().size() == 0 )
        return;
    
    DocumentQmlScope::Ptr documentScope = m_documentScope;
    ProjectQmlScope::Ptr projectScope = m_projectHandler->scanMonitor()->projectScope();

    if ( context.expressionPath().size() > 1 ){
        QString firstSegment = context.expressionPath()[0];
        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
        bool isPointer = false;
        QString typeLibraryKey;
        if ( qmlhandler_helpers::getProperty(
                 context.objectTypePath(),
                 firstSegment,
                 cursorPosition,
                 documentScope,
                 projectScope,
                 typePath,
                 isPointer,
                 typeLibraryKey
        ) ){
            if ( isPointer ){
                for ( int i = 1; i < context.expressionPath().size() - 1; ++i ){
                    QList<LanguageUtils::FakeMetaObject::ConstPtr> newTypePath;

                    qmlhandler_helpers::evaluatePropertyClass(
                        documentScope,
                        projectScope,
                        typePath,
                        typeLibraryKey,
                        context.expressionPath()[i],
                        newTypePath
                    );
                    typePath = newTypePath;
                }
                qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, ": ", suggestions);
            }
        } else if ( qmlhandler_helpers::isNamespace(firstSegment, documentScope) ){
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
            qmlhandler_helpers::getTypePath(documentScope, projectScope, typeNamespace, type, typePath, libraryKey);
            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, ": ", suggestions);
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
    DocumentQmlScope::Ptr documentScope = m_documentScope;
    ProjectQmlScope::Ptr projectScope   = m_projectHandler->scanMonitor()->projectScope();

    if ( context.expressionPath().size() > 1 ){
        QString firstSegment = context.expressionPath()[0];
        DocumentQmlInfo::ValueReference value;
        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
        QString typeLibraryKey;

//        QString propertyType;
        bool isPointer = false;

        if (qmlhandler_helpers::isId(firstSegment, documentScope, projectScope, value, typePath, typeLibraryKey)){

            DocumentQmlObject valueObj = documentScope->info()->extractValueObject(value);
            int startSegment = 1;

            if ( startSegment < context.expressionPath().size() - 1 ){
                if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
                    QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
                    if ( DocumentQmlInfo::isObject(valueType) ){
                        typePath.clear();
                        qmlhandler_helpers::getTypePath(
                            documentScope,
                            projectScope,
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
                    documentScope,
                    projectScope,
                    typePath,
                    typeLibraryKey,
                    context.expressionPath()[i],
                    newTypePath
                );
                typePath = newTypePath;
            }

            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);

        } else if (qmlhandler_helpers::isParent(
                       firstSegment,
                       cursorPosition,
                       documentScope,
                       projectScope,
                       value,
                       typePath,
                       typeLibraryKey))
        {
            DocumentQmlObject valueObj = documentScope->info()->extractValueObject(value);
            int startSegment = 1;

            if ( startSegment < context.expressionPath().size() - 1 ){
                if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
                    QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
                    if ( DocumentQmlInfo::isObject(valueType) ){
                        typePath.clear();
                        qmlhandler_helpers::getTypePath(
                            documentScope,
                            projectScope,
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
                    documentScope,
                    projectScope,
                    typePath,
                    typeLibraryKey,
                    context.expressionPath()[i],
                    newTypePath
                );
                typePath = newTypePath;
            }

            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);

        } else if (qmlhandler_helpers::getProperty(
                       context.objectTypePath(),
                       firstSegment,
                       cursorPosition,
                       documentScope,
                       projectScope,
                       typePath,
                       isPointer,
                       typeLibraryKey))
        {
            if ( isPointer ){
                for ( int i = 1; i < context.expressionPath().size() - 1; ++i ){
                    QList<LanguageUtils::FakeMetaObject::ConstPtr> newTypePath;

                    qmlhandler_helpers::evaluatePropertyClass(
                        documentScope,
                        projectScope,
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
                       "",
                       firstSegment,
                       documentScope,
                       projectScope,
                       typePath,
                       typeLibraryKey ) )
        {
            qmlhandler_helpers::suggestionsForObjectPath(
                typePath, false, false, false, true, false, "", suggestions
            );
        } else if ( qmlhandler_helpers::isNamespace(firstSegment, documentScope) )
        {
            if ( context.expressionPath().size() == 3 ){

                /// get type and enums
                QmlLibraryInfo::ExportVersion ev = qmlhandler_helpers::getType(
                    documentScope, projectScope, context.expressionPath()[1], firstSegment, typeLibraryKey
                );
                if ( ev.isValid() ){
                    typePath.append(ev.object);

                    qmlhandler_helpers::generateTypePathFromObject(
                        documentScope,
                        projectScope,
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
            DocumentQmlInfo::ValueReference documentValue = documentScope->info()->valueAtPosition(cursorPosition);
            if ( !documentScope->info()->isValueNull(documentValue) )
                suggestionsForValueObject(
                    documentScope->info()->extractValueObject(documentValue),
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
            qmlhandler_helpers::getTypePath(documentScope, projectScope, typeNamespace, type, typePath, libraryKey);
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
    DocumentQmlScope::Ptr documentScope = m_documentScope;
    ProjectQmlScope::Ptr projectScope = m_projectHandler->scanMonitor()->projectScope();

    DocumentQmlInfo::ValueReference documentValue = documentScope->info()->valueAtPosition(cursorPosition);
    if ( !documentScope->info()->isValueNull(documentValue) )
        suggestionsForValueObject(
            documentScope->info()->extractValueObject(documentValue),
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
    qmlhandler_helpers::getTypePath(documentScope, projectScope, typeNamespace, type, typePath, libraryKey);
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

}// namespace
