#ifndef LVTCPLINERESPONSE_H
#define LVTCPLINERESPONSE_H

#include <QObject>

namespace lv{

class TcpLineSocket;

class TcpLineResponse : public QObject{

    Q_OBJECT

public:
    explicit TcpLineResponse(TcpLineSocket *parent = nullptr);
    ~TcpLineResponse(){}

public slots:
    void send(const QString& propertyName, const QVariant& value);

private:
    TcpLineSocket* m_socket;

};

}// namespace

#endif // LVTCPLINERESPONSE_H
