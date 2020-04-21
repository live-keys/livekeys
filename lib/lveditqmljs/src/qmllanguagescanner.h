#ifndef LVQMLLANGUAGESCANNER_H
#define LVQMLLANGUAGESCANNER_H

#include <QObject>
#include <QMutex>
#include <QLinkedList>

#include <functional>

#include "live/lockedfileiosession.h"
#include "live/qmllanguageinfo.h"
#include "live/visuallog.h"
#include "live/documentqmlinfo.h"

namespace lv{

class CodeQmlHandler;

class LV_EDITQMLJS_EXPORT QmlLanguageScanner : public QObject{

    Q_OBJECT

public:
    class DocumentTransport{
    public:
        CodeQmlHandler* codeHandler;
        DocumentQmlInfo::Ptr documentInfo;
    };

public:
    explicit QmlLanguageScanner(LockedFileIOSession::Ptr lio, const QStringList& importPaths, QObject *parent = nullptr);
    ~QmlLanguageScanner();

    void queueLibrary(const QmlLibraryInfo::Ptr& lib);

    void onLibraryUpdate(std::function<void(QmlLibraryInfo::Ptr, int)> listener);
    void onQueueFinished(std::function<void()> listener);
    void onMessage(std::function<void(int, const QString& message)> listener);

    bool isProcessing() const;
    void requestStop();

    void enableFork(bool enable);

    QmlLibraryInfo::Ptr libraryInfo(const QString& uri) const;

public slots:
    void processQueue();
    void scanDocument(const QString& path, const QString& content, CodeQmlHandler* handler);

signals:
    void documentScanReady(DocumentTransport* transport);

private:
    QList<QmlTypeInfo::Ptr> scanLibrary(const QmlLibraryInfo::Ptr& lib);
    QList<QmlTypeInfo::Ptr> scanTypeInfoFile(const QString& typeInfo, const QmlLibraryInfo::Ptr& lib);
    QList<QmlTypeInfo::Ptr> scanTypeInfoStream(
        const QString& typeInfoPath, const QByteArray& stream, const QmlLibraryInfo::Ptr& lib
    );
    QmlTypeInfo::Ptr scanObjectFile(
        QmlLibraryInfo::Ptr lib,
        const QString& filePath,
        const QString& componentName,
        const QString& fileData);
    void scanQmlDirForExports(const QmlDirParser& dir, const QmlLibraryInfo::Ptr& lib);
    void scanPathForExports(const QString& path, const QmlLibraryInfo::Ptr& lib);

    void insertNewLibrary(const QmlLibraryInfo::Ptr& lib);

    QMutex                           m_queueMutex;
    QLinkedList<QmlLibraryInfo::Ptr> m_queue;

    std::function<void(QmlLibraryInfo::Ptr, int)>    m_updateListener;
    std::function<void()>                            m_queueFinished;
    std::function<void(int, const QString& message)> m_messageListener;

    QHash<QString, QmlLibraryInfo::Ptr> m_libraries;
    QStringList                         m_defaultImportPaths;
    LockedFileIOSession::Ptr            m_ioSession;

    bool                                m_isProcessing;
    bool                                m_stopRequest;
    bool                                m_forkEnabled;
};

inline void QmlLanguageScanner::onLibraryUpdate(std::function<void (QmlLibraryInfo::Ptr, int)> listener){
    m_updateListener = listener;
}

inline void QmlLanguageScanner::onQueueFinished(std::function<void ()> listener){
    m_queueFinished = listener;
}

inline bool QmlLanguageScanner::isProcessing() const{
    return m_isProcessing;
}

inline void QmlLanguageScanner::requestStop(){
    m_stopRequest = true;
}

inline void QmlLanguageScanner::enableFork(bool enable){
    m_forkEnabled = enable;
}

inline QmlLibraryInfo::Ptr QmlLanguageScanner::libraryInfo(const QString &uri) const{
    auto findIt = m_libraries.find(uri);
    if ( findIt == m_libraries.end() )
        return nullptr;
    return findIt.value();
}

}// namespace

#endif // LVQMLLANGUAGESCANNER_H
