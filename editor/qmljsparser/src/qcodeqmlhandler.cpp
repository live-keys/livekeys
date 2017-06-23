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

#include "qcodeqmlhandler.h"
#include "qqmljshighlighter_p.h"
#include "qqmljssettings.h"
#include "qprojectqmlscanner_p.h"
#include "qdocumentqmlobject.h"
#include "qdocumentqmlobject_p.h"
#include "qqmlcompletioncontextfinder.h"
#include "qcodecompletionsuggestion.h"
#include "qprojectqmlscopecontainer_p.h"
#include "qqmllibraryinfo_p.h"
#include "qprojectfile.h"
#include "qplugininfoextractor.h"
#include "qplugintypesfacade.h"
#include "qdocumentqmlvaluescanner_p.h"
#include "qdocumentqmlvalueobjects.h"
#include "qdocumentqmlfragment.h"
#include "qqmljshighlighter_p.h"

#include "qmljs/qmljsscanner.h"

#include <QQmlEngine>
#include <QFileInfo>
#include <QDirIterator>
#include <QTextDocument>
#include <QQmlComponent>
#include <QStringList>
#include <QWaitCondition>

#include <QDebug>

//#define QDOCUMENT_QML_HANDLER_DEBUG_FLAG
#ifdef QDOCUMENT_QML_HANDLER_DEBUG_FLAG
#define QDOCUMENT_QML_HANDLER_DEBUG(_param) qDebug() << "QML HANDLER:" << (_param)
#else
#define QDOCUMENT_QML_HANDLER_DEBUG(_param)
#endif


namespace lcv{

namespace qmlhandler_helpers{

    class ScopeCopy{
    public:
        ScopeCopy(const QProjectQmlScope::Ptr& pProject, const QDocumentQmlScope::Ptr& pDocument)
            : project(pProject)
            , document(pDocument)
        {}

        QProjectQmlScope::Ptr  project;
        QDocumentQmlScope::Ptr document;
    };

    /// Retrieve a type from any available libraries to the document scope

    QQmlLibraryInfo::ExportVersion getType(
            QDocumentQmlScope::Ptr document,
            QProjectQmlScope::Ptr project,
            const QString& name,
            QString& libraryKey)
    {
        QQmlLibraryInfo::ExportVersion ev =
            project->implicitLibraries()->libraryInfo(document->path())->findExport(name);
        if ( ev.isValid() ){
            libraryKey = "";
            return ev;
        }

        foreach( const QDocumentQmlScope::ImportEntry& imp, document->imports() ){
            QQmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(imp.second)->findExport(name);
            if ( ev.isValid() ){
                libraryKey = imp.second;
                return ev;
            }
        }

        foreach( const QString& defaultLibrary, project->defaultLibraries() ){
            QQmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(defaultLibrary)->findExport(name);
            if ( ev.isValid() ){
                libraryKey = defaultLibrary;
                return ev;
            }
        }

        return QQmlLibraryInfo::ExportVersion();
    }

    /// Retrieve a type from a specified namespace. If the namespace is empty, the type is searched through
    /// implicit libraries and default ones as well. Otherwise, only libraries with the imported namespace are
    /// searched.

    QQmlLibraryInfo::ExportVersion getType(
            QDocumentQmlScope::Ptr document,
            QProjectQmlScope::Ptr project,
            const QString& importNamespace,
            const QString& name,
            QString& libraryKey)
    {
        if ( importNamespace.isEmpty() ){
            QQmlLibraryInfo::ExportVersion ev =
                project->implicitLibraries()->libraryInfo(document->path())->findExport(name);
            if ( ev.isValid() ){
                libraryKey = "";
                return ev;
            }

            foreach( const QString& defaultLibrary, project->defaultLibraries() ){
                QQmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(defaultLibrary)->findExport(name);
                if ( ev.isValid() ){
                    libraryKey = defaultLibrary;
                    return ev;
                }
            }
        }

        foreach( const QDocumentQmlScope::ImportEntry& imp, document->imports() ){
            if ( imp.first.as() == importNamespace ){
                QQmlLibraryInfo::ExportVersion ev = project->globalLibraries()->libraryInfo(imp.second)->findExport(name);
                if ( ev.isValid() ){
                    libraryKey = imp.second;
                    return ev;
                }
            }
        }

        return QQmlLibraryInfo::ExportVersion();
    }

    void generateTypePathFromObject(
        QDocumentQmlScope::Ptr documentScope,
        QProjectQmlScope::Ptr project,
        LanguageUtils::FakeMetaObject::ConstPtr& object,
        QString typeLibrary,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath
    ){
        if ( !object.isNull() && object->superclassName() != "" && object->superclassName() != object->className() ){
            QString typeSuperClass = object->superclassName();
            QDOCUMENT_QML_HANDLER_DEBUG("Loooking up object \'" + typeSuperClass + "\' from " + typeLibrary);

            // Slider -> Slider (Controls) -> Slider(Controls.Private) -> ... (the reason I can go into a loop is recursive library dependencies)
            // Avoid loop? -> keep track of all library dependencies and dont go back -> super type with the same name cannot be from the same library
            QQmlLibraryInfo::Ptr libraryInfo = typeLibrary == ""
                    ? project->implicitLibraries()->libraryInfo(documentScope->path())
                    : project->globalLibraries()->libraryInfo(typeLibrary);

            LanguageUtils::FakeMetaObject::ConstPtr superObject = libraryInfo->findObjectByClassName(typeSuperClass);

            if ( superObject.isNull()  ){
                QProjectQmlScopeContainer* globalLibs = project->globalLibraries();
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
        QDocumentQmlScope::Ptr documentScope,
        QProjectQmlScope::Ptr project,
        const QString& typeName,
        QString typeLibrary,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath
    ){
        QDOCUMENT_QML_HANDLER_DEBUG("Looking up type \'" + typeName + "\' from " + typeLibrary);
        QQmlLibraryInfo::Ptr libraryInfo = typeLibrary == ""
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
        QDocumentQmlScope::Ptr documentScope,
        QProjectQmlScope::Ptr project,
        const QString& importAs,
        const QString& name,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey
    ){
        QQmlLibraryInfo::ExportVersion ev = getType(
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
        QDocumentQmlScope::Ptr documentScope,
        QProjectQmlScope::Ptr project,
        const QString& name,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey
    ){
        QQmlLibraryInfo::ExportVersion ev = getType(
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
        QDocumentQmlScope::Ptr documentScope,
        QProjectQmlScope::Ptr projectScope,
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
        QDocumentQmlScope::Ptr scope,
        QProjectQmlScope::Ptr projectScope,
        QDocumentQmlInfo::ValueReference& documentValue,
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
        QDocumentQmlScope::Ptr scope,
        QProjectQmlScope::Ptr projectScope,
        QDocumentQmlInfo::ValueReference& parentValue,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& typeLibraryKey
    ){
        if ( str != "parent" )
            return false;

        QDocumentQmlInfo::ValueReference documentValue = scope->info()->valueAtPosition(position);
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
        QDocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(propertyPosition);
        if ( !scope.document->info()->isValueNull(documentValue) ){
            QDocumentQmlObject valueObject = scope.document->info()->extractValueObject(documentValue);

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
                        QDocumentQmlInfo::isObject(it.value()), 0
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
        QDocumentQmlScope::Ptr scope,
        QProjectQmlScope::Ptr projectScope,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        bool& isPointer,
        QString& libraryKey
    ){
        QDocumentQmlInfo::ValueReference documentValue = scope->info()->valueAtPosition(position);
        if ( !scope->info()->isValueNull(documentValue) ){
            QDocumentQmlObject valueObject = scope->info()->extractValueObject(documentValue);

            for (
                QMap<QString, QString>::const_iterator it = valueObject.memberProperties().begin();
                it != valueObject.memberProperties().end();
                ++it )
            {
                if ( it.key() == str ){
                    QString propertyType = it.value();
                    isPointer = QDocumentQmlInfo::isObject(propertyType);
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
        QDocumentQmlScope::Ptr documentScope,
        QProjectQmlScope::Ptr projectScope,
        QList<LanguageUtils::FakeMetaObject::ConstPtr>& typePath,
        QString& libraryKey)
    {
        QQmlLibraryInfo::ExportVersion ev = getType(
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
    bool isNamespace(const QString& str, QDocumentQmlScope::Ptr documentScope){
        foreach( const QDocumentQmlScope::ImportEntry& imp, documentScope->imports() ){
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
        QList<QCodeCompletionSuggestion>& suggestions
    ){
        foreach( LanguageUtils::FakeMetaObject::ConstPtr object, typePath ){
            QList<QCodeCompletionSuggestion> localSuggestions;

            QString objectTypeName = "";
            if ( object->exports().size() > 0 )
                objectTypeName = object->exports().first().type;

            if ( suggestProperties ){
                for ( int i = 0; i < object->propertyCount(); ++i ){
                    if ( !object->property(i).name().startsWith("__") ){
                        localSuggestions << QCodeCompletionSuggestion(
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
                        localSuggestions << QCodeCompletionSuggestion(
                            m.methodName() + "()", "function", objectTypeName, completion
                        );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestSignals ){
                    QString completion =
                        m.methodName() + "(" + m.parameterNames().join(", ") + ")";
                        localSuggestions << QCodeCompletionSuggestion(
                            m.methodName() + "()", "signal", objectTypeName, completion
                        );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestGeneratedSlots ){
                    QString methodName = m.methodName();
                    if ( methodName.size() > 0 )
                        methodName[0] = methodName[0].toUpper();
                    localSuggestions << QCodeCompletionSuggestion(
                        "on" + methodName, "slot", objectTypeName, "on" + methodName + suffix
                    );
                }
            }
            if ( suggestEnums ){
                for ( int i = 0; i < object->enumeratorCount(); ++i ){
                    LanguageUtils::FakeMetaEnum e = object->enumerator(i);
                    for ( int j = 0; j < e.keyCount(); ++j ){
                        localSuggestions << QCodeCompletionSuggestion(
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

                        localSuggestions << QCodeCompletionSuggestion(
                            "on" + propertyName + "Changed",
                            "slot",
                            objectTypeName,
                            "on" + propertyName + "Changed" + suffix
                        );
                    }
                }
            }

            std::sort(localSuggestions.begin(), localSuggestions.end(), &QCodeCompletionSuggestion::compare);
            suggestions << localSuggestions;
        }
    }
}

// QDocumentQmlHandler implementation
// ----------------------------------

QCodeQmlHandler::QCodeQmlHandler(
        QQmlEngine* engine,
        QMutex *engineMutex,
        QLockedFileIOSession::Ptr lockedFileIO,
        QObject *parent)
    : QAbstractCodeHandler(parent)
    , m_target(0)
    , m_highlighter(new QQmlJsHighlighter(0, 0))
    , m_settings(new QQmlJsSettings(m_highlighter))
    , m_engine(engine)
    , m_completionContextFinder(new QQmlCompletionContextFinder)
    , m_documentScope(0)
    , m_projectScope(0)
    , m_newScope(false)
    , m_scanner(new QProjectQmlScanner(engine, engineMutex, lockedFileIO))
{
    connect(m_scanner, SIGNAL(documentScopeReady()), SLOT(newDocumentScopeReady()) );
    connect(m_scanner, SIGNAL(projectScopeReady()), SLOT(newProjectScope()));
    connect(m_scanner, SIGNAL(requestObjectLoad(QString)), SLOT(loadImport(QString)));
}

QCodeQmlHandler::~QCodeQmlHandler(){
    delete m_scanner;
    delete m_completionContextFinder;
}

void QCodeQmlHandler::assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        QCodeCompletionModel* model,
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
            QQmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
            if ( !(ctx->context() & QQmlCompletionContext::InStringLiteral) ){
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
            QQmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
            if ( !(ctx->context() & QQmlCompletionContext::InStringLiteral) ){
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

    QQmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);
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
        QQmlCompletionContext* modelCtx = static_cast<QQmlCompletionContext*>(model->completionContext());
        if ( modelCtx && *modelCtx == *ctx && !m_newScope ){
            model->setFilter(filter);
            if ( model->rowCount() )
                model->enable();
            return;
        }
        model->removeCompletionContext();
    }
    m_newScope = false;

    QList<QCodeCompletionSuggestion> suggestions;
    if ( ctx->context() & QQmlCompletionContext::InStringLiteral ){
        if ( ctx->context() & QQmlCompletionContext::InImport ){
            suggestionsForStringImport(extractQuotedString(cursor), suggestions, filter);
            model->setSuggestions(suggestions, filter);
        } else {
            model->setSuggestions(suggestions, filter);
        }
    } else if ( ctx->context() & QQmlCompletionContext::InImport ){
        if ( ctx->context() & QQmlCompletionContext::InImportVersion ){
            model->setSuggestions(suggestions, filter);
        } else {
            suggestionsForImport(*ctx, suggestions);
            model->setSuggestions(suggestions, filter);
        }
    } else if ( ctx->context() & QQmlCompletionContext::InAfterOnLhsOfBinding ){
        suggestionsForLeftSignalBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx->context() & QQmlCompletionContext::InLhsOfBinding ){
        suggestionsForLeftBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx->context() & QQmlCompletionContext::InRhsofBinding ){
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

void QCodeQmlHandler::setTarget(QTextDocument *target, QDocumentHandlerState* state){
    m_target      = target;
    m_highlighter->setTarget(m_target, state);
}

void QCodeQmlHandler::setDocument(QProjectDocument *document){
    m_document      = document;
    m_documentScope = QDocumentQmlScope::createEmptyScope(m_projectScope);

    if ( !m_projectScope.isNull() && document != 0 ){
        m_scanner->scanDocumentScope(document->file()->path(), document->content(), m_projectScope.data());
        m_scanner->queueProjectScan();
    }
}

void QCodeQmlHandler::updateScope(const QString& data){
    if ( !m_projectScope.isNull() && m_document )
        m_scanner->queueDocumentScopeScan(m_document->file()->path(), data, m_projectScope.data());
}

void QCodeQmlHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_highlighter )
        m_highlighter->rehighlightBlock(block);
}

QList<QCodeDeclaration::Ptr> QCodeQmlHandler::getDeclarations(const QTextCursor& cursor){

    QList<QCodeDeclaration::Ptr> properties;
    int length = cursor.selectionEnd() - cursor.selectionStart();

    if ( length == 0 ){
        QQmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

        QStringList expression;
        int propertyPosition = ctx->propertyPosition();
        int propertyLength   = 0;

        if ( ctx->context() & QQmlCompletionContext::InLhsOfBinding ){
            expression = ctx->expressionPath();

            if ( propertyPosition == -1 ){
                if ( m_target->characterAt(cursor.position()).isSpace() ){
                    return properties;
                }
                else
                    propertyPosition = cursor.position();
            }

            int advancedLength = QDocumentQmlValueScanner::getPropertyLength(
                m_target, cursor.position(), &expression
            );
            propertyLength = (cursor.position() - propertyPosition) + advancedLength;

        } else if ( ctx->context() & QQmlCompletionContext::InRhsofBinding ){
            expression     = ctx->propertyPath();
            propertyLength = QDocumentQmlValueScanner::getPropertyLength(m_target, ctx->propertyPosition());
        }

        if ( expression.size() > 0 ){
            qmlhandler_helpers::ScopeCopy scope(m_projectScope, m_documentScope);
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
                }
            }

            if ( propertyType == "" ){
                propertyType = property.typeName();
            }

            if ( property.revision() != -1 ){
                properties.append(QCodeDeclaration::create(
                    expression, propertyType, ctx->objectTypePath(), propertyPosition, propertyLength, m_document
                ));
            }
        }

    } else { // multiple properties were selected

        QDocumentQmlInfo::Ptr docinfo = QDocumentQmlInfo::create(m_document->file()->path());
        docinfo->parse(m_target->toPlainText());

        QDocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

        QList<QDocumentQmlValueObjects::RangeProperty*> rangeProperties = objects->propertiesBetween(
            cursor.selectionStart(), cursor.selectionEnd()
        );

        if ( rangeProperties.size() > 0 ){

            qmlhandler_helpers::ScopeCopy scope(m_projectScope, m_documentScope);

            for( QList<QDocumentQmlValueObjects::RangeProperty*>::iterator it = rangeProperties.begin();
                 it != rangeProperties.end();
                 ++it )
            {
                QDocumentQmlValueObjects::RangeProperty* rp = *it;
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
                    properties.append(QCodeDeclaration::create(
                        rp->name(), propertyType, QStringList(), rp->begin, rp->propertyEnd - rp->begin, m_document
                    ));
                }
            }
        }
    }

    return properties;
}

bool QCodeQmlHandler::findPropertyValue(int position, int length, int &valuePosition, int &valueEnd){
    QDocumentQmlValueScanner vs(m_document, position, length);
    if ( vs() ){
        valuePosition = vs.valuePosition();
        valueEnd      = vs.valueEnd();
        return true;
    } else {
        valuePosition = -1;
        valueEnd      = -1;
        return false;
    }
}

void QCodeQmlHandler::connectBindings(QList<QCodeRuntimeBinding *> bindings, QObject *root){
    if ( m_document && m_document->isActive() )
        QDocumentQmlInfo::syncBindings(m_target->toPlainText(), m_document, bindings, root);
}

QDocumentEditFragment *QCodeQmlHandler::createInjectionChannel(
        QCodeDeclaration::Ptr declaration,
        QObject *runtime,
        QCodeConverter* converter)
{
    if ( m_document && m_document->isActive() ){

        QQmlProperty foundProperty(
            QDocumentQmlInfo::findMatchingProperty(m_target->toPlainText(), m_document, declaration, runtime)
        );
        if ( foundProperty.isValid() ){
            return new QDocumentQmlFragment(
                declaration,
                converter,
                foundProperty
            );
        }
    }

    return 0;
}

QPluginInfoExtractor* QCodeQmlHandler::getPluginInfoExtractor(const QString &import){
    if ( !QPluginTypesFacade::pluginTypesEnabled() ){
        qCritical("Plugin types not available in this build.");
        return 0;
    }

    QQmlLibraryDependency parsedImport = QQmlLibraryDependency::parse(import);
    if ( !parsedImport.isValid() ){
        qCritical("Invalid import: %s", qPrintable(import));
        return 0;
    }

    m_projectScope = QProjectQmlScope::create(m_engine);
    m_scanner->setProjectScope(m_projectScope);

    QList<QString> paths;
    m_projectScope->findQmlLibraryInImports(
        parsedImport.uri(),
        parsedImport.versionMajor(),
        parsedImport.versionMinor(),
        paths
    );

    if ( paths.size() == 0 ){
        qCritical("Failed to find import path: %s", qPrintable(import));
        return 0;
    }

    QPluginInfoExtractor* extractor = new QPluginInfoExtractor(m_scanner, paths.first(), this);
    connect(m_scanner, &QProjectQmlScanner::projectScopeReady, extractor, &QPluginInfoExtractor::newProjectScope);
    return extractor;
}

void QCodeQmlHandler::newDocumentScopeReady(){
    m_documentScope = m_scanner->lastDocumentScope();
    m_newScope = true;
}

void QCodeQmlHandler::newProjectScope(){
    m_newScope = true;
}

void QCodeQmlHandler::newProject(const QString &){
    m_projectScope = QProjectQmlScope::create(m_engine);
    m_documentScope = QDocumentQmlScope::createEmptyScope(m_projectScope);

    m_scanner->setProjectScope(m_projectScope);
}

void QCodeQmlHandler::directoryChanged(const QString &path){
    QDOCUMENT_QML_HANDLER_DEBUG("Reseting libraries in directory: " + path);

    QProjectQmlScope::Ptr project = m_projectScope;
    project->globalLibraries()->resetLibrariesInPath(path);
    project->implicitLibraries()->resetLibrariesInPath(path);
}

void QCodeQmlHandler::fileChanged(const QString &path){
    QFileInfo finfo(path);
    if ( finfo.fileName() == "" || finfo.suffix() != "qml" || !finfo.fileName().at(0).isUpper() )
        return;
    QString fileDir = finfo.path();

    QDOCUMENT_QML_HANDLER_DEBUG("Reseting library for file: " + path);
    QProjectQmlScope::Ptr project = m_projectScope;
    project->globalLibraries()->resetLibrary(fileDir);
    project->implicitLibraries()->resetLibrary(fileDir);
}

void QCodeQmlHandler::loadImport(const QString &import){
    QQmlComponent component(m_engine);
    QByteArray code = "import " + import.toUtf8() + "\nQtObject{}\n";

    QDOCUMENT_QML_HANDLER_DEBUG("Importing object for plugininfo: " + code);

    component.setData(code, QUrl::fromLocalFile("loading.qml"));
    if ( component.errors().size() > 0 ){
        QDOCUMENT_QML_HANDLER_DEBUG("Importing object error: " + component.errorString());
        m_scanner->updateLoadRequest(import, 0, true);
        return;
    }
    QObject* obj = component.create(m_engine->rootContext());
    if ( obj == 0 ){
        m_scanner->updateLoadRequest(import, 0, true);
    } else {
        m_scanner->updateLoadRequest(import, obj, false);
    }
}

void QCodeQmlHandler::suggestionsForGlobalQmlContext(
        const QQmlCompletionContext &,
        QList<QCodeCompletionSuggestion> &suggestions
){
    suggestions << QCodeCompletionSuggestion("import", "", "", "import ");
    suggestions << QCodeCompletionSuggestion("pragma singleton", "", "", "pragma singleton");
}

void QCodeQmlHandler::suggestionsForImport(
        const QQmlCompletionContext& context,
        QList<QCodeCompletionSuggestion> &suggestions)
{

    foreach (const QString& importPath, m_engine->importPathList()){
        suggestionsForRecursiveImport(0, importPath, context.expressionPath(), suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &QCodeCompletionSuggestion::compare);
}

void QCodeQmlHandler::suggestionsForStringImport(
        const QString& enteredPath,
        QList<QCodeCompletionSuggestion> &suggestions,
        QString& filter)
{
    if ( m_document ){
        QStringList enteredPathSegments = enteredPath.split('/');
        filter = enteredPathSegments.size() > 0 ? enteredPathSegments.last() : "";
        QString path = m_document->file()->path();
        QString dirPath = QFileInfo(path).path();
        suggestionsForRecursiveImport(0, dirPath, enteredPathSegments, suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &QCodeCompletionSuggestion::compare);
}

void QCodeQmlHandler::suggestionsForRecursiveImport(
        int index,
        const QString& dir,
        const QStringList& expression,
        QList<QCodeCompletionSuggestion> &suggestions)
{
    if ( index >= expression.size() - 1 ){
        QDirIterator dit(dir);
        while( dit.hasNext() ){
            dit.next();
            if ( dit.fileInfo().isDir() && dit.fileName() != "." && dit.fileName() != ".."){
                suggestions << QCodeCompletionSuggestion(dit.fileName(), "", "", dit.fileName());
            }
        }
    } else {
        QString path = dir + (dir.endsWith("/") ? "" : "/") + expression[index];
        ++index;
        suggestionsForRecursiveImport(index, path, expression, suggestions);
    }
}

void QCodeQmlHandler::suggestionsForValueObject(
        const QDocumentQmlObject &object,
        QList<QCodeCompletionSuggestion> &suggestions,
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
            suggestions << QCodeCompletionSuggestion(it.key(), it.value(), object.typeName(), it.key());
        }
    }
    if ( extractFunctions ){
        for (
            QMap<QString, QDocumentQmlObject::FunctionValue>::const_iterator it = object.memberFunctions().begin();
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

            suggestions << QCodeCompletionSuggestion(it.value().name + "()", "function", object.typeName(), completion);
        }
    }
    if ( extractSignals ){
        for (
            QMap<QString, QDocumentQmlObject::FunctionValue>::const_iterator it = object.memberSignals().begin();
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

            suggestions << QCodeCompletionSuggestion(it.value().name + "()", "signal", object.typeName(), completion);
        }
    }
    if ( extractSlots ){
        for (
            QMap<QString, QString>::const_iterator it = object.memberSlots().begin();
            it != object.memberSlots().end();
            ++it )
        {
            suggestions << QCodeCompletionSuggestion(it.key(), "slot", object.typeName(), it.key());
        }
    }
}

void QCodeQmlHandler::suggestionsForNamespaceTypes(
    const QString &typeNameSpace,
    QList<QCodeCompletionSuggestion> &suggestions)
{
    QProjectQmlScope::Ptr projectScope = m_projectScope;
    QDocumentQmlScope::Ptr document    = m_documentScope;

    if ( typeNameSpace.isEmpty() ){
        QStringList exports;
        projectScope->implicitLibraries()->libraryInfo(document->path())->listExports(&exports);

        foreach( const QString& e, exports ){
            if ( e != document->componentName() )
                suggestions << QCodeCompletionSuggestion(e, "", "implicit", e);
        }


        foreach( const QString& defaultLibrary, projectScope->defaultLibraries() ){
            QStringList exports;
            projectScope->globalLibraries()->libraryInfo(defaultLibrary)->listExports(&exports);
            foreach( const QString& e, exports ){
                suggestions << QCodeCompletionSuggestion(e, "", "QtQml", e);
            }
        }
    }

    foreach( const QDocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() == typeNameSpace ){
            QStringList exports;
            projectScope->globalLibraries()->libraryInfo(imp.second)->listExports(&exports);
            int segmentPosition = imp.second.lastIndexOf('/');
            QString libraryName = imp.second.mid(segmentPosition + 1);
            
            foreach( const QString& e, exports ){
                suggestions << QCodeCompletionSuggestion(e, "", libraryName, e);
            }
        }
    }
}

void QCodeQmlHandler::suggestionsForNamespaceImports(QList<QCodeCompletionSuggestion> &suggestions){
    QMap<QString, QString> imports;
    QDocumentQmlScope::Ptr document = m_documentScope;

    QList<QCodeCompletionSuggestion> localSuggestions;

    foreach( const QDocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() != "" ){
            imports[imp.first.as()] = imp.first.path();
        }
    }
    for ( QMap<QString, QString>::iterator it = imports.begin(); it != imports.end(); ++it ){
        localSuggestions << QCodeCompletionSuggestion(it.key(), "import", it.value(), it.key());
    }
    std::sort(localSuggestions.begin(), localSuggestions.end(), &QCodeCompletionSuggestion::compare);

    suggestions << localSuggestions;
}

void QCodeQmlHandler::suggestionsForDocumentsIds(QList<QCodeCompletionSuggestion> &suggestions){
    QStringList ids = m_documentScope->info()->extractIds();
    ids.sort();
    foreach( const QString& id, ids ){
        suggestions << QCodeCompletionSuggestion(id, "id", "", id);
    }
}

void QCodeQmlHandler::suggestionsForLeftBind(
        const QQmlCompletionContext& context,
        int cursorPosition,
        QList<QCodeCompletionSuggestion> &suggestions)
{
    if ( context.objectTypePath().size() == 0 )
        return;
    
    QDocumentQmlScope::Ptr documentScope = m_documentScope;
    QProjectQmlScope::Ptr projectScope = m_projectScope;

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
        suggestions << QCodeCompletionSuggestion("function", "", "", "function ");
        suggestions << QCodeCompletionSuggestion("property", "", "", "property ");
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

void QCodeQmlHandler::suggestionsForRightBind(
        const QQmlCompletionContext& context,
        int cursorPosition,
        QList<QCodeCompletionSuggestion> &suggestions)
{
    QDocumentQmlScope::Ptr documentScope = m_documentScope;
    QProjectQmlScope::Ptr projectScope   = m_projectScope;

    if ( context.expressionPath().size() > 1 ){
        QString firstSegment = context.expressionPath()[0];
        QDocumentQmlInfo::ValueReference value;
        QList<LanguageUtils::FakeMetaObject::ConstPtr> typePath;
        QString typeLibraryKey;

//        QString propertyType;
        bool isPointer = false;

        if (qmlhandler_helpers::isId(firstSegment, documentScope, projectScope, value, typePath, typeLibraryKey)){

            QDocumentQmlObject valueObj = documentScope->info()->extractValueObject(value);
            int startSegment = 1;

            if ( startSegment < context.expressionPath().size() - 1 ){
                if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
                    QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
                    if ( QDocumentQmlInfo::isObject(valueType) ){
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
            QDocumentQmlObject valueObj = documentScope->info()->extractValueObject(value);
            int startSegment = 1;

            if ( startSegment < context.expressionPath().size() - 1 ){
                if ( valueObj.memberProperties().contains(context.expressionPath()[startSegment])){
                    QString valueType = valueObj.memberProperties()[context.expressionPath()[startSegment]];
                    if ( QDocumentQmlInfo::isObject(valueType) ){
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
                QQmlLibraryInfo::ExportVersion ev = qmlhandler_helpers::getType(
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
            suggestions << QCodeCompletionSuggestion(type, "id", "", type);
        } else {

            /// other cases
            suggestions << QCodeCompletionSuggestion("parent", "id", "", "parent");
            suggestionsForDocumentsIds(suggestions);
            QDocumentQmlInfo::ValueReference documentValue = documentScope->info()->valueAtPosition(cursorPosition);
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

void QCodeQmlHandler::suggestionsForLeftSignalBind(
        const QQmlCompletionContext& context,
        int cursorPosition,
        QList<QCodeCompletionSuggestion> &suggestions)
{
    QDocumentQmlScope::Ptr documentScope = m_documentScope;
    QProjectQmlScope::Ptr projectScope = m_projectScope;

    QDocumentQmlInfo::ValueReference documentValue = documentScope->info()->valueAtPosition(cursorPosition);
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

QString QCodeQmlHandler::extractQuotedString(const QTextCursor &cursor) const{
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
