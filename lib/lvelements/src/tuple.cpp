#include "tuple.h"

namespace lv{ namespace el{

Tuple::Tuple(Engine *engine)
    : Element(engine)
{
}

Tuple::~Tuple(){

}

InstanceProperty *Tuple::addProperty(
        const std::string &name,
        const std::string &type,
        ScopedValue value,
        bool isDefault,
        bool isWritable,
        const std::string &notifyEvent)
{
    InstanceProperty* ip = Element::addProperty(name, type, value, isDefault, isWritable, notifyEvent);
    if ( !notifyEvent.empty() ){
        try {
            on(notifyEvent, [this, name](const Function::Parameters&){
                propertyChanged(name);
            });
        } catch (lv::Exception exc){
            engine()->throwError(&exc, nullptr);
            return nullptr;
        }
    }
    return ip;
}

}} // namespace
