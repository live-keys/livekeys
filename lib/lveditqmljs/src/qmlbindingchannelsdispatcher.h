#ifndef LVQMLBINDINGCHANNELSDISPATCHER_H
#define LVQMLBINDINGCHANNELSDISPATCHER_H

#include <QObject>
#include "live/project.h"

namespace lv{

class CodeQmlHandler;
class DocumentQmlChannels;

/// \private
class QmlBindingChannelsDispatcher : public QObject{

    Q_OBJECT

public:
    explicit QmlBindingChannelsDispatcher(Project* project, QObject *parent = nullptr);
    ~QmlBindingChannelsDispatcher() override;

    void initialize(CodeQmlHandler* codeHandler, DocumentQmlChannels* documentChannels);
    void removeDocumentChannels(DocumentQmlChannels* documentChannels);

signals:

public slots:
    void __newQmlBuild(Runnable* runnable, QmlBuild* build);
    void __qmlBuildReady();


private:
    Project*                   m_project;
    QSet<DocumentQmlChannels*> m_channeledDocuments;

};

}// namespace

#endif // LVQMLBINDINGCHANNELSDISPATCHER_H
