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

    void addOutputChannel(QmlBindingChannel::Ptr bc);
    const QList<QmlBindingChannel::Ptr>& outputChannels() const;

    void setInputChannel(QmlBindingChannel::Ptr bc);
    const QmlBindingChannel::Ptr& inputChannel() const;

private:
    QmlEditFragment*           m_fragment;
    QmlBindingPath::Ptr           m_expressionPath;
    QList<QmlBindingChannel::Ptr> m_outputChannels;
    QmlBindingChannel::Ptr        m_inputChannel;
};


inline void QmlBindingSpan::setExpressionPath(QmlBindingPath::Ptr expressionPath){
    m_expressionPath = expressionPath;
}

inline QmlBindingPath::Ptr QmlBindingSpan::expressionPath(){
    return m_expressionPath;
}

inline const QList<QmlBindingChannel::Ptr> &QmlBindingSpan::outputChannels() const{
    return m_outputChannels;
}

inline const QmlBindingChannel::Ptr &QmlBindingSpan::inputChannel() const{
    return m_inputChannel;
}

}// namespace

#endif // LVQMLBINDINGSPAN_H
