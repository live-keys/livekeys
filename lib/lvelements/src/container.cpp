#include "container.h"
#include "element_p.h"

namespace lv{ namespace el{

Container::Container(Engine* engine)
    : Element(engine)
    , m_children(createList())
{
}

Container::~Container(){
    delete m_children;
}

ScopedValue Container::children(){
    return ScopedValue(engine(), m_children);
}

void Container::setChildren(ScopedValue children){
    v8::Local<v8::Value> data = children.data();
    foreach( Element* e, m_data ){
        e->setParent(nullptr);
    }
    m_data.clear();

    if ( data->IsArray()){
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(data);
        for ( unsigned int i = 0; i < arr->Length(); ++i ){
            ScopedValue v(arr->Get(i));
            Element* e = v.toElement(engine());
            e->setParent(this);
            m_data.push_back(e);
        }
    }
    m_children->valuesReset();
    childrenChanged();
}

List *Container::createList(){
    return new List(engine(), &m_data, &at, &length, &assign, &append, &clear);
}

}} // namespace lv, el
