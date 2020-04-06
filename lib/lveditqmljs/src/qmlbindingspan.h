#ifndef LVQMLBINDINGSPAN_H
#define LVQMLBINDINGSPAN_H

#include "qmlbindingpath.h"
#include "qmlbindingchannel.h"

#include <QDebug>

namespace lv{

class QmlEditFragment;

/**
 * \brief The BindingSpan class
 *
 * A binding span can have multiple binding channels, describing an expression
 * within a docuemnt that requires a connection with a set of runtimes.
 *
 * Since components can be created in different documents, and in turn those documents
 * can be recreated, a single expression can correspond to multiple binding paths, and
 * also Runnables.
 *
 * \private
 */
class QmlBindingSpan{

public:
    QmlBindingSpan(QmlEditFragment* fragment);
    ~QmlBindingSpan();

    void setExpressionPath(QmlBindingPath::Ptr expressionPath);
    QmlBindingPath::Ptr expressionPath();

    void commit(const QVariant& value);

    void addChannel(QmlBindingChannel::Ptr bc);
    const QList<QmlBindingChannel::Ptr>& channels() const;

    void setConnectionChannel(QmlBindingChannel::Ptr bc);
    const QmlBindingChannel::Ptr& connectionChannel() const;

private:
    QmlEditFragment*              m_fragment;
    QmlBindingPath::Ptr           m_expressionPath;
    QList<QmlBindingChannel::Ptr> m_outputChannels;
    QmlBindingChannel::Ptr        m_connectionChannel;
};


inline void QmlBindingSpan::setExpressionPath(QmlBindingPath::Ptr expressionPath){
    m_expressionPath = expressionPath;
}

inline QmlBindingPath::Ptr QmlBindingSpan::expressionPath(){
    return m_expressionPath;
}

inline const QList<QmlBindingChannel::Ptr> &QmlBindingSpan::channels() const{
    return m_outputChannels;
}

inline const QmlBindingChannel::Ptr &QmlBindingSpan::connectionChannel() const{
    return m_connectionChannel;
}

}// namespace

#endif // LVQMLBINDINGSPAN_H
