/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "element.h"
#include "element_p.h"
#include "context_p.h"
#include "live/exception.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

Element::Element(el::Engine* engine)
    : m_refs(0)
    , m_d(new ElementPrivate)
{
    m_d->engine = engine;
    m_d->parent = nullptr;
}

Element::~Element(){
    removeBoundPropertyExpressions();
    removeListeners();
    removeEmitters();
    removeInstanceProperties();
    if ( m_d->parent )
        m_d->parent->m_d->children.remove(this);
    for ( auto it = m_d->children.begin(); it != m_d->children.end(); ++it ){
        (*it)->m_d->parent = nullptr;
        (*it)->unref();
    }
    if ( !m_d->persistent.IsEmpty() )
        m_d->persistent.ClearWeak();
    delete m_d;
}

Engine *Element::engine() const{
    return m_d->engine;
}

void Element::setParent(Element *parent){
    ElementPrivate* d = m_d;
    if ( d->parent == nullptr ){
        if ( parent != nullptr ){
            ref();
            parent->m_d->children.push_back(this);
        }
        d->parent = parent;
    } else {
        d->parent->m_d->children.remove(this);
        if ( parent == nullptr ){
            if ( --m_refs == 0 ){
                if ( !m_d->persistent.IsEmpty() )
                    m_d->persistent.SetWeak(this, &ElementPrivate::weakElementDestructor, v8::WeakCallbackType::kParameter);
            }
        } else {
            parent->m_d->children.push_back(this);
        }
        d->parent = parent;
    }

}

Element *Element::parent(){
    return m_d->parent;
}

size_t Element::totalChildren() const{
    return m_d->children.size();
}

void Element::setLifeTimeWithObject(const v8::Local<v8::Object> &obj){
    ElementPrivate* d = m_d;
    obj->SetInternalField(0, v8::External::New(m_d->engine->isolate(), this));
    m_d->persistent.Reset(v8::Isolate::GetCurrent(), obj);
    if ( m_refs == 0 )
        m_d->persistent.SetWeak(this, &ElementPrivate::weakElementDestructor, v8::WeakCallbackType::kParameter);

    v8::Local<v8::Context> context = engine()->currentContext()->asLocal();

    for ( auto it = d->instanceProperties.begin(); it != d->instanceProperties.end(); ++it ){
        Property* p = it->second;

        v8::Local<v8::External> pdata = v8::External::New(engine()->isolate(), p);

        if ( p->isWritable() ){
            obj->SetAccessor(
                context,
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(engine()->isolate(), p->name().c_str()).ToLocalChecked()),
                &Property::ptrGetImplementation,
                &Property::ptrSetImplementation,
                pdata
            ).IsNothing();
        } else {
            obj->SetAccessor(
                context,
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(engine()->isolate(), p->name().c_str()).ToLocalChecked()),
                &Property::ptrGetImplementation,
                0,
                pdata
            ).IsNothing();;
        }
    }

    //TODO: Add instance events
}


// Reference Parts
// ---------------

// Notes on persistance

// While an object holds a persistent, the object should make sure that 'SetWeak'
// is set to false(using ClearWeak()). When the object free's the persistent, it should 'SetWeak' on
// it, and let the engine collect the object. Deleting the object manually will
// free the persistent.

// A persistent can be !NearDeath() when it's being freed, but that should be the
// responsibility of the user to make sure such things happen.

void Element::ref(){
    if ( m_refs == 0 && !m_d->persistent.IsEmpty() ){
        m_d->persistent.ClearWeak();
    }
    ++m_refs;
}

void Element::unref(){
    if ( --m_refs == 0 ){
        if ( !m_d->persistent.IsEmpty() ){
            m_d->persistent.SetWeak(this, &ElementPrivate::weakElementDestructor, v8::WeakCallbackType::kParameter);
        } else {
            Element::destroy(this);
        }
    }
}

bool Element::hasScriptReference() const{
    if ( m_d->persistent.IsEmpty() )
        return false;

    return true;
    // return !m_d->persistent.IsNearDeath();
}

void Element::destroy(Element *e){
    delete e;
}

EventConnection* Element::on(const std::string &key, std::function<void (const Function::Parameters &)> f){
    EventFunction* fnc = typeMetaObject().getEvent(key);
    if ( fnc ){
        EventListenerContainerBase* container = nullptr;
        auto delegateIt = m_listeningConnections.find(fnc->eventId());
        if ( delegateIt == m_listeningConnections.end() ){
            container = fnc->generateListenerContainer();
            m_listeningConnections[fnc->eventId()] = container;
        } else {
            container = delegateIt->second;
        }

        return container->append(this, fnc->eventId(), f);
    } else { // search locally
        auto evIt = m_d->instanceEvents.find(key);
        if ( evIt == m_d->instanceEvents.end() )
            THROW_EXCEPTION(lv::Exception, "Event under given key can't be found", lv::Exception::toCode("~Element"));

        InstanceEvent* ie = evIt->second;

        EventListenerContainerBase* container = nullptr;
        auto delegateIt = m_listeningConnections.find(ie->m_eventId);
        if ( delegateIt == m_listeningConnections.end() ){
            container = new EventListenerContainer<const Function::Parameters&>;
            m_listeningConnections[ie->m_eventId] = container;
        } else {
            container = delegateIt->second;
        }

        return container->append(this, ie->m_eventId, f);
    }
}

EventConnection *Element::on(const std::string &key, Callable callback){
    EventFunction* fnc = typeMetaObject().getEvent(key);
    if ( fnc ){
        EventListenerContainerBase* container = nullptr;
        auto delegateIt = m_listeningConnections.find(fnc->eventId());
        if ( delegateIt == m_listeningConnections.end() ){
            container = fnc->generateListenerContainer();
            m_listeningConnections[fnc->eventId()] = container;
        } else {
            container = delegateIt->second;
        }

        return container->append(this, fnc->eventId(), callback);
    } else { // search locally
        auto evIt = m_d->instanceEvents.find(key);
        if ( evIt == m_d->instanceEvents.end() ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Object does not have event: " + key, 1);
            engine()->throwError(&e, this);
            return nullptr;
        }

        InstanceEvent* ie = evIt->second;

        EventListenerContainerBase* container = nullptr;
        auto delegateIt = m_listeningConnections.find(ie->m_eventId);
        if ( delegateIt == m_listeningConnections.end() ){
            container = new EventListenerContainer<const Function::Parameters&>;
            m_listeningConnections[ie->m_eventId] = container;
        } else {
            container = delegateIt->second;
        }

        return container->append(this, ie->m_eventId, callback);
    }
}

void Element::on(ScopedValue eventKey, Callable callback){
    on(eventKey.toStdString(engine()), callback);
}

void Element::trigger(const std::string &eventKey, const Function::Parameters &params){
    EventFunction* fnc = typeMetaObject().getEvent(eventKey);
    Event::Id eventId = 0;

    if ( fnc ){
        eventId = fnc->eventId();
    } else {
        auto evIt = m_d->instanceEvents.find(eventKey);
        if ( evIt == m_d->instanceEvents.end() )
            THROW_EXCEPTION(lv::Exception, "Can't trigger event with given key", lv::Exception::toCode("~Element"));

        InstanceEvent* ie = evIt->second;
        eventId = ie->m_eventId;
    }

    EventListenerContainerBase* container = nullptr;
    auto delegateIt = m_listeningConnections.find(eventId);
    if ( delegateIt != m_listeningConnections.end() ){
        container = delegateIt->second;
        container->trigger(params);
    }
}

void Element::addEvent(Element *e, const std::string &ekey, const std::vector<std::string> &types){
    ElementPrivate* d = e->m_d;

    if ( d->instanceEvents.find(ekey) != d->instanceEvents.end() )
        THROW_EXCEPTION(lv::Exception, "Event with given key already exists.", lv::Exception::toCode("~Element"));

    InstanceEvent* ie = new InstanceEvent(
        e, EventIdGenerator::concatenate(0, 0, d->instanceEvents.size()), static_cast<int>(types.size())
    );
    d->instanceEvents[ekey] = ie;

    if ( !d->persistent.IsEmpty() ){
        v8::Local<v8::Object> v = d->persistent.Get(e->engine()->isolate());
        v8::Local<v8::External> fdata = v8::External::New(d->engine->isolate(), ie);

        v->Set(
            d->engine->isolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(d->engine->isolate(), ekey.c_str()).ToLocalChecked(),
            v8::FunctionTemplate::New(d->engine->isolate(), &InstanceEvent::callbackImplementation, fdata)->GetFunction(d->engine->isolate()->GetCurrentContext()).ToLocalChecked()
        ).IsNothing();
    }
}

void Element::addEvent(Element *e, ScopedValue eventKey, ScopedValue types){
    ElementPrivate* d = e->m_d;
    std::string ekey = eventKey.toStdString(e->engine());

    auto isolate = e->engine()->isolate();
    auto context = isolate->GetCurrentContext();

    if ( d->instanceEvents.find(ekey) != d->instanceEvents.end() )
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "An event with the same name already exists.", lv::Exception::toCode("~Element"));
        e->engine()->throwError(&exc, e);
        return;
    }

    v8::Local<v8::Array> typesArray = v8::Local<v8::Array>::Cast(types.data());

    InstanceEvent* ie = new InstanceEvent(
        e, EventIdGenerator::concatenate(0, 0, d->instanceEvents.size()), typesArray->Length()
    );
    d->instanceEvents[ekey] = ie;

    if ( !d->persistent.IsEmpty() ){
        v8::Local<v8::Object> v = d->persistent.Get(e->engine()->isolate());
        v8::Local<v8::External> fdata = v8::External::New(d->engine->isolate(), ie);

        v->Set(
            context,
            v8::String::NewFromUtf8(isolate, ekey.c_str()).ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, &InstanceEvent::callbackImplementation, fdata)->GetFunction(context).ToLocalChecked()
        ).IsNothing();
    }
}


/**
 * @brief Element::addProperty
 * @param e
 * @param propertyName
 * @param propertyOptions
 *
 * Options:
 *
 * type: "int"
 * value: 200 / (){ window.width + window.height }
 * isDefault: true/false
 * isWritable: true/false
 * bindings: [
 *      [window, "widthChanged"],
 *      [window, "heightChanged"]
 * ]
 * notify: "propertyChanged"
 *
 * @return
 */
void Element::addProperty(Element *e, ScopedValue propertyName, ScopedValue propertyOptions){
    std::string name = propertyName.toStdString(e->engine());
    std::string type = "variant";
    ScopedValue value(e->engine());
    bool isDefault   = false;
    bool isWritable  = true;
    std::string notifyEvent;

    auto isolate = e->engine()->isolate();
    auto context = isolate->GetCurrentContext();

    std::list<std::pair<Element*, std::string> > bindings;

    v8::Local<v8::Object> options = propertyOptions.data()->ToObject(context).ToLocalChecked();

    v8::Local<v8::String> typeKey       = v8::String::NewFromUtf8(e->engine()->isolate(), "type").ToLocalChecked();
    if ( options->Has(context, typeKey).ToChecked() ){
        type = *v8::String::Utf8Value(isolate, options->Get(context, typeKey).ToLocalChecked()->ToString(context).ToLocalChecked());
    }

    v8::Local<v8::String> isDefaultKey  = v8::String::NewFromUtf8(isolate, "isDefault").ToLocalChecked();
    if ( options->Has(context, isDefaultKey).ToChecked() ){
        isDefault = options->Get(context, isDefaultKey).ToLocalChecked()->BooleanValue(isolate);
    }
    v8::Local<v8::String> isWritableKey = v8::String::NewFromUtf8(isolate, "isWritable").ToLocalChecked();
    if ( options->Has(context, isWritableKey).ToChecked() ){
        isWritable = options->Get(context, isWritableKey).ToLocalChecked()->BooleanValue(isolate);
    }
    if ( isWritable ){
        v8::Local<v8::String> notifyKey = v8::String::NewFromUtf8(isolate, "notify").ToLocalChecked();
        notifyEvent = *v8::String::Utf8Value(isolate, options->Get(context, notifyKey).ToLocalChecked()->ToString(context).ToLocalChecked());
    }

    v8::Local<v8::String> valueKey      = v8::String::NewFromUtf8(e->engine()->isolate(), "value").ToLocalChecked();
    if ( options->Has(context, valueKey).ToChecked() ){
        v8::Local<v8::Value> v = options->Get(context, valueKey).ToLocalChecked();
        v8::Local<v8::String> bindingsKey = v8::String::NewFromUtf8(e->engine()->isolate(), "bindings").ToLocalChecked();
        if ( v->IsFunction() && options->Has(context, bindingsKey).ToChecked() ){
            e->addProperty(name, type, value, isDefault, isWritable, notifyEvent);
            assignPropertyExpression(e, propertyName, ScopedValue(v),
                ScopedValue(options->Get(context, bindingsKey).ToLocalChecked()));
        } else {
            value = ScopedValue(e->engine(), v);
            e->addProperty(name, type, value, isDefault, isWritable, notifyEvent);
        }
    } else {
        e->addProperty(name, type, value, isDefault, isWritable, notifyEvent);
    }
}

void Element::assignPropertyExpression(
        Element *e,
        ScopedValue propertyName,
        ScopedValue propertyExpression,
        ScopedValue bindings)
{
    ElementPrivate* d = e->m_d;

    std::string name    = propertyName.toStdString(e->engine());
    Callable expression = propertyExpression.toCallable(e->engine());
    Property* p = e->property(name);

    auto isolate = e->engine()->isolate();
    auto context = isolate->GetCurrentContext();

    auto it = d->boundPropertyExpressions.find(name);
    if ( it != d->boundPropertyExpressions.end() ){
        Element::BindableExpression* be = it->second;
        EventConnection::remove(be->monitoringConnection);
        for ( auto it = be->bindables.begin(); it != be->bindables.end(); ++it ){
            EventConnection::remove(*it);
        }
        delete it->second;
    }

    // Assign value
    ScopedValue v = expression.call(e->engine(), Function::Parameters(0));
    p->write(e, v.data());

    // Create monitoring expression
    Element::BindableExpression* be = new Element::BindableExpression(expression);
    be->propertyName = name;
    be->element = e;
    be->property = p;
    be->isActive = false;
    try {
        be->monitoringConnection = e->on(p->changedEvent(), [be](const Function::Parameters&){
            if ( be->isActive ){
                be->isActive = false;
            } else {
                be->element->clearPropertyBoundExpression(be->propertyName);
            }
        });
    } catch (lv::Exception exc) {
        e->engine()->throwError(&exc, e);
        return;
    }
    d->boundPropertyExpressions[name] = be;

    v8::Local<v8::Array> bindingsArray = v8::Local<v8::Array>::Cast(bindings.data());
    for ( uint32_t i = 0; i < bindingsArray->Length(); ++i ){
        v8::Local<v8::Array> baItem = v8::Local<v8::Array>::Cast(bindingsArray->Get(context, i).ToLocalChecked());
        if ( baItem->Length() != 2 ){
            lv::Exception exc = CREATE_EXCEPTION(
                lv::Exception, "Bindings require array of dual arrays: [[object, name]].", Exception::toCode("~Binding")
            );
            d->engine->throwError(&exc, e);
            return;
        }

        ScopedValue bindingElementValue = ScopedValue(e->engine(), baItem->Get(context, 0).ToLocalChecked());
        if ( bindingElementValue.isElement() ){
            Element* bindingElement  = bindingElementValue.toElement(e->engine());
            std::string bindingEvent = ScopedValue(e->engine(), baItem->Get(context, 1).ToLocalChecked()).toStdString(e->engine());

            try {
                EventConnection* bec = bindingElement->on(bindingEvent, [be](const Function::Parameters&){
                    be->run();
                });
                be->bindables.push_back(bec);
            } catch (lv::Exception exc) {
                e->engine()->throwError(&exc, e);
                return;
            }
        }
    }
}

void Element::assignChildrenAndComplete(Element *e, ScopedValue value){
    Element::assignChildren(e, value);
    Element::complete(e);
}

void Element::assignChildren(Element *e, ScopedValue value){
    if ( !value.isNull() ){
        if ( e->defaultProperty().empty() ){
            auto exc = CREATE_EXCEPTION(lv::Exception, "Default property doesn't exist", lv::Exception::toCode("~Element"));
            e->engine()->throwError(&exc, e);
            return;
        }

        Property* p = e->property(e->defaultProperty());
        p->write(e, value.data());
    }
}

void Element::complete(Element *e){
    e->onReady();
}

void Element::assignId(Element *e, const std::string &id){
    e->m_d->id = id;
}

void Element::clearPropertyBoundExpression(const std::string &name){
    ElementPrivate* d = m_d;
    auto it = d->boundPropertyExpressions.find(name);
    if ( it != d->boundPropertyExpressions.end() ){
        Element::BindableExpression* be = it->second;
        EventConnection::remove(be->monitoringConnection);
        for ( auto it = be->bindables.begin(); it != be->bindables.end(); ++it ){
            EventConnection::remove(*it);
        }
        delete be;
        d->boundPropertyExpressions.erase(it);
    }
}

const std::string &Element::getId() const{
    return m_d->id;
}

InstanceProperty* Element::addProperty(
        const std::string &name,
        const std::string &type,
        ScopedValue value,
        bool isDefault,
        bool isWritable,
        const std::string &notifyEvent)
{
    if ( type == "default" ){
        isDefault = true;
    }

    ElementPrivate* d = m_d;

    auto isolate = engine()->isolate();
    auto context = isolate->GetCurrentContext();
    if ( d->instanceProperties.find(name) != d->instanceProperties.end() )
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "A property under such name already exists", lv::Exception::toCode("~Element"));
        engine()->throwError(&exc, this);
        return nullptr;
    }

    if ( isDefault && !d->defaultProperty.empty() )
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "Default property already exists", lv::Exception::toCode("~Element"));
        engine()->throwError(&exc, this);
        return nullptr;
    }

    InstanceProperty* ip = new InstanceProperty(this, name, type, value, isWritable, notifyEvent);
    d->instanceProperties[name] = ip;

    if ( isDefault )
        d->defaultProperty = name;
    if ( !notifyEvent.empty() ){
        std::vector<std::string> types;
        types.push_back(type);
        Element::addEvent(this, notifyEvent, types);
    }

    if ( !d->persistent.IsEmpty() ){
        v8::Local<v8::Object> v = d->persistent.Get(engine()->isolate());

        v8::Local<v8::Context> context = d->engine->currentContext()->asLocal();
        v8::Local<v8::External> pdata = v8::External::New(engine()->isolate(), ip);

        if ( isWritable ){
            v->SetAccessor(
                context,
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(d->engine->isolate(), name.c_str()).ToLocalChecked()),
                &Property::ptrGetImplementation,
                &Property::ptrSetImplementation,
                pdata
            ).IsNothing();
        } else {
            v->SetAccessor(
                context,
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(d->engine->isolate(), name.c_str()).ToLocalChecked()),
                &Property::ptrGetImplementation,
                0,
                pdata
            ).IsNothing();
        }
    }

    return ip;
}

const std::string& Element::defaultProperty() const{
    if ( m_d->defaultProperty.empty() )
        return typeMetaObject().defaultProperty();
    return m_d->defaultProperty;
}

Property *Element::property(const std::string &name){
    Property* p = typeMetaObject().getProperty(name);
    if ( !p ){
        auto it = m_d->instanceProperties.find(name);
        if ( it != m_d->instanceProperties.end() )
            p = it->second;
    }
    return p;
}

Property *Element::property(const std::string &name) const{
    Property* p = typeMetaObject().getProperty(name);
    if ( !p ){
        auto it = m_d->instanceProperties.find(name);
        if ( it != m_d->instanceProperties.end() )
            p = it->second;
    }
    return p;
}

bool Element::hasProperty(const std::string &name) const{
    return property(name) != 0;
}

ScopedValue Element::get(const std::string &name){
    Property* p = property(name);
    if ( !p )
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "Property with the given name doesn't exist", lv::Exception::toCode("~Element"));
        engine()->throwError(&exc, this);
        return ScopedValue(engine());
    }

    return p->read(this);
}

void Element::set(const std::string &name, const ScopedValue &value){
    Property* p = property(name);
    if ( !p )
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "Property with the given name doesn't exist", lv::Exception::toCode("~Element"));
        engine()->throwError(&exc, this);
        return;
    }

    p->write(this, value.data());
}

void Element::removeListeningConnection(EventConnection *conn){
    // If there's a listener, remove the connection from it's emission
    if ( conn->listener() ){
        conn->listener()->removeEmitterConnectionDontNotify(conn);
    }
    m_listeningConnections[conn->eventId()]->removeConnection(conn);
}

void Element::removeListeners(){
    // Collect all objects that are listening to this object
    std::set<Element*> listeningElements;
    for ( auto it = m_listeningConnections.begin(); it != m_listeningConnections.end(); ++it )
        it->second->collectListeningElements(listeningElements);

    // Remove this object from their emitters list
    for ( auto it = listeningElements.begin(); it != listeningElements.end(); ++it )
        (*it)->removeEmitterDontNotify(this);

    // Delete all listeners
    auto it = m_listeningConnections.begin();
    while ( it != m_listeningConnections.end() ){
        EventListenerContainerBase* elc = it->second;
        if ( elc->isRunning() ){
            elc->removeConnections();
            ++it;
        } else {
            delete it->second;
            it = m_listeningConnections.erase(it);
        }
    }
}

void Element::removeEmitters(){
    // Itertate emitters one by one, and notify the emitter to delete the connection
    for ( auto it = m_emittingConnections.begin(); it != m_emittingConnections.end(); ++it ){
        EventConnection* conn = *it;
        conn->emitter()->removeListenerConnectionDontNotify(conn);
    }
    m_emittingConnections.clear();
}

void Element::removeInstanceProperties(){
    for ( auto it = m_d->instanceProperties.begin(); it != m_d->instanceProperties.end(); ++it ){
        delete it->second;
    }
    m_d->instanceProperties.clear();
}

void Element::removeBoundPropertyExpressions(){
    for ( auto it = m_d->boundPropertyExpressions.begin(); it != m_d->boundPropertyExpressions.end(); ++it ){
        delete it->second;
    }
    m_d->boundPropertyExpressions.clear();
}

void Element::removeListenerConnectionDontNotify(EventConnection *conn){
    m_listeningConnections[conn->eventId()]->removeConnection(conn);
}

void Element::removeEmitterConnectionDontNotify(EventConnection *conn){
    for ( auto it = m_emittingConnections.begin(); it != m_emittingConnections.end(); ++it ){
        if ( *it == conn ){
            m_emittingConnections.erase(it);
            return;
        }
    }
}

void Element::removeEmitterDontNotify(Element *e){
    auto it = m_emittingConnections.begin();
    while ( it != m_emittingConnections.end() ){
        EventConnection* conn = *it;
        if ( conn->emitter() == e ){
            it = m_emittingConnections.erase(it);
        } else {
            ++it;
        }
    }
}

v8::Local<v8::Object> ElementPrivate::localObject(Element *element){
    ElementPrivate* d = element->m_d;
    if ( d->persistent.IsEmpty() ){
        if ( !d->engine )
        {
            THROW_EXCEPTION(lv::Exception, "Engine should be assigned to Element", lv::Exception::toCode("~ElementPrivate"));
        }

        d->engine->wrapScriptObject(element);
    }
    return d->persistent.Get(d->engine->isolate());
}

Element *ElementPrivate::elementFromObject(const v8::Local<v8::Object> &object){
    if ( object->InternalFieldCount() != 1 )
        THROW_EXCEPTION(lv::Exception, "Object is not of element type.", 1);

    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return reinterpret_cast<Element*>(ptr);
}

}}// namespace lv, el
