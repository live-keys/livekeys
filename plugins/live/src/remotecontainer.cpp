#include "remotecontainer.h"

namespace lv{

RemoteContainer::RemoteContainer(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
{

}

RemoteContainer::~RemoteContainer(){
}

void RemoteContainer::componentComplete(){
    m_componentComplete = true;
}

void RemoteContainer::sendError(const QByteArray &, int, const QString &){}
void RemoteContainer::sendBuild(const QByteArray &){}
void RemoteContainer::sendInput(const MLNode&){}

bool RemoteContainer::isReady() const{ return false; }

void RemoteContainer::onMessage(std::function<void (const LineMessage &, void*)>, void*){}
void RemoteContainer::onError(std::function<void (int, const std::string &)>){}

}// namespace
