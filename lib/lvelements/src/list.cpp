#include "list.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

List::List(Engine *engine)
    : Element(engine)
    , m_data(nullptr)
    , m_isObservable(false)
    , m_at(&List::atImpl)
    , m_length(&List::length)
{
}

List::List(Engine *engine,
        void *data,
        const std::function<LocalValue (List *, int)> &at,
        const std::function<int (List *)> &length,
        bool isObservable)
    : Element(engine)
    , m_data(data)
    , m_isObservable(isObservable)
    , m_at(at)
    , m_length(length)
{
}

List::List(Engine *engine,
        void *data,
        const std::function<LocalValue (List *, int)> &at,
        const std::function<int (List *)> &length,
        const std::function<void (List *, int, LocalValue)> &assign,
        const std::function<void (List *, LocalValue)> &append,
        const std::function<void (List *)> &clear,
        bool isObservable)
    : Element(engine)
    , m_data(data)
    , m_isObservable(isObservable)
    , m_at(at)
    , m_length(length)
    , m_assign(assign)
    , m_append(append)
    , m_clear(clear)
{
}

List::~List(){
    if ( m_dataDestructor )
        m_dataDestructor(this);
}

LocalValue List::atImpl(List *l, int){
    throw std::exception(); //TODO, js exception
    return LocalValue(l->engine());
}

int List::lengthImpl(List*){
    throw std::exception(); //TODO js exception
    return 0;
}

}} // namespace lv, el
