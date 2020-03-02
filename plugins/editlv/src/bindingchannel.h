#ifndef LVBINDINGCHANNEL_H
#define LVBINDINGCHANNEL_H

#include "live/elements/element.h"
#include "live/elements/property.h"
#include "live/projectdocument.h"
#include "live/visuallog.h"
#include "live/runnable.h"
#include "live/elements/bindingpath.h"

namespace lv{

class BindingSpan;

/// \private
class BindingChannel : public QObject{

    Q_OBJECT

public:
    typedef std::shared_ptr<BindingChannel>       Ptr;
    typedef std::shared_ptr<const BindingChannel> ConstPtr;

public:
    static BindingChannel::Ptr create(el::BindingPath::Ptr bindingPath, Runnable* runnable);
    static BindingChannel::Ptr create(
        el::BindingPath::Ptr bindingPath,
        Runnable* runnable,
        el::Property* property,
        int listIndex = -1
    );

    explicit BindingChannel(el::BindingPath::Ptr bindingPath, Runnable* runnable, QObject *parent = nullptr);
    virtual ~BindingChannel();

    bool hasConnection() const;
    void updateConnection(el::Property* property, int listIndex = -1);
    void clearConnection();

    const el::BindingPath::ConstPtr& bindingPath() const{ return m_bindingPath; }
    Runnable* runnable();
    int listIndex() const{ return m_listIndex; }
    el::Property* property(){ return m_property; }
    bool isEnabled() const{ return m_enabled; }
    bool canModify() const;

    el::BindingPath::Ptr expressionPath();

    void setEnabled(bool enable);
    BindingChannel::Ptr traverseBindingPath();
public slots:
    void __runnableReady();

signals:
    void runnableObjectReady();

private:
    el::BindingPath::Ptr    m_bindingPath;
    Runnable*               m_runnable;
    el::Property*           m_property;
    int                     m_listIndex;
    bool                    m_enabled;
};

inline bool BindingChannel::hasConnection() const{
    return m_property != nullptr;
}

inline Runnable *BindingChannel::runnable(){
    return m_runnable;
}

inline bool BindingChannel::canModify() const{
    return m_enabled && hasConnection();
}

inline void BindingChannel::setEnabled(bool enable){
    m_enabled = enable;
}

inline void BindingChannel::updateConnection(el::Property* property, int listIndex){
    m_property = property;
    m_listIndex = listIndex;
}

inline void BindingChannel::clearConnection(){
    m_property  = nullptr;
    m_listIndex = -1;
}

}// namespace lv

#endif // LVBINDINGCHANNEL_H
