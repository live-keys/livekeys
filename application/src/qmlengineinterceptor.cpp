#include "qmlengineinterceptor.h"
#include <QNetworkRequest>
#include <QQmlEngine>
#include <QTimer>

namespace lv{

Project* QmlEngineInterceptor::m_project = nullptr;

QmlEngineInterceptor::UrlInterceptor::UrlInterceptor(ViewEngine* engine, PackageGraph* packageGraph, Project* project)
    : m_project(project)
    , m_engine(engine)
    , m_packageGraph(packageGraph)
{
}

QUrl QmlEngineInterceptor::UrlInterceptor::intercept(const QUrl &path, QQmlAbstractUrlInterceptor::DataType dataType){
    QStringList importPaths = m_engine->engine()->importPathList();

    if ( dataType == QQmlAbstractUrlInterceptor::QmldirFile && path.isLocalFile() ){
        try{
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

                try{
                    Plugin::Ptr plugin = m_packageGraph->loadPlugin(partsConverted);
                    if ( plugin != nullptr ){
                        return QUrl::fromLocalFile(QString::fromStdString(plugin->path() + "/qmldir"));
                    }
                } catch ( lv::Exception& ){}
            }
        } catch ( lv::Exception& e ){
            m_engine->throwError(&e, nullptr);
        }
    }
    else if (dataType == QQmlAbstractUrlInterceptor::QmlFile && path.isLocalFile()){
        if (m_project->isOpened(path.toLocalFile())){
            QUrl memoryPath(path);
            memoryPath.setScheme("memory");
            return memoryPath;
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
    if (request.url().scheme() == "memory")
    {
        MemoryNetworkReply* mnr = new MemoryNetworkReply();
        QUrl url = request.url();
        url.setScheme("file");
        QString content = m_project->openTextFile(url.toLocalFile())->content();
        mnr->setContent(content);
        return mnr;
    }
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

void QmlEngineInterceptor::interceptEngine(ViewEngine *engine, PackageGraph* packageGraph, Project* project){
    m_project = project;
    engine->engine()->setNetworkAccessManagerFactory(new QmlEngineInterceptor::Factory);
    engine->engine()->setUrlInterceptor(new QmlEngineInterceptor::UrlInterceptor(engine, packageGraph, project));
}

void MemoryNetworkReply::setContent(const QString &content)
{
    m_content = content.toUtf8();
    m_offset = 0;
    open(ReadOnly | Unbuffered);
    setFinished(true);
}

void MemoryNetworkReply::abort(){}

qint64 MemoryNetworkReply::bytesAvailable() const
{
    return m_content.size() - m_offset + QIODevice::bytesAvailable();
}

bool MemoryNetworkReply::isSequential() const
{
    return true;
}

qint64 MemoryNetworkReply::readData(char *data, qint64 maxSize)
{
    if (m_offset >= m_content.size())
        return -1;

    qint64 number = qMin(maxSize, m_content.size() - m_offset);
    memcpy(data, m_content.constData() + m_offset, static_cast<size_t>(number));
    m_offset += number;

    return number;
}



}// namespace
