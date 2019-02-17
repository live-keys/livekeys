#ifndef LVCALL_H
#define LVCALL_H

#include <QObject>
#include <QVariant>
#include <QQmlParserStatus>

namespace lv{

/// \private
class Call : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QVariant result READ result WRITE setResult NOTIFY resultChanged)

public:
    Call(QObject* parent = nullptr);
    virtual ~Call();

    void classBegin(){}
    void componentComplete();

    bool isComponentComplete();

    const QVariant& result() const;

public slots:
    void setResult(const QVariant& result);

signals:
    void trigger();
    void complete();

    void resultChanged();

private:
    bool     m_isComponentComplete;
    QVariant m_result;
};

inline bool Call::isComponentComplete(){
    return m_isComponentComplete;
}

inline const QVariant &Call::result() const{
    return m_result;
}

inline void Call::setResult(const QVariant &result){
    if (m_result == result)
        return;

    m_result = result;
    emit resultChanged();
}

} // namespace

#endif // LVCALL_H
