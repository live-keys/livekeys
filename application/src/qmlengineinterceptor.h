#ifndef LVQMLENGINEINTERCEPTOR_H
#define LVQMLENGINEINTERCEPTOR_H

#include "live/lvviewglobal.h"
#include "live/packagegraph.h"
#include "live/viewengine.h"
#include "live/project.h"

#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QQmlAbstractUrlInterceptor>
#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkReply>

namespace lv{

class MemoryNetworkReply : public QNetworkReply
{
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
        UrlInterceptor(ViewEngine* engine, PackageGraph* PackageGraph, Project* project);

        QUrl intercept(const QUrl &path, DataType type);

    private:
        Project*      m_project;
        ViewEngine*   m_engine;
        PackageGraph* m_packageGraph;
    };

    class Factory : public QQmlNetworkAccessManagerFactory{
    public:
        virtual QNetworkAccessManager *create(QObject *parent);
    };

public:
    explicit QmlEngineInterceptor(QObject* parent = Q_NULLPTR) : QNetworkAccessManager(parent){}

    static void interceptEngine(ViewEngine* engine, PackageGraph* packageGraph, Project* project);

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
private:
    static Project* m_project;
};

}// namespace

#endif // LVQMLENGINEINTERCEPTOR_H
