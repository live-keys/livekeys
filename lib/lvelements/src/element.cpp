#include "element.h"
#include "element_p.h"
#include "context_p.h"
#include "live/exception.h"
#include "v8.h"

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
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(engine()->isolate(), p->name().c_str())),
                &Property::ptrGetImplementation,
                &Property::ptrSetImplementation,
                pdata
            );
        } else {
            obj->SetAccessor(
                context,
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(engine()->isolate(), p->name().c_str())),
                &Property::ptrGetImplementation,
                0,
                pdata
            );
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
    return !m_d->persistent.IsNearDeath();
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
            throw std::exception();

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

void Element::on(LocalValue eventKey, Callable callback){
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
            throw std::exception();

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
        throw std::exception();

    InstanceEvent* ie = new InstanceEvent(
        e, EventIdGenerator::concatenate(0, 0, d->instanceEvents.size()), static_cast<int>(types.size())
    );
    d->instanceEvents[ekey] = ie;

    if ( !d->persistent.IsEmpty() ){
        v8::Local<v8::Object> v = d->persistent.Get(e->engine()->isolate());
        v8::Local<v8::External> fdata = v8::External::New(d->engine->isolate(), ie);

        v->Set(
            v8::String::NewFromUtf8(d->engine->isolate(), ekey.c_str()),
            v8::FunctionTemplate::New(d->engine->isolate(), &InstanceEvent::callbackImplementation, fdata)->GetFunction());
    }
}

void Element::addEvent(Element *e, LocalValue eventKey, LocalValue types){
    ElementPrivate* d = e->m_d;
    std::string ekey = eventKey.toStdString(e->engine());

    if ( d->instanceEvents.find(ekey) != d->instanceEvents.end() )
        throw std::exception();

    v8::Local<v8::Array> typesArray = v8::Local<v8::Array>::Cast(types.data());

    InstanceEvent* ie = new InstanceEvent(
        e, EventIdGenerator::concatenate(0, 0, d->instanceEvents.size()), typesArray->Length()
    );
    d->instanceEvents[ekey] = ie;

    if ( !d->persistent.IsEmpty() ){
        v8::Local<v8::Object> v = d->persistent.Get(e->engine()->isolate());
        v8::Local<v8::External> fdata = v8::External::New(d->engine->isolate(), ie);

        v->Set(
            v8::String::NewFromUtf8(d->engine->isolate(), ekey.c_str()),
            v8::FunctionTemplate::New(d->engine->isolate(), &InstanceEvent::callbackImplementation, fdata)->GetFunction());
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
void Element::addProperty(Element *e, LocalValue propertyName, LocalValue propertyOptions){
    std::string name = propertyName.toStdString(e->engine());
    std::string type = "variant";
    LocalValue value(e->engine());
    bool isDefault   = false;
    bool isWritable  = true;
    std::string notifyEvent;

    std::list<std::pair<Element*, std::string> > bindings;

    v8::Local<v8::Object> options = propertyOptions.data()->ToObject();

    v8::Local<v8::String> typeKey       = v8::String::NewFromUtf8(e->engine()->isolate(), "type");
    if ( options->Has(typeKey) ){
        type = *v8::String::Utf8Value(options->Get(typeKey)->ToString());
    }

    v8::Local<v8::String> isDefaultKey  = v8::String::NewFromUtf8(e->engine()->isolate(), "isDefault");
    if ( options->Has(isDefaultKey) ){
        isDefault = options->Get(isDefaultKey)->BooleanValue();
    }
    v8::Local<v8::String> isWritableKey = v8::String::NewFromUtf8(e->engine()->isolate(), "isWritable");
    if ( options->Has(isWritableKey) ){
        isWritable = options->Get(isWritableKey)->BooleanValue();
    }
    if ( isWritable ){
        v8::Local<v8::String> notifyKey = v8::String::NewFromUtf8(e->engine()->isolate(), "notify");
        notifyEvent = *v8::String::Utf8Value(options->Get(notifyKey)->ToString());
    }

    v8::Local<v8::String> valueKey      = v8::String::NewFromUtf8(e->engine()->isolate(), "value");
    if ( options->Has(valueKey) ){
        v8::Local<v8::Value> v = options->Get(valueKey);
        v8::Local<v8::String> bindingsKey = v8::String::NewFromUtf8(e->engine()->isolate(), "bindings");
        if ( v->IsFunction() && options->Has(bindingsKey) ){
            e->addProperty(name, type, value, isDefault, isWritable, notifyEvent);
            assignPropertyExpression(e, propertyName, LocalValue(v), LocalValue(options->Get(bindingsKey)));
        } else {
            value = LocalValue(e->engine(), v);
            e->addProperty(name, type, value, isDefault, isWritable, notifyEvent);
        }
    } else {
        e->addProperty(name, type, value, isDefault, isWritable, notifyEvent);
    }
}

void Element::assignPropertyExpression(
        Element *e,
        LocalValue propertyName,
        LocalValue propertyExpression,
        LocalValue bindings)
{
    ElementPrivate* d = e->m_d;

    std::string name    = propertyName.toStdString(e->engine());
    Callable expression = propertyExpression.toCallable(e->engine());
    Property* p = e->property(name);

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
    LocalValue v = expression.call(e->engine(), Function::Parameters(0));
    p->write(e, v.data());

    // Create monitoring expression
    Element::BindableExpression* be = new Element::BindableExpression(expression);
    be->propertyName = name;
    be->element = e;
    be->property = p;
    be->isActive = false;
    be->monitoringConnection = e->on(p->changedEvent(), [be](const Function::Parameters&){
        if ( be->isActive ){
            be->isActive = false;
        } else {
            be->element->clearPropertyBoundExpression(be->propertyName);
        }
    });
    d->boundPropertyExpressions[name] = be;

    v8::Local<v8::Array> bindingsArray = v8::Local<v8::Array>::Cast(bindings.data());
    for ( uint32_t i = 0; i < bindingsArray->Length(); ++i ){
        v8::Local<v8::Array> baItem = v8::Local<v8::Array>::Cast(bindingsArray->Get(i));
        if ( baItem->Length() != 2 ){
            lv::Exception exc = CREATE_EXCEPTION(
                lv::Exception, "Bindings require array of dual arrays: [[object, name]].", Exception::toCode("~Binding")
            );
            d->engine->throwError(&exc, e);
            return;
        }

        Element* bindingElement  = LocalValue(e->engine(), baItem->Get(0)).toElement(e->engine());
        std::string bindingEvent = LocalValue(e->engine(), baItem->Get(1)).toStdString(e->engine());

        EventConnection* bec = bindingElement->on(bindingEvent, [be](const Function::Parameters&){
            be->run();
        });

        be->bindables.push_back(bec);
    }
}

void Element::assignDefaultProperty(Element *e, LocalValue value){
    if ( e->defaultProperty().empty() )
        throw std::exception();

    Property* p = e->property(e->defaultProperty());
    p->write(e, value.data());
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
        LocalValue value,
        bool isDefault,
        bool isWritable,
        const std::string &notifyEvent)
{
    ElementPrivate* d = m_d;

    if ( d->instanceProperties.find(name) != d->instanceProperties.end() )
        throw std::exception();

    if ( isDefault && !d->defaultProperty.empty() )
        throw std::exception();

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
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(d->engine->isolate(), name.c_str())),
                &Property::ptrGetImplementation,
                &Property::ptrSetImplementation,
                pdata
            );
        } else {
            v->SetAccessor(
                context,
                v8::Local<v8::Name>::Cast(v8::String::NewFromUtf8(d->engine->isolate(), name.c_str())),
                &Property::ptrGetImplementation,
                0,
                pdata
            );
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

LocalValue Element::get(const std::string &name){
    Property* p = property(name);
    if ( !p )
        throw std::exception();

    return p->read(this);
}

void Element::set(const std::string &name, const LocalValue &value){
    Property* p = property(name);
    if ( !p )
        throw std::exception();

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
            throw std::exception();

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

}}// namespace lv, namespace script
