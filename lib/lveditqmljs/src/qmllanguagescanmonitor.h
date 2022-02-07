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

    QmlLanguageScanner* languageScanner() const;

    bool isProcessing() const;

signals:
    void libraryUpdates();
    void scannerQueueCleared();

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
