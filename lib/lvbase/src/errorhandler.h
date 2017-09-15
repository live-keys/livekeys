#ifndef LVERRORHANDLER_H
#define LVERRORHANDLER_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lcv{

class Engine;
class ErrorHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QObject* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit ErrorHandler(QObject *parent = 0);
    ~ErrorHandler();

    void componentComplete();
    void classBegin(){}

    void signalError(const QJSValue& error);
    void signalWarning(const QJSValue& error);

    void setTarget(QObject* target);
    QObject* target() const;

signals:
    void error(QJSValue e);
    void fatal(QJSValue e);
    void warning(QJSValue e);

    void targetChanged(QObject* target);

public slots:
    void skip(const QJSValue& error);

private:
    Engine*  m_engine;
    QObject* m_target;
    bool     m_componentComplete;
};

inline QObject *ErrorHandler::target() const{
    return m_target;
}

}// namespace

#endif // LVERRORHANDLER_H
