#ifndef LVQMLFOLLOWUP_H
#define LVQMLFOLLOWUP_H

#include "act.h"

namespace lv{

/// \private
class QmlFollowUp : public Act{

    Q_OBJECT
    Q_PROPERTY(QVariant prev READ prev NOTIFY prevChanged)

public:
    explicit QmlFollowUp(QObject *parent = 0);
    ~QmlFollowUp(){}

    QVariant prev() const;

public slots:
    void _whenPrevChanged();

signals:
    void prevChanged();

protected:
    void componentComplete() override;

private:
    Act*     m_prevAct;
    QVariant m_prev;
};

inline QVariant QmlFollowUp::prev() const{
    return m_prev;
}

} // namespace

#endif // LVQMLFOLLOWUP_H
