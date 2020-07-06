#include "qmlerror.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

#include <QJSValueIterator>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

namespace lv{

QmlError::QmlError()
    : m_engine(nullptr)
{
}

QmlError::QmlError(const Exception &e, QObject *engineObject)
    : m_engine(nullptr)
{
    m_engine = ViewEngine::grab(engineObject);
    if ( !m_engine ){
        vlog().w() << "QmlError: Failed to initialize engine from object in QmlError. This object will not throw.";
    } else {
        initializeFromException(e, engineObject);
    }
}

QmlError::QmlError(ViewEngine *engine, QJSValue error)
    : m_engine(engine)
{
    m_error = error;
}

QmlError::QmlError(ViewEngine *engine, const QString &message, qint64 code, QObject *object)
    : m_engine(engine)
{
    m_error = m_engine->engine()->newErrorObject(QJSValue::GenericError, message);
    m_error.setProperty("code", static_cast<double>(code));
    if ( object )
        m_error.setProperty("object", m_engine->engine()->newQObject(object));
}

QmlError::QmlError(ViewEngine* engine, const Exception &e, QObject *object)
    : m_engine(engine)
{
    initializeFromException(e, object);
}

QmlError::QmlError(ViewEngine* engine, const QQmlError &error)
    : m_engine(engine)
{
    m_error = m_engine->engine()->newErrorObject(QJSValue::GenericError, error.description());
    m_error.setProperty("type", "Error");
    assignLocation(error.url().toString(), error.line(), "");

    if ( error.object() ){
        assignObject(error.object());
    }
}

QmlError::~QmlError(){
}


void QmlError::initializeFromException(const Exception &e, QObject *object){
    m_error = m_engine->engine()->newErrorObject(QJSValue::GenericError, QString::fromStdString(e.message()));
    m_error.setProperty("code", static_cast<double>(e.code()));

    if ( e.hasLocation() )
        assignLocation(QString::fromStdString(e.file()), e.line(), QString::fromStdString(e.functionName()));

    if ( object )
        assignObject(object);

    if ( e.hasStackTrace() ){
        StackTrace::Ptr st = e.stackTrace();
        QJSValue stackTrace = m_engine->engine()->newArray(static_cast<quint32>(st->size()));
        quint32 i = 0;
        for ( auto it = st->begin(); it != st->end(); ++it ){
            stackTrace.setProperty(i++, QString::fromStdString(it->functionName()) + "(" + it->fileName().c_str() + ":" + QString::number(it->line()) + ")");
        }
        assignCStackTrace(stackTrace);
    }

    if ( dynamic_cast<const lv::FatalException*>(&e) != nullptr ){
        m_error.setProperty("type", "FatalException");
    } else if ( dynamic_cast<const lv::InputException*>(&e) != nullptr ){
        m_error.setProperty("type", "ConfigurationException");
    } else {
        m_error.setProperty("type", "Exception");
    }
}

void QmlError::assignLocation(const QString &fileName, int lineNumber, const QString &functionName){
    m_error.setProperty("fileName", fileName);
    m_error.setProperty("lineNumber", lineNumber);
    m_error.setProperty("functionName", functionName);
}

void QmlError::assignObject(QObject *object){
    m_error.setProperty("object", m_engine->engine()->newQObject(object));
}

void QmlError::assignCStackTrace(const QJSValue &trace){
    m_error.setProperty("cStackTrace", trace);
}

bool QmlError::isNull() const{
    return !m_engine;
}

QString QmlError::messageWithId() const{
    return message() + "(id:" + m_id + ")";
}

QString QmlError::message() const{
    return m_error.property("message").toString();
}

QObject *QmlError::object() const{
    return m_error.property("object").toQObject();
}

QString QmlError::toString(int options) const{
    QString result;

    if ( options & QmlError::PrintMessage )
        result = message() + "(code:" + m_error.property("code").toString() + ")";

    if ( options & QmlError::PrintLocation ){

        if ( m_error.hasOwnProperty("fileName") ){
            result += "\nat " +
                    m_error.property("fileName").toString() + ":" +
                    m_error.property("lineNumber").toString() + "@" +
                    m_error.property("functionName").toString();
        }
    }

    if ( options & QmlError::PrintStackTrace ){
        if ( m_error.hasOwnProperty("stackTrace") ){
            result += "\nStackTrace:";
            QJSValueIterator stackIt(m_error.property("stackTrace"));
            while ( stackIt.hasNext() ){
                stackIt.next();
                result += "\n" + stackIt.value().toString();
            }
        } else if ( m_error.hasOwnProperty("stack") ){
            result += "\nStackTrace:\n" + m_error.property("stack").toString();
        }
    }

    if ( options & QmlError::PrintCStackTrace ){
        if ( m_error.hasOwnProperty("cStackTrace") ){
            result += "\nInternal StackTrace:";
            QJSValueIterator stackIt(m_error.property("cStackTrace"));
            while ( stackIt.hasNext() ){
                stackIt.next();
                result += "\n" + stackIt.value().toString();
            }
        }
    }

    return result;
}

void QmlError::jsThrow(){
    if ( m_engine ){
        m_engine->throwError(*this);
    } else {
        qWarning("QmlError: Attempting to throw a null error.");
    }
}

}// namespace
