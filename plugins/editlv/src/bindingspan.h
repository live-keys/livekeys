#ifndef LVBINDINGSPAN_H
#define LVBINDINGSPAN_H

#include "live/elements/bindingpath.h"
#include "bindingchannel.h"

#include <QDebug>

namespace lv{

class LvEditFragment;

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
class BindingSpan{

public:
    BindingSpan(LvEditFragment* fragment);
    ~BindingSpan();

    void setExpressionPath(el::BindingPath::Ptr expressionPath);
    el::BindingPath::Ptr expressionPath();

    void commit(const QVariant& value);

    void addOutputChannel(BindingChannel::Ptr bc);
    const QList<BindingChannel::Ptr>& outputChannels() const;

    void setInputChannel(BindingChannel::Ptr bc);
    const BindingChannel::Ptr& inputChannel() const;

private:
    LvEditFragment*                 m_fragment;
    el::BindingPath::Ptr            m_expressionPath;
    QList<BindingChannel::Ptr>      m_outputChannels;
    BindingChannel::Ptr             m_inputChannel;
};


inline void BindingSpan::setExpressionPath(el::BindingPath::Ptr expressionPath){
    m_expressionPath = expressionPath;
}

inline el::BindingPath::Ptr BindingSpan::expressionPath(){
    return m_expressionPath;
}

inline const QList<BindingChannel::Ptr> &BindingSpan::outputChannels() const{
    return m_outputChannels;
}

inline const BindingChannel::Ptr &BindingSpan::inputChannel() const{
    return m_inputChannel;
}

}// namespace

#endif // LVQMLBINDINGSPAN_H
