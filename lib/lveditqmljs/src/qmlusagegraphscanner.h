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

#ifndef LVQMLUSAGEGRAPHSCANNER_H
#define LVQMLUSAGEGRAPHSCANNER_H

#include <QThread>
#include "qmlscopesnap_p.h"

namespace lv{

class QmlUsageGraphScanner : public QThread{

    Q_OBJECT

public:
    // A binding entry contains a file component(name and path) and a
    // reference to where that component is being used
    class BindingEntry{
    public:
        QString          componentName;
        QString          componentPath;
        QmlBindingPath::Ptr path;

        QString toString() const{ return componentName + " from \'" + componentPath + "\' in ---> " + path->toString(); }
    };

public:
    explicit QmlUsageGraphScanner(ViewEngine* engine, Project* project, const QmlScopeSnap &qss, QObject *parent = nullptr);
    ~QmlUsageGraphScanner();

    void run() override;

    void setSearchComponent(const QString componentFile, const QString& component);

    bool hasResult() const;
    QPair<Runnable*, QmlBindingPath::Ptr> popResult();

    void requestStop();

signals:
    void bindingPathAdded();

public slots:

private:
    Q_DISABLE_COPY(QmlUsageGraphScanner);

    QList<QmlUsageGraphScanner::BindingEntry> extractRanges(
        DocumentQmlValueObjects::RangeObject* obj,
        QmlBindingPath::Ptr bp,
        const QString& source,
        const QString& file);

    void followGraph(
        const QString& component,
        const QString &componentPath,
        const QList<QmlUsageGraphScanner::BindingEntry> &gatheredComponents,
        const QMap<QString, QList<QmlBindingPath::Ptr> > &usageGraph);

    void pushGraphResult(QList<QmlUsageGraphScanner::BindingEntry> segments);
    bool checkEntry(const QmlUsageGraphScanner::BindingEntry& entry);

    bool                   m_stopRequest;
    ViewEngine*            m_viewEngine;
    Project*               m_project;
    QmlScopeSnap           m_scopeSnap;
    QMap<QString, QString> m_runnables;
    QString                m_searchComponent;
    QString                m_searchComponentFile;
    std::list<QmlBindingPath::Ptr>* m_scannedResults;
};

inline bool QmlUsageGraphScanner::hasResult() const{
    return !m_scannedResults->empty();
}

inline void QmlUsageGraphScanner::requestStop(){
    m_stopRequest = true;
}

}// namespace

#endif // LVQMLUSAGEGRAPHSCANNER_H
