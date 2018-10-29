/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "live/mlnode.h"
#include "live/visuallog.h"
#include "assert.h"
#include <QByteArray>

namespace lv{

// MLNode::BytesType
// ----------------------------------------------------------------------------

MLNode::BytesType::BytesType(unsigned char *data, size_t size)
    : m_data(new unsigned char[size])
    , m_size(size)
    , m_ref(new int)
{
    ++(*m_ref);
    memcpy((void*)m_data, (void*)data, size);
}

MLNode::BytesType::BytesType(const MLNode::BytesType &other)
    : m_data(other.m_data)
    , m_size(other.m_size)
    , m_ref(other.m_ref)
{
    ++(*m_ref);
}

MLNode::BytesType::BytesType()
    : m_data(0)
    , m_size(0)
    , m_ref(new int)
{
    ++(*m_ref);
}

MLNode::BytesType::~BytesType(){
    --(*m_ref);
    if ( *m_ref == 0 ){
        delete m_ref;
        delete[] m_data;
    }
}

MLNode::BytesType &MLNode::BytesType::operator=(const MLNode::BytesType &other){
    if ( this != &other ){
        --(*m_ref);
        if ( *m_ref == 0 ){
            delete[] m_data;
            delete m_ref;
        }

        m_data = other.m_data;
        m_ref  = other.m_ref;
        ++(*m_ref);
    }
    return *this;
}

bool MLNode::BytesType::operator ==(const MLNode::BytesType &other) const{
    return (m_data == other.m_data && m_ref == other.m_ref);
}

MLNode::StringType MLNode::BytesType::toBase64String(){
    return MLNode::StringType(QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_data), static_cast<int>(m_size)).toBase64().data()
    );
}

QByteArray MLNode::BytesType::toBase64(){
    return QByteArray::fromRawData(reinterpret_cast<const char*>(m_data), static_cast<int>(m_size)).toBase64();
}

MLNode::BytesType MLNode::BytesType::fromBase64(const MLNode::StringType &str){
    QByteArray result = QByteArray::fromBase64(QByteArray::fromRawData(str.c_str(), static_cast<int>(str.size())));
    return MLNode::BytesType(reinterpret_cast<MLNode::ByteType*>(result.data()), result.size());
}

MLNode::ByteType *MLNode::BytesType::data(){
    return m_data;
}

size_t MLNode::BytesType::size() const{
    return m_size;
}

// MLNode::Iterator
// ----------------------------------------------------------------------------


MLNode::Iterator::Iterator(MLNode::Iterator::Pointer object)
    : m_object(object)
{
    assert(m_object != nullptr);

    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator = ObjectType::iterator();
        break;
    case MLNode::Array:
        m_it.arrayIterator = ArrayType::iterator();
        break;
    default:
        m_it.primitiveIterator = false;
        break;
    }
}

MLNode::Iterator::Iterator(const MLNode::Iterator &other)
    : m_object(other.m_object)
    , m_it(other.m_it)
{
}

MLNode::Iterator &MLNode::Iterator::operator=(const MLNode::Iterator &other){
    m_object = other.m_object;
    m_it     = other.m_it;
    return *this;
}

bool MLNode::Iterator::canCompareTo(const MLNode::Iterator &other) const{
    return m_object == other.m_object;
}

bool MLNode::Iterator::operator==(const MLNode::Iterator &other) const{
    assert(m_object != nullptr);
    if ( !canCompareTo(other) ){
        THROW_EXCEPTION(InvalidMLTypeException, "Iterators are of different type.", 0);
    }

    switch(m_object->m_type){
    case MLNode::Object:
        return m_it.objectIterator == other.m_it.objectIterator;
    case MLNode::Array:
        return m_it.arrayIterator == other.m_it.arrayIterator;
    default:
        return m_it.primitiveIterator == other.m_it.primitiveIterator;
    }
}

bool MLNode::Iterator::operator!=(const MLNode::Iterator &other) const{
    return !(*this == other);
}

bool MLNode::Iterator::operator <(const MLNode::Iterator &other) const{
    assert(m_object != nullptr);
    if ( !canCompareTo(other) ){
        THROW_EXCEPTION(InvalidMLTypeException, "Iterators are of different type.", 0);
    }
    switch(m_object->m_type){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot compare object iterators.", 0);
    case MLNode::Array:
        return m_it.arrayIterator < other.m_it.arrayIterator;
    default:
        return (m_it.primitiveIterator == true && other.m_it.primitiveIterator == false);
    }
}

bool MLNode::Iterator::operator<=(const MLNode::Iterator &other) const{
    return !(other < *this);
}

bool MLNode::Iterator::operator>(const MLNode::Iterator &other) const{
    return !(*this <= (other));
}

bool MLNode::Iterator::operator>=(const MLNode::Iterator &other) const{
    return !(*this < other);
}

MLNode::Iterator &MLNode::Iterator::operator++(){
    assert(m_object != nullptr);
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator++;
        break;
    case MLNode::Array:
        m_it.arrayIterator++;
        break;
    default:
        if ( m_it.primitiveIterator == true )
            m_it.primitiveIterator = false;
        break;
    }

    return *this;
}

MLNode::Iterator &MLNode::Iterator::operator--(){
    assert(m_object != nullptr);
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator--;
        break;
    case MLNode::Array:
        m_it.arrayIterator--;
        break;
    default:
        if ( m_it.primitiveIterator == false )
            m_it.primitiveIterator = true;
        break;
    }

    return *this;
}

MLNode::Iterator &MLNode::Iterator::operator+=(MLNode::Iterator::DifferenceType i){
    switch( m_object->m_type ){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with object iterators", 1);
    case MLNode::Array:
        m_it.arrayIterator += i;
        break;
    default:
        if ( m_it.primitiveIterator == false && i > 0)
            m_it.primitiveIterator = true;
        else if ( m_it.primitiveIterator == true && i < 0)
            m_it.primitiveIterator = false;
        break;
    }

    return *this;
}

MLNode::Iterator &MLNode::Iterator::operator-=(MLNode::Iterator::DifferenceType i){
    return operator+=(-i);
}

MLNode::Iterator MLNode::Iterator::operator+(MLNode::Iterator::DifferenceType i){
    auto result = *this;
    result += i;
    return result;
}

MLNode::Iterator::DifferenceType MLNode::Iterator::operator-(const MLNode::Iterator &other) const{
    assert(m_object != nullptr);

    switch(m_object->m_type){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with object iterators.", 0);
    case MLNode::Array:
        return m_it.arrayIterator - other.m_it.arrayIterator;
    default:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with primitive iterators.", 0);
    }
}

MLNode::Iterator::Reference MLNode::Iterator::operator*() const{
    assert(m_object != nullptr);
    switch ( m_object->m_type ){
    case MLNode::Object:
        assert(m_it.objectIterator != m_object->m_value.asObject->end());
        return m_it.objectIterator->second;
    case MLNode::Array:
        assert(m_it.arrayIterator != m_object->m_value.asArray->end());
        return *m_it.arrayIterator;
    case MLNode::Null:
        THROW_EXCEPTION(MLOutOfRanceException, "Null value given.", 2);
    default:
        if ( m_it.primitiveIterator == true )
            return *m_object;
        THROW_EXCEPTION(MLOutOfRanceException, "Iterator at end.", 2);
    }
}

MLNode::Iterator::Pointer MLNode::Iterator::operator->() const{
    assert(m_object != nullptr);
    switch (m_object->m_type){
    case MLNode::Object:
        assert(m_it.objectIterator != m_object->m_value.asObject->end());
        return &(m_it.objectIterator->second);
    case MLNode::Array:
        assert(m_it.arrayIterator != m_object->m_value.asArray->end());
        return &*m_it.arrayIterator;
    default:
        if ( m_it.primitiveIterator == true )
            return m_object;
        THROW_EXCEPTION(MLOutOfRanceException, "Iterator at end.", 2);
    }
}

MLNode::Iterator::Reference MLNode::Iterator::operator[](MLNode::Iterator::DifferenceType i) const{
    assert(m_object != nullptr);

    switch ( m_object->m_type ){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with object iterators.", 0);
    case MLNode::Array:
        return m_it.arrayIterator[i];
    default:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with primitive iterators.", 0);
    }
}

MLNode::ObjectType::key_type MLNode::Iterator::key() const{
    assert(m_object != nullptr);
    if ( m_object->m_type == MLNode::Object )
        return m_it.objectIterator->first;

    THROW_EXCEPTION(InvalidMLTypeException, "Cannot use key with non-object iterators.", 0);
}

MLNode::Iterator::Reference MLNode::Iterator::value() const{
    return operator*();
}

void MLNode::Iterator::positionAtBegin(){
    switch (m_object->m_type ){
    case MLNode::Object:
        m_it.objectIterator = m_object->m_value.asObject->begin();
        break;
    case MLNode::Array:
        m_it.arrayIterator = m_object->m_value.asArray->begin();
        break;
    case MLNode::Null:
        m_it.primitiveIterator = false;
        break;
    default:
        m_it.primitiveIterator = true;
        break;
    }
}

void MLNode::Iterator::positionAtEnd(){
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator = m_object->m_value.asObject->end();
        break;
    case MLNode::Array:
        m_it.arrayIterator = m_object->m_value.asArray->end();
        break;
    default:
        m_it.primitiveIterator = true;
        break;
    }
}

MLNode::Iterator MLNode::Iterator::operator-(MLNode::Iterator::DifferenceType i){
    auto result = *this;
    result -= i;
    return result;
}

// MLNode::ConstIterator
// ----------------------------------------------------------------------------


MLNode::ConstIterator::ConstIterator(MLNode::ConstIterator::ConstPointer object)
    : m_object(object)
{
    assert(m_object != nullptr);

    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator = ObjectType::const_iterator();
        break;
    case MLNode::Array:
        m_it.arrayIterator = ArrayType::const_iterator();
        break;
    default:
        m_it.primitiveIterator = false;
        break;
    }
}

MLNode::ConstIterator::ConstIterator(const MLNode::Iterator &other)
    : m_object(other.m_object)
{
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator = other.m_it.objectIterator;
        break;
    case MLNode::Array:
        m_it.arrayIterator = other.m_it.arrayIterator;
        break;
    default:
        m_it.primitiveIterator = other.m_it.primitiveIterator;
        break;
    }
}

MLNode::ConstIterator::ConstIterator(const MLNode::ConstIterator &other)
    : m_object(other.m_object)
    , m_it(other.m_it)
{
}

MLNode::ConstIterator &MLNode::ConstIterator::operator=(const MLNode::ConstIterator &other){
    m_object = other.m_object;
    m_it     = other.m_it;
    return *this;
}

bool MLNode::ConstIterator::canCompareTo(const MLNode::ConstIterator &other) const{
    return m_object == other.m_object;
}

bool MLNode::ConstIterator::operator==(const MLNode::ConstIterator &other) const{
    assert(m_object != nullptr);
    if ( !canCompareTo(other) ){
        THROW_EXCEPTION(InvalidMLTypeException, "Iterators are of different type.", 0);
    }

    switch(m_object->m_type){
    case MLNode::Object:
        return m_it.objectIterator == other.m_it.objectIterator;
    case MLNode::Array:
        return m_it.arrayIterator == other.m_it.arrayIterator;
    default:
        return m_it.primitiveIterator == other.m_it.primitiveIterator;
    }
}

bool MLNode::ConstIterator::operator!=(const MLNode::ConstIterator &other) const{
    return !(*this == other);
}

bool MLNode::ConstIterator::operator <(const MLNode::ConstIterator &other) const{
    assert(m_object != nullptr);
    if ( !canCompareTo(other) ){
        THROW_EXCEPTION(InvalidMLTypeException, "Iterators are of different type.", 0);
    }
    switch(m_object->m_type){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot compare object iterators.", 0);
    case MLNode::Array:
        return m_it.arrayIterator < other.m_it.arrayIterator;
    default:
        return (m_it.primitiveIterator == true && other.m_it.primitiveIterator == false);
    }
}

bool MLNode::ConstIterator::operator<=(const MLNode::ConstIterator &other) const{
    return !(other < *this);
}

bool MLNode::ConstIterator::operator>(const MLNode::ConstIterator &other) const{
    return !(*this <= (other));
}

bool MLNode::ConstIterator::operator>=(const MLNode::ConstIterator &other) const{
    return !(*this < other);
}

MLNode::ConstIterator &MLNode::ConstIterator::operator++(){
    assert(m_object != nullptr);
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator++;
        break;
    case MLNode::Array:
        m_it.arrayIterator++;
        break;
    default:
        if ( m_it.primitiveIterator == true )
            m_it.primitiveIterator = false;
        break;
    }

    return *this;
}

MLNode::ConstIterator &MLNode::ConstIterator::operator--(){
    assert(m_object != nullptr);
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator--;
        break;
    case MLNode::Array:
        m_it.arrayIterator--;
        break;
    default:
        if ( m_it.primitiveIterator == false )
            m_it.primitiveIterator = true;
        break;
    }

    return *this;
}

MLNode::ConstIterator &MLNode::ConstIterator::operator+=(MLNode::ConstIterator::DifferenceType i){
    switch( m_object->m_type ){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with object iterators", 1);
    case MLNode::Array:
        m_it.arrayIterator += i;
        break;
    default:
        if ( m_it.primitiveIterator == false && i > 0)
            m_it.primitiveIterator = true;
        else if ( m_it.primitiveIterator == true && i < 0)
            m_it.primitiveIterator = false;
        break;
    }

    return *this;
}

MLNode::ConstIterator &MLNode::ConstIterator::operator-=(MLNode::ConstIterator::DifferenceType i){
    return operator+=(-i);
}

MLNode::ConstIterator MLNode::ConstIterator::operator+(MLNode::ConstIterator::DifferenceType i){
    auto result = *this;
    result += i;
    return result;
}

MLNode::ConstIterator::DifferenceType MLNode::ConstIterator::operator-(const MLNode::ConstIterator &other) const{
    assert(m_object != nullptr);

    switch(m_object->m_type){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with object iterators.", 0);
    case MLNode::Array:
        return m_it.arrayIterator - other.m_it.arrayIterator;
    default:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with primitive iterators.", 0);
    }
}

MLNode::ConstIterator::ConstReference MLNode::ConstIterator::operator*() const{
    assert(m_object != nullptr);
    switch ( m_object->m_type ){
    case MLNode::Object:
        assert(m_it.objectIterator != m_object->m_value.asObject->end());
        return m_it.objectIterator->second;
    case MLNode::Array:
        assert(m_it.arrayIterator != m_object->m_value.asArray->end());
        return *m_it.arrayIterator;
    case MLNode::Null:
        THROW_EXCEPTION(MLOutOfRanceException, "Null value given.", 2);
    default:
        if ( m_it.primitiveIterator == true )
            return *m_object;
        THROW_EXCEPTION(MLOutOfRanceException, "Iterator at end.", 2);
    }
}

MLNode::ConstIterator::ConstPointer MLNode::ConstIterator::operator->() const{
    assert(m_object != nullptr);
    switch (m_object->m_type){
    case MLNode::Object:
        assert(m_it.objectIterator != m_object->m_value.asObject->end());
        return &(m_it.objectIterator->second);
    case MLNode::Array:
        assert(m_it.arrayIterator != m_object->m_value.asArray->end());
        return &*m_it.arrayIterator;
    default:
        if ( m_it.primitiveIterator == true )
            return m_object;
        THROW_EXCEPTION(MLOutOfRanceException, "Iterator at end.", 2);
    }
}

MLNode::ConstIterator::ConstReference MLNode::ConstIterator::operator[](MLNode::ConstIterator::DifferenceType i) const{
    assert(m_object != nullptr);

    switch ( m_object->m_type ){
    case MLNode::Object:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with object iterators.", 0);
    case MLNode::Array:
        return m_it.arrayIterator[i];
    default:
        THROW_EXCEPTION(InvalidMLTypeException, "Cannot use offsets with primitive iterators.", 0);
    }
}

MLNode::ObjectType::key_type MLNode::ConstIterator::key() const{
    assert(m_object != nullptr);
    if ( m_object->m_type == MLNode::Object )
        return m_it.objectIterator->first;

    THROW_EXCEPTION(InvalidMLTypeException, "Cannot use key with non-object iterators.", 0);
}

MLNode::ConstIterator::ConstReference MLNode::ConstIterator::value() const{
    return operator*();
}

void MLNode::ConstIterator::positionAtBegin(){
    switch (m_object->m_type ){
    case MLNode::Object:
        m_it.objectIterator = m_object->m_value.asObject->begin();
        break;
    case MLNode::Array:
        m_it.arrayIterator = m_object->m_value.asArray->begin();
        break;
    case MLNode::Null:
        m_it.primitiveIterator = false;
        break;
    default:
        m_it.primitiveIterator = true;
        break;
    }
}

void MLNode::ConstIterator::positionAtEnd(){
    switch(m_object->m_type){
    case MLNode::Object:
        m_it.objectIterator = m_object->m_value.asObject->end();
        break;
    case MLNode::Array:
        m_it.arrayIterator = m_object->m_value.asArray->end();
        break;
    default:
        m_it.primitiveIterator = true;
        break;
    }
}

MLNode::ConstIterator MLNode::ConstIterator::operator-(MLNode::ConstIterator::DifferenceType i){
    auto result = *this;
    result -= i;
    return result;
}

// MLNode
// ----------------------------------------------------------------------------

MLNode::MLNode()
    : m_type(Type::Null)
    , m_value()
{
}

MLNode::MLNode(const std::initializer_list<MLNode> &init)
{
    bool isObject = std::all_of(
                init.begin(), init.end(), [](const MLNode& element){
        return  element.type() == MLNode::Array &&
                element.size() == 2 &&
                element[0].type() == MLNode::String;
    }
    );

    if ( isObject ){
        m_type  = Type::Object;
        m_value = Type::Object;
        std::for_each(init.begin(), init.end(), [this](const MLNode& element){
            (*m_value.asObject)[*(element[0].m_value.asString)] = element[1];
        });
    } else {
        m_type  = Type::Array;
        m_value = Type::Array;
        *(m_value.asArray) = init;
    }
}

MLNode::MLNode(std::nullptr_t)
    : m_type(Type::Null)
    , m_value()
{
}

MLNode::MLNode(const char *value)
    : m_type(Type::String)
    , m_value(StringType(value))
{
}

MLNode::MLNode(const MLNode::StringType &value)
    : m_type(Type::String)
    , m_value(value)
{
}

MLNode::MLNode(MLNode::Type value)
    : m_type(value)
    , m_value(value){
}

MLNode::MLNode(float value)
    : m_type(Type::Float)
    , m_value((double)value)
{
}

MLNode::MLNode(MLNode::FloatType value)
    : m_type(Type::Float)
    , m_value(value)
{
}

MLNode::MLNode(int value)
    : m_type(Type::Integer)
    , m_value((IntType)value)
{
}

MLNode::MLNode(MLNode::IntType value)
    : m_type(Type::Integer)
    , m_value(value)
{
}

MLNode::MLNode(MLNode::BoolType value)
    : m_type(Type::Boolean)
    , m_value(value)
{
}

MLNode::MLNode(const MLNode::BytesType &value)
   : m_type(Type::Bytes)
   , m_value(value)
{
}

MLNode::MLNode(lv::MLNode::ByteType *value, size_t size)
    : m_type(Type::Bytes)
    , m_value(MLValue(value, size))
{
}

MLNode::MLNode(const MLNode::ArrayType &value)
    : m_type(Type::Array)
    , m_value(value)
{
}

MLNode::MLNode(const MLNode::ObjectType &value)
    : m_type(Type::Object)
    , m_value(value)
{
}

MLNode::MLNode(const MLNode &other)
    : m_type(other.m_type)
{
    switch(m_type){
    case Type::Null: break;
    case Type::Object:  m_value = *other.m_value.asObject; break;
    case Type::Array:   m_value = *other.m_value.asArray; break;
    case Type::Bytes:   m_value = *other.m_value.asBytes; break;
    case Type::String:  m_value = *other.m_value.asString; break;
    case Type::Boolean: m_value = other.m_value.asBool; break;
    case Type::Integer: m_value = other.m_value.asInt; break;
    case Type::Float:   m_value = other.m_value.asFloat; break;
    }
}

MLNode::MLNode(MLNode &&other)
    : m_type(other.m_type)
    , m_value(other.m_value)
{
    other.m_type  = MLNode::Type::Null;
    other.m_value = {};
}

MLNode::~MLNode(){
    switch(m_type){
    case Type::Object: delete m_value.asObject; break;
    case Type::Array:  delete m_value.asArray; break;
    case Type::Bytes:  delete m_value.asBytes; break;
    case Type::String: delete m_value.asString; break;
    default: break;
    }
}

MLNode &MLNode::operator[](int index){
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    return (*m_value.asArray)[index];
}

void MLNode::append(const MLNode &value){
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    m_value.asArray->push_back(value);
}

bool MLNode::isNull() const{
    if ( m_type == Type::Null )
        return true;
    return false;
}

const MLNode &MLNode::operator[](int index) const{
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    return (*m_value.asArray)[index];
}

MLNode &MLNode::operator[](const MLNode::StringType &reference){
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    return (*m_value.asObject)[reference];
}

const MLNode &MLNode::operator[](const MLNode::StringType &reference) const{
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    return (*m_value.asObject)[reference];
}

void MLNode::toStringImpl(std::ostream &o, int indent, int indentStep) const{
    switch(m_type){
    case Type::Object:
        if ( m_value.asObject->empty() ){
            o << "{}";
            return;
        }

        o << "{";
        if ( indent >= 0 ){
            indent += indentStep;
            o << "\n";
        }
        for ( auto it = m_value.asObject->cbegin(); it != m_value.asObject->cend(); ++it ){
            if ( it != m_value.asObject->cbegin() )
                o << (indent >= 0 ? ",\n" : ",");
            o << (indent >= 0 ? StringType(indent, ' ') : "")
              << "\"" << it->first << "\":"
              << (indent >= 0 ? " " : "");

            it->second.toStringImpl(o, indent, indentStep);
        }

        if ( indent >= 0 ){
            indent -= indentStep;
            o << "\n";
        }

        o << (indent >= 0 ? StringType(indent, ' ') : "") << "}";

        return;

    case Type::Array:
        if ( m_value.asArray->empty() ){
            o << "[]";
            return;
        }

        o << "[";

        if ( indent >= 0 ){
            indent += indentStep;
            o << "\n";
        }

        for ( auto it = m_value.asArray->cbegin(); it != m_value.asArray->cend(); ++it ){
            if ( it != m_value.asArray->cbegin() ){
                o << (indent >= 0 ? ",\n" : ",");
            }
            o << (indent >= 0 ? StringType(indent, ' ') : "");
            it->toStringImpl(o, indent, indentStep);
        }

        if ( indent >= 0 ){
            indent -= indentStep;
            o << "\n";
        }

        o << (indent >= 0 ? StringType(indent, ' ') : "") << "]";

        return;
    case Type::String:
        o << StringType("\"") << *m_value.asString << "\"";
        return;
    case Type::Boolean:
        o << (m_value.asBool ? "true" : "false");
        return;
    case Type::Integer:
        o << m_value.asInt;
        return;
    case Type::Float:
        o << m_value.asFloat;
        return;
    case Type::Bytes:
    {
        QByteArray b64 = m_value.asBytes->toBase64();
        o << StringType("\"") << b64.data() << "\"";
    }
        return;
    case Type::Null:
        o << "null";
        return;
    }
}

std::string MLNode::typeString() const{
    switch(m_type){
    case Type::Null:    return "null";
    case Type::Object:  return "object";
    case Type::Array:   return "array";
    case Type::Bytes:   return "bytes";
    case Type::String:  return "string";
    case Type::Boolean: return "boolean";
    case Type::Integer: return "integer";
    case Type::Float:   return "float";
    default:return "";
    }
    return "";
}

std::string MLNode::toString(int indent, int indentStep) const{
    std::stringstream stream;
    toStringImpl(stream, indent, indentStep);
    return stream.str();
}

MLNode::Iterator MLNode::begin(){
    Iterator it(this);
    it.positionAtBegin();
    return it;
}

MLNode::Iterator MLNode::end(){
    Iterator it(this);
    it.positionAtEnd();
    return it;
}

MLNode::ConstIterator MLNode::begin() const{
    return cbegin();
}

MLNode::ConstIterator MLNode::end() const{
    return cend();
}

MLNode::ConstIterator MLNode::cbegin() const{
    ConstIterator it(this);
    it.positionAtBegin();
    return it;
}

MLNode::ConstIterator MLNode::cend() const{
    ConstIterator it(this);
    it.positionAtEnd();
    return it;
}

int MLNode::asInt() const{
    if ( m_type != Type::Integer )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of integer type.", 0);

    return static_cast<int>(m_value.asInt);
}

bool MLNode::asBool() const{
    if ( m_type != Type::Boolean )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of boolean type.", 0);

    return m_value.asBool;
}

MLNode::FloatType MLNode::asFloat() const{
    if ( m_type != Type::Float )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of float type.", 0);

    return m_value.asFloat;
}

MLNode::StringType MLNode::asString() const{
    if ( m_type != Type::String )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of string type.", 0);

    return *m_value.asString;
}

MLNode::BytesType MLNode::asBytes() const{
    if ( m_type == Type::Bytes ){
        return *m_value.asBytes;
    } else if ( m_type == Type::String ){
        return MLNode::BytesType::fromBase64(*m_value.asString);
    } else
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of bytes type.", 0);
}

const MLNode::ArrayType &MLNode::asArray() const{
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    return *m_value.asArray;
}

int MLNode::size() const{
    if ( m_type == Type::Array ){
        return static_cast<int>(m_value.asArray->size());
    } else if ( m_type == Type::Object ){
        return static_cast<int>(m_value.asObject->size());
    } else {
        return 0;
    }
}

bool MLNode::hasKey(const MLNode::StringType &key) const{
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    auto it = m_value.asObject->find(key);
    return it != m_value.asObject->end();
}

void MLNode::remove(const MLNode::StringType &key){
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    m_value.asObject->erase(key);
}


VisualLog &operator <<(VisualLog &vl, const MLNode &value){
    return vl << value.toString().c_str();
}

}

