#ifndef TCPLINEPROPERTY_H
#define TCPLINEPROPERTY_H

#include <QObject>

namespace lv{

class TcpLine;
class TcpLineProperty : public QObject{

    Q_OBJECT

public:
    explicit TcpLineProperty(const QString& name, TcpLine *parent = nullptr);
    ~TcpLineProperty(){}

    const QString& name();

public slots:
    void changed();

private:
    TcpLine* m_line;
    QString  m_name;

};

inline const QString &TcpLineProperty::name(){
    return m_name;
}

}// namespace

#endif // TCPLINEPROPERTY_H
