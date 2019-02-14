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

#ifndef LVVIEWENGINE_H
#define LVVIEWENGINE_H

#include <QObject>
#include <QJSValue>
#include <QMap>
#include <QLinkedList>
#include <functional>

#include "live/lvviewglobal.h"
#include "live/exception.h"
#include "live/typeinfo.h"
#include "live/mlnode.h"

class QQmlEngine;
class QQmlError;
class QQmlIncubator;
class QMutex;

namespace lv{

class ErrorHandler;
class IncubationController;

/**
 * \class lv::FatalException
 * \brief Subclass of the lv::Exception used for unrecoverable errors
 *
 *
 * \ingroup lvview
 */
class LV_VIEW_EXPORT FatalException : public lv::Exception{

public:
    /** Default costructor */
    FatalException(const std::string& message, int code = 0): lv::Exception(message, code){}
    /** QString variant of the default constructor */
    FatalException(const QString& message, int code = 0) : lv::Exception(message.toStdString(), code){}
    /** Default destructor */
    virtual ~FatalException(){}
};

/**
 * \class lv::InputException
 * \brief Subclass of the lv::Exception used for configuration errors
 * \ingroup lvview
 */
class LV_VIEW_EXPORT InputException : public lv::Exception{
public:
    /** Defautlt constructor */
    InputException(const std::string& message, int code = 0) : lv::Exception(message, code){}
    /** QString variant of the default constructor */
    InputException(const QString& message, int code = 0) : lv::Exception(message.toStdString(), code){}
    /** Default destructor */
    virtual ~InputException(){}
};


class LV_VIEW_EXPORT ViewEngine : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    /** Callback function type to be run after the engine finishes compiling */
    typedef void(*CompileHook)(const QString&, const QUrl&, QObject*, QObject*, void*);

private:
    class CompileHookEntry{
    public:
        CompileHook m_hook;
        void*       m_userData;
    };

public:
    explicit ViewEngine(QQmlEngine* engine, QObject *parent = 0);
    ~ViewEngine();

    bool isLoading() const;
    void setIsLoading(bool isLoading);

    void useEngine(std::function<void(QQmlEngine*)> call);

    const QList<QQmlError>& lastErrors() const;

    QQmlEngine* engine();
    QMutex* engineMutex();

    QJSValue evaluate(const QString& jsCode, const QString &fileName = QString(), int lineNumber = 1);
    void throwError(const Exception *e, QObject* object = 0);
    void throwError(const QQmlError& error);

    void throwWarning(const QString& message, QObject* object = 0, const QString& fileName = QString(), int lineNumber = 0);

    bool hasErrorHandler(QObject* object);
    void registerErrorHandler(QObject* object, ErrorHandler* handler);
    void removeErrorHandler(QObject* object);

    void addCompileHook(CompileHook ch, void* userData);
    void removeCompileHook(CompileHook ch, void* userData);

    template<typename T> TypeInfo::Ptr registerQmlTypeInfo(
        const std::function<void(const T&, MLNode&)>& serializeFunction,
        const std::function<void(const MLNode&, T&)>& deserializeFunction,
        const std::function<QObject*()>& constructorFunction,
        bool canLog
    );

    TypeInfo::Ptr typeInfo(const QMetaObject *type) const;
    TypeInfo::Ptr typeInfo(const QByteArray& typeName) const;
    TypeInfo::Ptr typeInfo(const QMetaType& metaType) const;

signals:
    /** Signals that we're about to create an object of our compiled code */
    void aboutToCreateObject(const QUrl& file);
    /** Loading indicator has changed */
    void isLoadingChanged(bool isLoading);
    /** Object was created */
    void objectCreated(QObject* object);
    /** Error in object creation */
    void objectCreationError(QJSValue errors);

    /** Emitted when the error is propagated all the way to the application */
    void applicationError(QJSValue error);
    /** Emitted when the warning is propagated all the way to the application  */
    void applicationWarning(QJSValue warning);

public slots:
    void createObjectAsync(
        const QString& qmlCode,
        QObject* parent,
        const QUrl& file,
        QObject *attachment,
        bool clearCache = false
    );
    QObject* createObject(const QString& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);
    void engineWarnings(const QList<QQmlError>& warnings);

    void throwError(const QJSValue& error, QObject* object = 0);
    void throwWarning(const QJSValue& error, QObject* object = 0);
#ifndef DOXYGEN_PRIVATE
    /// @private
    QString markErrorObject(QObject* object);
    /// @private
    QJSValue lastErrorsObject() const;
#endif
private:
    QJSValue toJSError(const QQmlError& error) const;
    QJSValue toJSErrors(const QList<QQmlError>& errors) const;

    QQmlEngine*    m_engine;
    QMutex*        m_engineMutex;
    QQmlIncubator* m_incubator;
    IncubationController* m_incubationController;
    QJSValue       m_errorType;

    QLinkedList<CompileHookEntry> m_compileHooks;

    QList<QQmlError>              m_lastErrors;
    QMap<QObject*, ErrorHandler*> m_errorHandlers;
    QMap<QString,  QObject*>      m_errorObjects;

    QHash<const QMetaObject*, TypeInfo::Ptr> m_types;
    QHash<QString, const QMetaObject*>       m_typeNames;

    bool m_isLoading;
};

/**
 * \brief Allows the engine to register info about a type
 *
 * We pass serialization functions that transforms the type into MLNode, which enables logging, as well as a constructor
 * and a canLog parameters which shows if logging is possible for this type.
 */
template<typename T>
TypeInfo::Ptr ViewEngine::registerQmlTypeInfo(
        const std::function<void(const T&, MLNode&)>& serializeFunction,
        const std::function<void(const MLNode&, T&)>& deserializeFunction,
        const std::function<QObject*()>& constructorFunction,
        bool canLog)
{
    TypeInfo::Ptr t = TypeInfo::create(T::staticMetaObject.className());
    t->addSerialization<T>(serializeFunction, deserializeFunction);
    t->addConstructor(constructorFunction);
    if ( canLog )
        t->addLogging<T>();
    m_types[&T::staticMetaObject] = t;
    m_typeNames[T::staticMetaObject.className()] = &T::staticMetaObject;

    return t;
}

/** Shows if the engine is loading */
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

}// namespace

#endif // LVVIEWENGINE_H
