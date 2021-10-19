#ifndef LVQMLERROR_H
#define LVQMLERROR_H

#include <QObject>
#include <QJSValue>
#include <QQmlEngine>

#include "live/lvviewglobal.h"
#include "live/exception.h"

namespace lv{

class ViewEngine;

class LV_VIEW_EXPORT QmlError{

public:
    friend class ViewEngine;

    enum Print{
        None = 0,
        PrintMessage = 1,
        PrintLocation = 2,
        PrintExtra = 4,
        PrintStackTrace = 8,
        PrintCStackTrace = 16
    };

public:
    QmlError();
    QmlError(const lv::Exception& e, QObject* engineObject);
    QmlError(ViewEngine* engine, QJSValue error);
    QmlError(ViewEngine* engine, const QString& message, qint64 code, QObject* object = nullptr);
    QmlError(ViewEngine* engine, const lv::Exception& exception, QObject* object = nullptr);
    QmlError(ViewEngine* engine, const QQmlError& error);
    ~QmlError();

    void assignLocation(const QString& fileName, int lineNumber, const QString& functionName);
    void assignObject(QObject* object);
    void assignCStackTrace(const QJSValue& trace);

    const QJSValue& value() const;

    bool isNull() const;

    double code() const;
    QString messageWithId() const;
    QString message() const;
    QObject* object() const;
    bool hasLocation() const;
    QString fileName() const;
    int lineNumber() const;
    QString functionName() const;

    QString toString(int options = QmlError::PrintMessage | QmlError::PrintLocation) const;

    void jsThrow();

    static void warnNoEngineCaptured(QObject* object, QString prefix = "");

    static QString toString(const QList<QmlError>& errors, int options = QmlError::PrintMessage | QmlError::PrintLocation);
    static QmlError join(const QList<QmlError>& errors);

private:
    void initializeFromException(const lv::Exception& e, QObject* object = nullptr);

    ViewEngine*     m_engine;
    QJSValue        m_error;
    mutable QString m_id;

};

inline const QJSValue &QmlError::value() const{
    return m_error;
}

#define THROW_QMLERROR(_exceptionType, _message, _code, _object) \
    lv::QmlError(CREATE_EXCEPTION(_exceptionType, _message, _code), _object).jsThrow()

}// namespace

#endif // LVQMLERROR_H
