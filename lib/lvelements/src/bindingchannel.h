#ifndef LVBINDINGCHANNEL_H
#define LVBINDINGCHANNEL_H

#include "element.h"
#include "property.h"
#include "live/projectdocument.h"
#include "live/visuallog.h"
#include "live/runnable.h"
#include "bindingpath.h"

namespace lv{ namespace el {

class BindingSpan;

/// \private
class BindingChannel : public Element{

    Q_OBJECT

public:
    typedef std::shared_ptr<BindingChannel>       Ptr;
    typedef std::shared_ptr<const BindingChannel> ConstPtr;

public:
    static BindingChannel::Ptr create(BindingPath::Ptr bindingPath, Runnable* runnable);
    static BindingChannel::Ptr create(
        BindingPath::Ptr bindingPath,
        Runnable* runnable,
        Property* property,
        int listIndex = -1
    );

    explicit BindingChannel(BindingPath::Ptr bindingPath, Runnable* runnable, Engine *parent = nullptr);
    virtual ~BindingChannel();

    bool hasConnection() const;
    void updateConnection(Property* property, int listIndex = -1);
    void clearConnection();

    const BindingPath::ConstPtr& bindingPath() const{ return m_bindingPath; }
    Runnable* runnable();
    int listIndex() const{ return m_listIndex; }
    Property* property(){ return m_property; }
    bool isEnabled() const{ return m_enabled; }
    bool canModify() const;

    BindingPath::Ptr expressionPath();

    void setEnabled(bool enable);

    void commit(const QVariant& value); // TODO

public slots:
    // void __runableReady();

signals:
    void runnableObjectReady();

private:
    BindingPath::Ptr    m_bindingPath;
    Runnable*           m_runnable;
    Property*           m_property;
    int                 m_listIndex;
    bool                m_enabled;
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

inline void BindingChannel::updateConnection(Property* property, int listIndex){
    m_property = property;
    m_listIndex = listIndex;
}

inline void BindingChannel::clearConnection(){
    m_property  = nullptr;
    m_listIndex = -1;
}

}// namespace el
}// namespace lv

#endif // LVBINDINGCHANNEL_H
