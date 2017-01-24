#ifndef QLIVECVENGINE_H
#define QLIVECVENGINE_H

#include <QObject>
#include <QJSValue>
#include <functional>


class QQmlEngine;
class QQmlError;
class QQmlIncubator;
class QLiveCVIncubationController;
class QMutex;

namespace lcv{

class QLiveCVEngine : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit QLiveCVEngine(QQmlEngine* engine, QObject *parent = 0);
    ~QLiveCVEngine();

    bool isLoading() const;
    void setIsLoading(bool isLoading);

    void useEngine(std::function<void(QQmlEngine*)> call);

    const QList<QQmlError>& lastErrors() const;

    QQmlEngine* engine();
    QMutex* engineMutex();

signals:
    void aboutToCreateObject(const QUrl& file);
    void isLoadingChanged(bool isLoading);
    void objectCreated(QObject* object);
    void objectCreationError(QJSValue errors);

public slots:
    void createObjectAsync(const QString& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);
    QObject* createObject(const QString& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);

    QJSValue lastErrorsObject() const;

private:
    QJSValue toJSErrors(const QList<QQmlError>& errors) const;

    QQmlEngine*    m_engine;
    QMutex*        m_engineMutex;
    QQmlIncubator* m_incubator;
    QLiveCVIncubationController* m_incubationController;

    QList<QQmlError> m_lastErrors;

    bool m_isLoading;
};

inline bool QLiveCVEngine::isLoading() const{
    return m_isLoading;
}

inline void QLiveCVEngine::setIsLoading(bool isLoading){
    m_isLoading = isLoading;
}

inline QQmlEngine*QLiveCVEngine::engine(){
    return m_engine;
}

inline QMutex *QLiveCVEngine::engineMutex(){
    return m_engineMutex;
}

}// namespace

#endif // QLIVECVENGINE_H
