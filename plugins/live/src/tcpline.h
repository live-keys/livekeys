#ifndef LVTCPLINE_H
#define LVTCPLINE_H

#include <QObject>
#include <QQmlPropertyMap>
#include "componentsource.h"
#include "tcplineconnection.h"

namespace lv{

class TcpLineProperty;

class TcpLine : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::ComponentSource* source       READ source     WRITE setSource     NOTIFY sourceChanged)
    Q_PROPERTY(lv::TcpLineConnection* connection READ connection WRITE setConnection NOTIFY connectionChanged)
    Q_PROPERTY(QQmlPropertyMap* result           READ result     NOTIFY resultChanged)
    Q_CLASSINFO("DefaultProperty", "source")

    friend class TcpLineProperty;

public:
    explicit TcpLine(QObject *parent = nullptr);
    ~TcpLine();

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

    bool isComponentComplete() const;

    lv::ComponentSource* source() const;
    void setSource(lv::ComponentSource* source);

    lv::TcpLineConnection* connection() const;
    void setConnection(lv::TcpLineConnection* connection);

    static void receiveMessage(const LineMessage& message, void* data);
    void onMessage(const LineMessage& message);

    QQmlPropertyMap* result() const;

public slots:
    void initialize();

signals:
    void complete();
    void sourceChanged();
    void connectionChanged();
    void resultChanged();

private:
    void propertyChanged(TcpLineProperty* property);
    void sendProperty(const QString& propertyName);

    QList<TcpLineProperty*> m_properties;
    bool                    m_componentComplete;
    bool                    m_componentBuild;
    lv::ComponentSource*    m_source;
    lv::TcpLineConnection*  m_connection;
    QQmlPropertyMap*        m_result;

    QSet<QString>           m_propertiesToSend;
};

inline bool TcpLine::isComponentComplete() const{
    return m_componentComplete;
}

inline ComponentSource *TcpLine::source() const{
    return m_source;
}

inline TcpLineConnection *TcpLine::connection() const{
    return m_connection;
}

inline QQmlPropertyMap *TcpLine::result() const{
    return m_result;
}

}// namespace

#endif // LVTCPLINE_H
