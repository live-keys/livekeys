#ifndef LVQMLFOLLOWUP_H
#define LVQMLFOLLOWUP_H

#include "qmlact.h"

namespace lv{

/// \private
class QmlFollowUp : public QmlAct{

    Q_OBJECT
    Q_PROPERTY(QJSValue prev READ prev NOTIFY prevChanged)

public:
    explicit QmlFollowUp(QObject *parent = nullptr);
    ~QmlFollowUp() override{}

    QJSValue prev() const;

public slots:
    void _whenPrevChanged();

signals:
    void prevChanged();

protected:
    void componentComplete() override;

private:
    QmlAct*  m_prevAct;
    QJSValue m_prev;
};

inline QJSValue QmlFollowUp::prev() const{
    return m_prev;
}

} // namespace

#endif // LVQMLFOLLOWUP_H
