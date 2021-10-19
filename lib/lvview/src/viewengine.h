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

#ifndef LVVIEWENGINE_H
#define LVVIEWENGINE_H

#include <QObject>
#include <QJSValue>
#include <QMap>
#include <QLinkedList>
#include <functional>

#include "live/lvviewglobal.h"
#include "live/exception.h"
#include "live/metainfo.h"
#include "live/mlnode.h"
#include "live/qmlerror.h"

class QQmlEngine;
class QQmlError;
class QQmlContext;
class QQmlIncubator;
class QMutex;

namespace lv{

class Memory;
class ErrorHandler;
class PackageGraph;
class ComponentDeclaration;
class IncubationController;

class LV_VIEW_EXPORT FatalException : public lv::Exception{

public:
    /** Default costructor */
    FatalException(const std::string& message, Exception::Code code = 0): lv::Exception(message, code){}
    /** QString overload of the default constructor */
    FatalException(const QString& message, Exception::Code code = 0) : lv::Exception(message.toStdString(), code){}
    /** Default destructor */
    virtual ~FatalException(){}
};

class LV_VIEW_EXPORT InputException : public lv::Exception{
public:
    /** Defautlt constructor */
    InputException(const std::string& message, Exception::Code code = 0) : lv::Exception(message, code){}
    /** QString overload of the default constructor */
    InputException(const QString& message, Exception::Code code = 0) : lv::Exception(message.toStdString(), code){}
    /** Default destructor */
    virtual ~InputException(){}
};


class LV_VIEW_EXPORT ViewEngine : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool isLoading   READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(lv::Memory* mem  READ memory    CONSTANT)

public:
    class LV_VIEW_EXPORT ComponentResult{

    public:
        typedef QSharedPointer<ComponentResult> Ptr;

    public:
        static Ptr create(QObject* o = nullptr, QQmlComponent* c = nullptr);
        ~ComponentResult();

        bool hasError() const{ return errors.size(); }
        void jsThrowError();

        QList<QmlError> errors;
        QObject*        object;
        QQmlComponent*  component;

    private:
        ComponentResult(QObject* o = nullptr, QQmlComponent* c = nullptr) : object(o), component(c){}

        ComponentResult(const ComponentResult&);
        ComponentResult& operator=(const ComponentResult&);
    };

public:
    explicit ViewEngine(QQmlEngine* engine, QObject *parent = nullptr);
    ~ViewEngine();

    /** Shows if the engine is loading */
    bool isLoading() const;
    void setIsLoading(bool isLoading);

    Memory* memory();

    const QList<QQmlError>& lastErrors() const;

    QQmlEngine* engine();
    QMutex* engineMutex();

    QJSValue evaluate(const QString& jsCode, const QString &fileName = QString(), int lineNumber = 1);
    void throwError(const lv::QmlError& error);
    void throwError(const Exception *e, QObject* object = nullptr);

    bool hasErrorHandler(QObject* object);
    void registerErrorHandler(QObject* object, ErrorHandler* handler);
    void removeErrorHandler(QObject* object);

    template<typename T> MetaInfo::Ptr registerQmlTypeInfo(
        const std::function<void(const T&, MLNode&)>& serializeFunction,
        const std::function<void(const MLNode&, T&)>& deserializeFunction,
        const std::function<QObject*()>& constructorFunction,
        bool canLog
    );

    MetaInfo::Ptr typeInfo(const QMetaObject *type) const;
    MetaInfo::Ptr typeInfo(const QByteArray& typeName) const;
    MetaInfo::Ptr typeInfo(const QMetaType& metaType) const;

    ComponentDeclaration rootDeclaration(QObject* object) const;

    static QString typeAsPropertyMessage(const QString& typeName, const QString& propertyName);

    static void registerBaseTypes(const char* uri);
    static void initializeBaseTypes(ViewEngine* engine);

    static QJSValue typeDefaultValue(const QString& typeInfo, ViewEngine* engine = nullptr);

    static QString toErrorString(const QQmlError& error);
    static QString toErrorString(const QList<QQmlError> &errors);

    QObject* createObject(const QByteArray& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);
    QObject* createObject(const char* qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);

    QJSValue toJSErrors(const QList<QQmlError>& errors) const;
    QJSValue toJSError(const QQmlError& error) const;

    const PackageGraph* packageGraph() const;
    void setPackageGraph(PackageGraph* pg);

    static void printTrace(QJSEngine* engine);
    static ViewEngine* grab(const QObject *object);

    QmlError findError(const QString& message) const;
    QmlError findError(QJSValue error) const;

    ComponentResult::Ptr createPluginObject(const QString& filePath, QObject* parent);
    ComponentResult::Ptr createObject(const QString& filePath, QObject* parent, QQmlContext* context = nullptr);
    ComponentResult::Ptr createObject(const QUrl& filePath, QObject* parent, QQmlContext* context = nullptr);
    ComponentResult::Ptr createObject(const QString& filePath, const QByteArray& source, QObject* parent, QQmlContext* context = nullptr);

    ComponentResult::Ptr compileJsModule(const QByteArray &imports, const QByteArray &source, const QString& moduleFile);

signals:
    /** Signals before compiling a new object. */
    void aboutToCreateObject(const QUrl& file);
    /** Signals after acquiring a new object, assuring no errors were found. */
    void objectAcquired(const QUrl& file, QObject* reference);
    /** Loading indicator has changed */
    void isLoadingChanged(bool isLoading);
    /** Object was created */
    void objectReady(QObject* object, const QUrl& file, QObject* reference, QQmlContext* context);
    /** Error in object creation */
    void objectCreationError(QJSValue errors, const QUrl& file, QObject* reference, QQmlContext* context);

    /** Emitted when the error is propagated all the way to the application */
    void applicationError(QJSValue error);
    /** Emitted when the warning is propagated all the way to the application  */
    void applicationWarning(QJSValue warning);

public slots:
    void createObjectAsync(
        const QString& qmlCode,
        QObject* parent,
        const QUrl& file,
        QObject* reference = nullptr,
        QQmlContext* context = nullptr,
        bool clearCache = false
    );
    QObject* createObject(const QString& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);
    QQmlComponent* createComponent(const QString& qmlCode, const QUrl& file);
    void engineWarnings(const QList<QQmlError>& warnings);

    void throwError(const QJSValue& error, QObject* object = nullptr);
    QJSValue unwrapError(QJSValue error) const;

    /// \private
    QString markErrorObject(QJSValue error, QObject* object);
    /// \private
    QJSValue lastErrorsObject() const;

private:
    void storeError(const QmlError& error);
    bool propagateError(const QmlError &error);

    QQmlEngine*           m_engine;
    QMutex*               m_engineMutex;
    QQmlIncubator*        m_incubator;
    IncubationController* m_incubationController;
    QJSValue              m_errorType;
    PackageGraph*         m_packageGraph;

    QList<QQmlError>              m_lastErrors;
    int                           m_errorCounter;
    QMap<QObject*, ErrorHandler*> m_errorHandlers;
    QMap<QString,  QmlError>      m_errorObjects;

    QHash<const QMetaObject*, MetaInfo::Ptr> m_types;
    QHash<QString, const QMetaObject*>       m_typeNames;

    lv::Memory* m_memory;
    bool        m_isLoading;
};

/**
 * \brief Allows the engine to register info about a type
 *
 * Store a constructor, serialization functions and a logging flag for this type.
 */
template<typename T>
MetaInfo::Ptr ViewEngine::registerQmlTypeInfo(
        const std::function<void(const T&, MLNode&)>& serializeFunction,
        const std::function<void(const MLNode&, T&)>& deserializeFunction,
        const std::function<QObject*()>& constructorFunction,
        bool canLog)
{
    MetaInfo::Ptr t = MetaInfo::create(T::staticMetaObject.className());
    t->addSerialization<T>(serializeFunction, deserializeFunction);
    t->addConstructor(constructorFunction);
    if ( canLog )
        t->addLogging<T>();
    m_types[&T::staticMetaObject] = t;
    m_typeNames[T::staticMetaObject.className()] = &T::staticMetaObject;

    return t;
}

inline bool ViewEngine::isLoading() const{
    return m_isLoading;
}

/** Sets the loading indicator of the object */
inline void ViewEngine::setIsLoading(bool isLoading){
    m_isLoading = isLoading;
}

/** The contained QQmlEngine */
inline QQmlEngine*ViewEngine::engine(){
    return m_engine;
}

/** The engine mutex, which is used to lock the engine for use */
inline QMutex *ViewEngine::engineMutex(){
    return m_engineMutex;
}

/** Retrieves the memory object */
inline Memory *ViewEngine::memory(){
    return m_memory;
}

/** Returns the package graph of the engine */
inline const PackageGraph *ViewEngine::packageGraph() const{
    return m_packageGraph;
}

}// namespace

#endif // LVVIEWENGINE_H
