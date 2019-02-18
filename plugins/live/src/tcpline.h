#ifndef NETWORKLINE_H
#define NETWORKLINE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlListProperty>
#include <QQmlComponent>

#include "tuple.h"
#include "componentsource.h"
#include "live/act.h"
#include "live/mlnode.h"

class QTcpServer;
class QTcpSocket;

namespace lv{

class Act;
class Project;
class TcpLineSocket;

/// \private
class TcpLine : public lv::Act{

    Q_OBJECT
    Q_PROPERTY(lv::Tuple* input             READ input   WRITE setInput   NOTIFY inputChanged)
    Q_PROPERTY(lv::Tuple* output            READ output  WRITE setOutput  NOTIFY outputChanged)
    Q_PROPERTY(lv::ComponentSource* source  READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(QString address              READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port                     READ port    WRITE setPort    NOTIFY portChanged)
    Q_CLASSINFO("DefaultProperty", "source")

public:
    explicit TcpLine(QObject *parent = nullptr);
    ~TcpLine();

    lv::Tuple* input();
    lv::Tuple* output();
    lv::ComponentSource* source();
    const QString& address() const;
    int port() const;

    void setSource(lv::ComponentSource *source);
    void setInput(lv::Tuple* input);
    void setOutput(lv::Tuple* output);
    void setAddress(const QString& address);
    void setPort(int port);

    virtual void classBegin(){}
    virtual void componentComplete();

    virtual void process();

public slots:
    void newConnection();

signals:
    void sourceChanged();
    void inputChanged();
    void outputChanged();
    void addressChanged();
    void portChanged();
    void listening();

private:
    void startListening();
    void removeSocket(TcpLineSocket *socket);
    void socketOutputReady(TcpLineSocket *socket);
    void initializeAgent();
    bool isAgentInitialized();
    TcpLineSocket* agent();

    lv::ComponentSource*  m_source;
    lv::Tuple*            m_input;
    lv::Tuple*            m_output;
    QByteArray            m_receivedBytes;
    MLNode                m_receivedOutput;
    lv::Project*          m_project;
    QTcpServer*           m_server;
    QList<TcpLineSocket*> m_sockets;

    QString            m_address;
    int                m_port;

    bool               m_componentComplete;

    MLNode             m_initializer;
};

inline Tuple *TcpLine::input(){
    return m_input;
}

inline Tuple *TcpLine::output(){
    return m_output;
}

inline ComponentSource *TcpLine::source(){
    return m_source;
}

inline const QString &TcpLine::address() const{
    return m_address;
}

inline int TcpLine::port() const{
    return m_port;
}

inline void TcpLine::setAddress(const QString &address){
    if (m_address == address)
        return;

    m_address = address;
    emit addressChanged();

    startListening();
}

inline void TcpLine::setPort(int port){
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged();

    startListening();
}

}// namespace

#endif // NETWORKLINE_H
