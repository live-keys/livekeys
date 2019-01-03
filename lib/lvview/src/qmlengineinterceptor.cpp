#include "qmlengineinterceptor.h"
#include <QNetworkRequest>
#include <QQmlEngine>

namespace lv{

QmlEngineInterceptor::UrlInterceptor::UrlInterceptor(ViewEngine* engine, PackageGraph* packageGraph)
    : m_engine(engine)
    , m_packageGraph(packageGraph)
{
}

QUrl QmlEngineInterceptor::UrlInterceptor::intercept(const QUrl &path, QQmlAbstractUrlInterceptor::DataType dataType){
    QStringList importPaths = m_engine->engine()->importPathList();

    if ( dataType == QQmlAbstractUrlInterceptor::QmldirFile && path.isLocalFile() ){
        QString localPath = path.toLocalFile();
        if ( localPath.endsWith("qmldir") ){
            localPath = localPath.mid(0, localPath.length() - 6);
        }

        QString importSegment;

        for ( auto it = importPaths.begin(); it != importPaths.end(); ++it ){
            if ( localPath.startsWith(*it) ){
                importSegment = localPath.mid(it->length());
                if ( importSegment.startsWith('/') )
                    importSegment = importSegment.mid(1);
                if ( importSegment.endsWith('/') )
                    importSegment = importSegment.mid(0, importSegment.length() - 1);
                break;
            }
        }

        if ( !importSegment.isEmpty() ){
            QStringList importSegmentParts = importSegment.split('/');
            std::vector<std::string> partsConverted;
            for ( auto it = importSegmentParts.begin(); it != importSegmentParts.end(); ++it ){
                int dotIndex = it->indexOf('.');
                if ( dotIndex != -1 ){
                    partsConverted.push_back(it->mid(0, dotIndex).toStdString());
                } else {
                    partsConverted.push_back(it->toStdString());
                }
            }

            Plugin::Ptr plugin = m_packageGraph->loadPlugin(partsConverted);
            if ( plugin != nullptr ){
                return QUrl::fromLocalFile(QString::fromStdString(plugin->path() + "/qmldir"));
            }
        }
    }
    return path;
}

QNetworkAccessManager *QmlEngineInterceptor::Factory::create(QObject *parent){
    return new QmlEngineInterceptor(parent);
}

QNetworkReply *QmlEngineInterceptor::createRequest(
        QNetworkAccessManager::Operation op,
        const QNetworkRequest &request,
        QIODevice *outgoingData)
{
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}


void QmlEngineInterceptor::interceptEngine(ViewEngine *engine, PackageGraph* packageGraph){
//    engine->setNetworkAccessManagerFactory(new QmlEngineInterceptor::Factory);
    engine->engine()->setUrlInterceptor(new QmlEngineInterceptor::UrlInterceptor(engine, packageGraph));
}

}// namespace
