#ifndef LVREMOTELINERESPONSE_H
#define LVREMOTELINERESPONSE_H

#include <QObject>
#include <functional>

namespace lv{

class RemoteLineResponse : public QObject{

    Q_OBJECT

public:
    explicit RemoteLineResponse(QObject *parent = nullptr);
    ~RemoteLineResponse(){}

    void onResponse(std::function<void(const QString&, const QVariant&)> callback);

public slots:
    void send(const QString& propertyName, const QVariant& value);

private:
    std::function<void(const QString&, const QVariant&)> m_responseCallback;
};

}// namespace

#endif // LVREMOTELINERESPONSE_H
