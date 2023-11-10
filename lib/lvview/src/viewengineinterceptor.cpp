#include "viewengineinterceptor.h"
#include "viewengineinterceptor_p.h"
#include "live/visuallogqt.h"

namespace lv{

ViewEngineInterceptor::ViewEngineInterceptor(){
}

ViewEngineInterceptor::~ViewEngineInterceptor(){
}

QmlEngineInterceptor::UrlInterceptor::UrlInterceptor(ViewEngine* engine)
    : m_engine(engine)
    , m_interceptor(nullptr)
{
}

void QmlEngineInterceptor::UrlInterceptor::setInterceptor(ViewEngineInterceptor *interceptor){
    m_interceptor = interceptor;
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
                    if ( Module::fileExistsIn(localPath.toStdString() ) ){
                        Module::Ptr module = m_engine->packageGraph()->loadModule(partsConverted);
                        if ( module != nullptr ){
                            return QUrl::fromLocalFile(QString::fromStdString(module->path() + "/qmldir"));
                        }
                    }
                } catch ( lv::Exception& ){}
            }
        } catch ( lv::Exception& e ){
            m_engine->throwError(&e, nullptr);
        }
    }
    return m_interceptor ? m_interceptor->interceptUrl(path, dataType) : path;
}

QmlEngineInterceptor::Factory::Factory(ViewEngineInterceptor *interceptor)
    : m_interceptor(interceptor)
{
}

QNetworkAccessManager *QmlEngineInterceptor::Factory::create(QObject *parent){
    return new QmlEngineInterceptor(m_interceptor, parent);
}

QNetworkReply *QmlEngineInterceptor::createRequest(
        QNetworkAccessManager::Operation op,
        const QNetworkRequest &request,
        QIODevice *outgoingData)
{
    ViewEngineInterceptor::ContentResult cr = m_interceptor->interceptContent(request.url());
    if ( cr.isValid() ){
        MemoryNetworkReply* mnr = new MemoryNetworkReply();
        mnr->setContent(cr.content());
        return mnr;
    }
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

void MemoryNetworkReply::setContent(const QString &content){
    m_content = content.toUtf8();
    m_offset = 0;
    open(ReadOnly | Unbuffered);
    setFinished(true);
}

void MemoryNetworkReply::abort(){}

qint64 MemoryNetworkReply::bytesAvailable() const{
    return m_content.size() - m_offset + QIODevice::bytesAvailable();
}

bool MemoryNetworkReply::isSequential() const{
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
