#ifndef LVBINDINGCHANNEL_H
#define LVBINDINGCHANNEL_H

#include <QObject>
#include <QQmlProperty>

#include "live/projectdocument.h"
#include "live/visuallog.h"
#include "bindingpath.h"

namespace lv{

class QmlEditFragment;

/**
 * \brief The BindingChannel class
 *
 * A binding channel can have multiple binding paths, describing an expression
 * within a docuemnt that requires a connection with the application.
 *
 * Since components can be created in different documents, and in turn those documents
 * can be recreated, a single expression can correspond to multiple binding paths, and
 * also documents.
 *
 * \private
 */
class BindingChannel : public QObject{

    Q_OBJECT

public:
    explicit BindingChannel(QmlEditFragment* fragment, QObject *parent = nullptr);
    virtual ~BindingChannel();

    void setExpressionPath(BindingPath* expressionPath);
    BindingPath* expressionPath();

    void commit(const QVariant& value);

public slots:
    void commitFromFragment();

private:
    QmlEditFragment*    m_fragment;
    BindingPath*        m_expressionPath;
    QList<BindingPath*> m_paths;
};


inline void BindingChannel::setExpressionPath(BindingPath *expressionPath){
    m_expressionPath = expressionPath;
}

inline BindingPath *BindingChannel::expressionPath(){
    return m_expressionPath;
}

inline void BindingChannel::commit(const QVariant &value){
    if ( m_expressionPath && m_expressionPath->hasConnection() ){
        if ( m_expressionPath->m_listIndex == -1 ){
            m_expressionPath->m_property.write(value);
        }
    }
}

}// namespace

#endif // LVBINDINGCHANNEL_H
