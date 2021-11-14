#ifndef LVVIEWENGINEINTERCEPTOR_P_H
#define LVVIEWENGINEINTERCEPTOR_P_H

#include "live/lvviewglobal.h"
#include "live/packagegraph.h"
#include "live/viewengine.h"
#include "live/viewengineinterceptor.h"

#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QQmlAbstractUrlInterceptor>
#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkReply>

namespace lv{

class MemoryNetworkReply : public QNetworkReply{

    Q_OBJECT

public:
    MemoryNetworkReply( QObject *parent=nullptr): QNetworkReply(parent) {}
    ~MemoryNetworkReply() {}

    void setContent( const QString &content );

    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;

protected:
    qint64 readData(char *data, qint64 maxSize);

private:
    QByteArray m_content;
    qint64     m_offset;
};

/// \private
class QmlEngineInterceptor : public QNetworkAccessManager{

public:
    class UrlInterceptor : public QQmlAbstractUrlInterceptor{
    public:
        UrlInterceptor(ViewEngine* engine);

        void setInterceptor(ViewEngineInterceptor* interceptor);

        QUrl intercept(const QUrl &path, DataType type);

    private:
        ViewEngine*            m_engine;
        ViewEngineInterceptor* m_interceptor;
    };

    class Factory : public QQmlNetworkAccessManagerFactory{
    public:
        Factory(ViewEngineInterceptor* interceptor);

        virtual QNetworkAccessManager *create(QObject *parent);

    private:
        ViewEngineInterceptor* m_interceptor;
    };

public:
    explicit QmlEngineInterceptor(ViewEngineInterceptor* interceptor, QObject* parent = Q_NULLPTR)
        : QNetworkAccessManager(parent), m_interceptor(interceptor)
    {}

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

private:
    ViewEngineInterceptor* m_interceptor;
};

}// namespace

#endif // LVVIEWENGINEINTERCEPTOR_P_H
