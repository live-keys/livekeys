#ifndef LVQMLLANGUAGESCANMONITOR_H
#define LVQMLLANGUAGESCANMONITOR_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "live/qmllanguageinfo.h"
#include "qmllanguagescanner.h"

namespace lv{

class QmlLanguageScanMonitor : public QObject{

    Q_OBJECT

public:
    explicit QmlLanguageScanMonitor(QmlLanguageScanner* scanner, QObject *parent = nullptr);
    ~QmlLanguageScanMonitor();

    void queueLibrary(const QmlLibraryInfo::Ptr& lib);
    QList<QmlLibraryInfo::Ptr> detachLibraries();

    void requestStop();
    bool isProcessing() const;

    QmlLanguageScanner* languageScanner() const;

signals:
    void libraryUpdates();

public slots:
    void processQueue();

private:
    QList<QmlLibraryInfo::Ptr> m_collectedLibraries;

    QMutex                     m_librariesMutex;
    QList<QmlLibraryInfo::Ptr> m_libraries;
    QmlLanguageScanner*        m_scanner;

};

inline QmlLanguageScanner *QmlLanguageScanMonitor::languageScanner() const{
    return m_scanner;
}

}// namespace

#endif // LVQMLLANGUAGESCANMONITOR_H
