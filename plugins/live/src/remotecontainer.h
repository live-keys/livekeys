#ifndef LVREMOTECONTAINER_H
#define LVREMOTECONTAINER_H

#include <QObject>
#include <QQmlParserStatus>

#include "live/linecapture.h"
#include "live/mlnode.h"

namespace lv{

class RemoteContainer : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit RemoteContainer(QObject* parent = nullptr);
    virtual ~RemoteContainer();

    virtual void sendError(const QByteArray& type, int code, const QString& message);
    virtual void sendBuild(const QByteArray& buildData);
    virtual void sendInput(const MLNode& input);
    virtual bool isReady() const;

    virtual void onMessage(std::function<void(const LineMessage&, void* data)>, void* handlerData = 0);
    virtual void onError(std::function<void(int, const std::string&)>);

    bool isComponentComplete() const;

signals:
    void ready();

protected:
    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

private:
    bool m_componentComplete;
};

inline bool RemoteContainer::isComponentComplete() const{
    return m_componentComplete;
}

}// namespace

#endif // LVREMOTECONTAINER_H
