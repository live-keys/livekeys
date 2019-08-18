#ifndef TCPLINEPROPERTY_H
#define TCPLINEPROPERTY_H

#include <QObject>

namespace lv{

class RemoteLine;
class RemoteLineProperty : public QObject{

    Q_OBJECT

public:
    explicit RemoteLineProperty(const QString& name, RemoteLine *parent = nullptr);
    ~RemoteLineProperty(){}

    const QString& name();

public slots:
    void changed();

private:
    RemoteLine* m_line;
    QString  m_name;

};

inline const QString &RemoteLineProperty::name(){
    return m_name;
}

}// namespace

#endif // TCPLINEPROPERTY_H
