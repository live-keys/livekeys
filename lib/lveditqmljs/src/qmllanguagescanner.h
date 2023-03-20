/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVQMLLANGUAGESCANNER_H
#define LVQMLLANGUAGESCANNER_H

#include <QObject>
#include <QMutex>

#include <functional>
#include <list>

#include "live/lockedfileiosession.h"
#include "live/qmllanguageinfo.h"
#include "live/visuallog.h"
#include "live/documentqmlinfo.h"

namespace lv{

class LanguageQmlHandler;

class LV_EDITQMLJS_EXPORT QmlLanguageScanner : public QObject{

    Q_OBJECT

public:
    class DocumentTransport{
    public:
        LanguageQmlHandler* codeHandler;
        DocumentQmlInfo::Ptr documentInfo;
    };

public:
    explicit QmlLanguageScanner(FileIOInterface::Ptr lio, const QStringList& importPaths, QObject *parent = nullptr);
    ~QmlLanguageScanner();

    void queueLibrary(const QmlLibraryInfo::Ptr& lib);

    void onLibraryUpdate(std::function<void(QmlLibraryInfo::Ptr, int)> listener);
    void onQueueFinished(std::function<void()> listener);
    void onMessage(std::function<void(int, const QString& message)> listener);

    bool isProcessing() const;
    void requestStop();

    void enableFork(bool enable);

    QmlLibraryInfo::Ptr libraryInfo(const QString& uri) const;

    int plannedQueueSize();

public slots:
    void processQueue();
    void scanDocument(const QString& path, const QString& content, LanguageQmlHandler* handler);

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
        const QString& fileData,
        bool isSingleton = false);
    void scanQmlDirForExports(const QmlDirParser& dir, const QmlLibraryInfo::Ptr& lib);
    void scanPathForExports(const QString& path, const QmlLibraryInfo::Ptr& lib);

    void updateLibraryUnknownTypes(const QmlLibraryInfo::Ptr& lib);

    void insertNewLibrary(const QmlLibraryInfo::Ptr& lib);

    QMutex                           m_plannedQueueMutex;
    std::list<QmlLibraryInfo::Ptr>*  m_plannedQueue;

    std::function<void(QmlLibraryInfo::Ptr, int)> m_updateListener;
    std::function<void()>                         m_queueFinished;
    std::function<void(int, const QString&)>      m_messageListener;

    QHash<QString, QmlLibraryInfo::Ptr> m_libraries;
    QStringList                         m_defaultImportPaths;
    FileIOInterface::Ptr                m_ioSession;

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
