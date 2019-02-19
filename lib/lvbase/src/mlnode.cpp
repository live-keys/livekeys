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

/**
  \class lv::MLNode
  \brief Provides an intermediate representation of objects that can later be converted to a markup language

  *
  * MLNode provides a wrapper node representation of generally used types, such as <b>ints</b>, <b>floats</b>,
  * <b>bools</b>, <b>strings</b>, <b>arrays</b> and <b>objects</b>, as well as a special type of <b>bytes</b>, which represents an
  * array of unsigned chars to be used as a form of data stream. Object and Array MLNodes can contain other MLNodes inside of them,
  * which provides nesting capabilites e.g. you could have an Array of Objects which map other Arrays. In a way, it's a simple, generic
  * type of node to represent a variety of data objects. The ML abbreviation hints at its purpose, to be used for conversion to markup languages.
  *
  *
  * MLNodes can be created through numerous constructors
  * ```
  * MLNode()                        // MLNode::Null
    MLNode("str")                   // MLNode::String
    MLNode(MLNode::StringType()     // MLNode::String
    MLNode(200)                     // MLNode::Integer
    MLNode(200.2)                   // MLNode::Float
    MLNode(true)                    // MLNode::Boolean
    MLNode(MLNode::BytesType())     // MLNode::Bytes
    MLNode(MLNode::ObjectType))     // MLNode::Object
    MLNode(MLNode::ArrayType())     // MLNode::Array
    MLNode n({100, 200, 300});      // MLNode::Array
    MLNode nObject({
        {"object", "value"}
    });                             // MLNode::Object

  * ```
  *
  * They can be initialized during assignment in simple and intuitive ways
  * ```
  * MLNode n = 100;                 // MLNode::Integer
  * MLNode n = 100.20;              // MLNode::Float
  * MLNode n = {100, 200, 300};     // MLNode::Array
  * MLNode n = {
        {"object", {
             {"key1", "value1"},
             {"key2", 100}
        }},
        {"array", { 100, "200", false}},
        {"bool", true},
        {"int", 100},
        {"float", 100.1}
    };                              // MLNode::Object
  * ```
  *
  * The simplest depiction of the inner structure of MLNode would be a simple <b>type</b> variable, indicating which
  * MLNode we have (\sa enum MLNode::Type), as well as <b>a flexible union type as its value</b>. This union is as small as possible,
  * containing primitive types as is, and using appropriate pointers for larger objects. Knowing the type, we can always
  * use the value by simply accessing the appropriate union field e.g. asObject, asInt. Simplified version of that union is displayed below.
  *
  * ```
  * union MLValue{
        ObjectType*  asObject;
        ArrayType*   asArray;
        BytesType*   asBytes;
        StringType*  asString;
        BoolType     asBool;
        IntType      asInt;
        FloatType    asFloat;

        // ... constructors for each type ...
    };
  * ```
  * For each of these types, there exists an appropriate getter function which casts the value to a correct type. In case of a mismatch,
  * an exception is thrown.
  *
  * <b>Arrays</b> are a linear type of MLNode containing other MLNodes, which allows combinations of data types which usually
  * can't be contained in a standard container. As seen above, they can easily be constructed via a simple initializer list.
  * Othe array-like behaviours are supported as well, such as iterating, indexing, appending etc.
  *
  * <b>Objects</b> are simply collections (maps) of string-MLNode pairs, where each MLNode can be accessed by its key i.e. its name.
  * There are several examples above on how to construct a simple object. Object type supports a map-like indexing access, as well as iteration.
  *
  *
  * ##### MLNode to JSON
  *
  * We provide several functions to convert our MLNode representation to JSON.
  * ```
  * void toJson(const MLNode& n, std::string& result);
  * void fromJson(const std::string& data, MLNode& n);
  * void fromJson(const char* data, MLNode& n);
  * ```
  * We are able to convert in both directions, from an MLNode to a JSON string, and vice versa (noting that the JSON string
  * can be either a standard string or a char array.
  *
  * Converting the following object
  * ```
  * MLNode n = {
        {"object", {
             {"string", "value1"},
             {"key2", 100}
        }},
        {"array", { 100, "200", false}},
        {"bool", true},
        {"int", 100},
        {"float", 100.1},
        {"null", nullptr}
    };
  * ```
  * to JSON would return the following JSON string (formatted for clarity)
  * ```
  * {
  *     "array": [100,"200",false],
  *     "bool": true,
  *     "float": 100.1,
  *     "int": 100,
  *     "null": null,
  *     "object": {
  *         "key2": 100,
  *         "string": "value1"
  *     }
  * }
  * ```
  \ingroup lvbase
*/


// MLNode::BytesType
// ----------------------------------------------------------------------------

/**
 * \class lv::MLNode::BytesType
 * \brief Internal type strongly related to MLNode, as one of the types.
 *
 * \ingroup lvbase
 */

/**
 * \brief Constructor from data
 *
 * Params are an array of unsigned chars and its size, both copied. A reference counter is included in order
 * to implement shallow copies of this object.
 */
MLNode::BytesType::BytesType(unsigned char *data, size_t size)
    : m_data(new unsigned char[size])
    , m_size(size)
    , m_ref(new int)
{
    ++(*m_ref);
    memcpy((void*)m_data, (void*)data, size);
}

/**
 * \brief Copy constructor of BytesType
 *
 * Creates only a shallow copy by copying pointers and incrementing the reference counter.
 */
MLNode::BytesType::BytesType(const MLNode::BytesType &other)
    : m_data(other.m_data)
    , m_size(other.m_size)
    , m_ref(other.m_ref)
{
    ++(*m_ref);
}

/**
 * \brief Default constructor of BytesType
 *
 * Initializes the object with zeroes.
 */
MLNode::BytesType::BytesType()
    : m_data(0)
    , m_size(0)
    , m_ref(new int)
{
    ++(*m_ref);
}

/**
 * \brief Destructor of BytesType
 *
 * We decrement the ref counter. If the ref counter is zero, we delete the pointers.
 */
MLNode::BytesType::~BytesType(){
    --(*m_ref);
    if ( *m_ref == 0 ){
        delete m_ref;
        delete[] m_data;
    }
}

/**
 * \brief Assignment operator for BytesType
 *
 * Reference counters are updated, and the pointers are copied from one to the other object.
 */
MLNode::BytesType &MLNode::BytesType::operator=(const MLNode::BytesType &other){
    if ( this != &other ){
        --(*m_ref);
        if ( *m_ref == 0 ){
            delete[] m_data;
            delete m_ref;
        }

        m_data = other.m_data;
        m_size = other.m_size;
        m_ref  = other.m_ref;
        ++(*m_ref);
    }
    return *this;
}

/**
 * @brief Equals relational operator for BytesType
 *
 * Compares the respective pointers of two objects to check if they're identical.
 * Shallow comparison only.
 */
bool MLNode::BytesType::operator ==(const MLNode::BytesType &other) const{
    return (m_data == other.m_data && m_ref == other.m_ref && m_size == other.m_size);
}

/**
 * \brief Returns a base64 string representation of the BytesType
 */
MLNode::StringType MLNode::BytesType::toBase64String(){
    return MLNode::StringType(QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_data), static_cast<int>(m_size)).toBase64().data()
    );
}

/**
 * \brief Returns a byte array base64 representation of BytesType object
 */
QByteArray MLNode::BytesType::toBase64(){
    return QByteArray::fromRawData(reinterpret_cast<const char*>(m_data), static_cast<int>(m_size)).toBase64();
}

/**
 * \brief Converts given string to BytesType
 */
MLNode::BytesType MLNode::BytesType::fromBase64(const MLNode::StringType &str){
    QByteArray result = QByteArray::fromBase64(QByteArray::fromRawData(str.c_str(), static_cast<int>(str.size())));
    return MLNode::BytesType(reinterpret_cast<MLNode::ByteType*>(result.data()), result.size());
}

/**
 * \brief Returns the actual data array
 *
 * This data array contains unsigned chars.
 */
MLNode::ByteType *MLNode::BytesType::data(){
    return m_data;
}

/**
 * \brief Returns the size of the BytesType array
 */

size_t MLNode::BytesType::size() const{
    return m_size;
}

// MLNode::Iterator
// ----------------------------------------------------------------------------

/**
 * \class lv::MLNode::Iterator
 * \brief Iterator to be used when the structure of MLNode supports iteration (i.e. arrays or objects)
 *
 * \ingroup lvbase
 */


/**
 * \brief Simple Iterator constructor with allocator traits pointer parameter
 */
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

/**
 * \brief Copy constructor of Iterator
 */
MLNode::Iterator::Iterator(const MLNode::Iterator &other)
    : m_object(other.m_object)
    , m_it(other.m_it)
{
}

/**
 * \brief Simple assignment operator, similar to a copy constructor
 */
MLNode::Iterator &MLNode::Iterator::operator=(const MLNode::Iterator &other){
    m_object = other.m_object;
    m_it     = other.m_it;
    return *this;
}

/**
 * \brief Shows if two iterators can be compared i.e. if they are used on the same MLNode
 */
bool MLNode::Iterator::canCompareTo(const MLNode::Iterator &other) const{
    return m_object == other.m_object;
}

/**
 * \brief Equals relational operator, checking if two iterators point to the same thing
 */
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

/**
 * \brief Not-equals relational operator, a simple negation of the equals operator
 */
bool MLNode::Iterator::operator!=(const MLNode::Iterator &other) const{
    return !(*this == other);
}

/**
 * \brief Less-than relational operator, can only be used with non-object iterators
 *
 * Objects iterators cannot be compared!
 */
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

/**
 * \brief Less-or-equal relational operator, uses the previously defined less operator
 */
bool MLNode::Iterator::operator<=(const MLNode::Iterator &other) const{
    return !(other < *this);
}

/**
 * \brief Greater-than relational operator, uses the previously defined less-or-equal operator
 */
bool MLNode::Iterator::operator>(const MLNode::Iterator &other) const{
    return !(*this <= (other));
}

/**
 * \brief Greater-or-equal relational operator, uses the previously defined less operator
 */
bool MLNode::Iterator::operator>=(const MLNode::Iterator &other) const{
    return !(*this < other);
}

/**
 * \brief Increment operator, moves the operator forward by one
 */
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

/**
 * \brief Decrement operator, moves the operator backwards by one
 */
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

/**
 * \brief Forward offset move operator, moves the iterator forward by the given number of steps
 *
 * Cannot be used with objects!
 */
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

/**
 * \brief Negative offset move operator, moves the operator backwards by the given number of steps
 *
 * Uses the already implemented forward offset move. Cannot be used with objects!
 */
MLNode::Iterator &MLNode::Iterator::operator-=(MLNode::Iterator::DifferenceType i){
    return operator+=(-i);
}

/**
 * \brief Offset addition operator, using the previously defined forward offset move operator
 */
MLNode::Iterator MLNode::Iterator::operator+(MLNode::Iterator::DifferenceType i){
    auto result = *this;
    result += i;
    return result;
}

/**
 * \brief Returns difference between two Array iterators.
 *
 * Can't be used with any other MLNode type!
 */
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

/**
 * @brief Dereference operator, returns the MLNode the iterator's positioned at
 */
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

/**
 * \brief Arrow operator, returns the address of the MLNode the iterator's positioned at
 */
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

/**
 * @brief Index operator, can only be used for Array MLNodes
 */
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

/**
 * \brief Returns the key of the MLNode in an Object that the iterator's currently positioned at
 */
MLNode::ObjectType::key_type MLNode::Iterator::key() const{
    assert(m_object != nullptr);
    if ( m_object->m_type == MLNode::Object )
        return m_it.objectIterator->first;

    THROW_EXCEPTION(InvalidMLTypeException, "Cannot use key with non-object iterators.", 0);
}

/**
 * @brief See the dereference operator above
 */
MLNode::Iterator::Reference MLNode::Iterator::value() const{
    return operator*();
}

/**
 * \brief Moves the iterator to the begin-iterator position
 */
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

/**
 * \brief Moves the iterator to the end-iterator position
 */
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

/**
 * \brief Offset subtraction operator, using the previously defined backwards offset move operator
 */

MLNode::Iterator MLNode::Iterator::operator-(MLNode::Iterator::DifferenceType i){
    auto result = *this;
    result -= i;
    return result;
}

// MLNode::ConstIterator
// ----------------------------------------------------------------------------

/**
 * \class lv::MLNode::ConstIterator
 * \brief Constant version of the regular MLNode iterator
 *
 * \ingroup lvbase
 */


/**
 * \brief Const version of MLNode::Iterator default constructor
 */
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

/**
 * \brief Constructor of ConstIterator from regular Iterator
 */
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

/**
 * \brief Const version of MLNode::Iterator copy constructor
 */
MLNode::ConstIterator::ConstIterator(const MLNode::ConstIterator &other)
    : m_object(other.m_object)
    , m_it(other.m_it)
{
}

/**
 * \brief Const version of MLNode::Iterator default constructor
 */
MLNode::ConstIterator &MLNode::ConstIterator::operator=(const MLNode::ConstIterator &other){
    m_object = other.m_object;
    m_it     = other.m_it;
    return *this;
}

/**
 * \brief Shows if two ConstIterators can be compared i.e. they iterate through the same object
 */
bool MLNode::ConstIterator::canCompareTo(const MLNode::ConstIterator &other) const{
    return m_object == other.m_object;
}

/**
 * \brief Equals relational operator that checks if two iterators points at the same pos
 */
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

/**
 * @brief Not-equals relational operator for ConstIterator
 */
bool MLNode::ConstIterator::operator!=(const MLNode::ConstIterator &other) const{
    return !(*this == other);
}

/**
 * \brief Const version of the MLNode::Iterator::operator<
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator<=
 */
bool MLNode::ConstIterator::operator<=(const MLNode::ConstIterator &other) const{
    return !(other < *this);
}

/**
 * \brief Const version of the MLNode::Iterator::operator>
 */
bool MLNode::ConstIterator::operator>(const MLNode::ConstIterator &other) const{
    return !(*this <= (other));
}

/**
 * \brief Const version of the MLNode::Iterator::operator>=
 */
bool MLNode::ConstIterator::operator>=(const MLNode::ConstIterator &other) const{
    return !(*this < other);
}

/**
 * \brief Const version of the MLNode::Iterator::operator++
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator--
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator+=
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator-=
 */
MLNode::ConstIterator &MLNode::ConstIterator::operator-=(MLNode::ConstIterator::DifferenceType i){
    return operator+=(-i);
}

/**
 * \brief Const version of the MLNode::Iterator::operator+
 */
MLNode::ConstIterator MLNode::ConstIterator::operator+(MLNode::ConstIterator::DifferenceType i){
    auto result = *this;
    result += i;
    return result;
}

/**
 * \brief Const version of the MLNode::Iterator::operator- for two ConstIterators
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator*
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator->
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator[]
 */
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

/**
 * \brief Const version of the key fetching function for MLNode::Iterator
 */
MLNode::ObjectType::key_type MLNode::ConstIterator::key() const{
    assert(m_object != nullptr);
    if ( m_object->m_type == MLNode::Object )
        return m_it.objectIterator->first;

    THROW_EXCEPTION(InvalidMLTypeException, "Cannot use key with non-object iterators.", 0);
}

/**
 * \brief Const version of the value fetching function for MLNode::Iterator
 */
MLNode::ConstIterator::ConstReference MLNode::ConstIterator::value() const{
    return operator*();
}

/**
 * \brief Const version of the same function for MLNode::Iterator
 */
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

/**
 * \brief Const version of the same function for MLNode::Iterator
 */
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

/**
 * \brief Const version of the MLNode::Iterator::operator- for subtracting an offset from the iterator
 */
MLNode::ConstIterator MLNode::ConstIterator::operator-(MLNode::ConstIterator::DifferenceType i){
    auto result = *this;
    result -= i;
    return result;
}

// MLNode
// ----------------------------------------------------------------------------

/** \brief The default constructor of MLNode
 *
 * Assigns the Null type and blank value.
 */
MLNode::MLNode()
    : m_type(Type::Null)
    , m_value()
{
}

/** \brief MLNode constructor with a parameter of initializer list.
 *
 * Can be used to initialize both arrays and objects, given as key-value pairs in the form of two-member arrays.
 * For example, {1, 2, 3} would initialize a simple Int array, and {{"one", 1},{"two", 2},{"three", 3}} would initialize
 * an object.
 * */
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

/**
 * \brief nullptr constructor
 *
 * Used to create a Null-type MLNode. Equivalent to the default constructor.
*/
MLNode::MLNode(std::nullptr_t)
    : m_type(Type::Null)
    , m_value()
{
}

/**
 * \brief Constructor of String MLNode given a C-type string i.e. an array of chars.
*/

MLNode::MLNode(const char *value)
    : m_type(Type::String)
    , m_value(StringType(value))
{
}

/**
 * \brief Constructor of String MLNode given a StringType-string (currently std::string).
*/

MLNode::MLNode(const MLNode::StringType &value)
    : m_type(Type::String)
    , m_value(value)
{
}

/**
 * \brief Constructor of a generic MLNode of a given type.
*/
MLNode::MLNode(MLNode::Type value)
    : m_type(value)
    , m_value(value){
}

/**
 * \brief Constructor of a Float MLNode given a float value.
*/
MLNode::MLNode(float value)
    : m_type(Type::Float)
    , m_value((double)value)
{
}

/**
 * \brief Constructor of Float MLNode given a FloatType (currently double).
*/
MLNode::MLNode(MLNode::FloatType value)
    : m_type(Type::Float)
    , m_value(value)
{
}

/**
 * \brief Constructor of Integer MLNode given an int.
 */

MLNode::MLNode(int value)
    : m_type(Type::Integer)
    , m_value((IntType)value)
{
}

/**
 * \brief Constructor of Integer MLNode given an IntType (currently long long).
 */
MLNode::MLNode(MLNode::IntType value)
    : m_type(Type::Integer)
    , m_value(value)
{
}

/**
 * \brief Constructor of Boolean MLNode given a BoolType (i.e. the one and only bool).
 */
MLNode::MLNode(MLNode::BoolType value)
    : m_type(Type::Boolean)
    , m_value(value)
{
}

/**
 * \brief Contructor of Bytes MLNode given a BytesType value.
 */

MLNode::MLNode(const MLNode::BytesType &value)
   : m_type(Type::Bytes)
   , m_value(value)
{
}

/**
 * \brief Constructor of Bytes MLNode given an array of unsigned chars and the number of them.
 */
MLNode::MLNode(lv::MLNode::ByteType *value, size_t size)
    : m_type(Type::Bytes)
    , m_value(MLValue(value, size))
{
}

/**
 * \brief Constructor of Array MLNode given a vector of MLNodes.
 */
MLNode::MLNode(const MLNode::ArrayType &value)
    : m_type(Type::Array)
    , m_value(value)
{
}

/**
 * \brief Constructor of Object MLNode given a map of string-MLNode pairs.
 *
 * The strings represent the keys, while the other pair component is any type of MLNode.
 */
MLNode::MLNode(const MLNode::ObjectType &value)
    : m_type(Type::Object)
    , m_value(value)
{
}

/**
 * \brief Copy constructor of the MLNode type.
 */
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

/**
 * \brief Move constructor of the MLNode type.
 */
MLNode::MLNode(MLNode &&other)
    : m_type(other.m_type)
    , m_value(other.m_value)
{
    other.m_type  = MLNode::Type::Null;
    other.m_value = {};
}

/**
 * \brief Destructor of MLNode type.
 *
 * Depending on the underlying type, we invoke the destructor on the appropriate pointer.
 */
MLNode::~MLNode(){
    switch(m_type){
    case Type::Object: delete m_value.asObject; break;
    case Type::Array:  delete m_value.asArray; break;
    case Type::Bytes:  delete m_value.asBytes; break;
    case Type::String: delete m_value.asString; break;
    default: break;
    }
}

/**
 * \brief Index operator of Array MLNode.
 *
 * The underlying type has to be an array, otherwise an exception is thrown.
 */
MLNode &MLNode::operator[](int index){
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    return (*m_value.asArray)[index];
}

/**
 * @brief Appends to an Array MLNode.
 *
 * In case of using it on a non-Array MLNode, an exception is thrown.
 */
void MLNode::append(const MLNode &value){
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    m_value.asArray->push_back(value);
}

/**
 * \brief Indicates if the MLNode is of Null type.
 */
bool MLNode::isNull() const{
    if ( m_type == Type::Null )
        return true;
    return false;
}

/**
 * \brief Index operator of Array MLNode that returns a const reference.
 *
 * Difference from the other index operator is that this returns an immutable reference.
 */
const MLNode &MLNode::operator[](int index) const{
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    return (*m_value.asArray)[index];
}

/**
 * \brief Index operator of Object MLNode, uses a key as reference.
 *
 * If used on a non-Object type, throws an exception.
 */
MLNode &MLNode::operator[](const MLNode::StringType &reference){
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    return (*m_value.asObject)[reference];
}

/**
 * \brief Index operator of Object MLNode that returns a const reference.
 *
 * Throws an exception in case of non-Object type, and unlike a similar index operator, returns an immutable reference.
 */

const MLNode &MLNode::operator[](const MLNode::StringType &reference) const{
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    return (*m_value.asObject)[reference];
}

/**
 * \brief Returns a string representation of a given MLNode.
 */
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

/**
 * @brief Returns a string representation of each MLNode type.
 */
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

/**
 * \brief Returns a string representation of an MLNode, indented with a given number of blank spaces.
 *
 * This uses the previously defined toStringImpl(), which prints out
 * the given MLNode to the given stream, whilepassing a stringstream.
 */
std::string MLNode::toString(int indent, int indentStep) const{
    std::stringstream stream;
    toStringImpl(stream, indent, indentStep);
    return stream.str();
}

/**
 * \brief Returns the appropriate begin-iterator depending on the underlying MLNode type.
 *
 * Types that allow iteration are Object and Array only.
 */
MLNode::Iterator MLNode::begin(){
    Iterator it(this);
    it.positionAtBegin();
    return it;
}

/**
 * \brief Returns the appropriate end-iterator depending on the underlying MLNode type.
 *
 * Types that allow iteration are Object and Array only.
 */
MLNode::Iterator MLNode::end(){
    Iterator it(this);
    it.positionAtEnd();
    return it;
}

/**
 * @brief See cbegin()
 */
MLNode::ConstIterator MLNode::begin() const{
    return cbegin();
}

/**
 * @brief See cend()
 */
MLNode::ConstIterator MLNode::end() const{
    return cend();
}

/**
 * \brief Returns the appropriate <b>const</b> begin-iterator depending on the underlying MLNode type.
 *
 * Types that allow iteration are Object and Array only.
 */
MLNode::ConstIterator MLNode::cbegin() const{
    ConstIterator it(this);
    it.positionAtBegin();
    return it;
}

/**
 * \brief Returns the appropriate <b>const</b> end-iterator depending on the underlying MLNode type.
 *
 * Types that allow iteration are Object and Array only.
 */
MLNode::ConstIterator MLNode::cend() const{
    ConstIterator it(this);
    it.positionAtEnd();
    return it;
}
/**
 * \brief Returns the MLNode value as int.
 *
 * If not the appropriate type, an exception is thrown.
 */
int MLNode::asInt() const{
    if ( m_type != Type::Integer )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of integer type.", 0);

    return static_cast<int>(m_value.asInt);
}

/**
 * \brief Returns the MLNode value as bool.
 *
 * Throws exception if node is not of bool type.
 */
bool MLNode::asBool() const{
    if ( m_type != Type::Boolean )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of boolean type.", 0);

    return m_value.asBool;
}

/**
 * \brief Returns the MLNode value as float.
 *
 * Throws exception if node is not of float type.
 */
MLNode::FloatType MLNode::asFloat() const{
    if ( m_type != Type::Float )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of float type.", 0);

    return m_value.asFloat;
}

/**
 * \brief Returns the MLNode value as string.
 *
 * Throws exception if node is not of string type.
 */
const MLNode::StringType &MLNode::asString() const{
    if ( m_type != Type::String )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of string type.", 0);

    return *m_value.asString;
}

/**
 * \brief Returns the MLNode value as bytes.
 *
 * This can be done with both byte and string types. In case of the string type,
 * it's converted to base 64 and then assigned to bytes. If not Bytes or String type,
 * an exception is thrown.
 */
MLNode::BytesType MLNode::asBytes() const{
    if ( m_type == Type::Bytes ){
        return *m_value.asBytes;
    } else if ( m_type == Type::String ){
        return MLNode::BytesType::fromBase64(*m_value.asString);
    } else
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of bytes type.", 0);
}

/**
 * \brief Returns the MLNode value as an array.
 *
 * The array is actually a vector of MLNodes. If not the appropriate type, an exception is thrown.
 */
const MLNode::ArrayType &MLNode::asArray() const{
    if ( m_type != Type::Array )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of array type.", 0);

    return *m_value.asArray;
}

/**
 * \brief Returns the MLNode value as an object.
 *
 * If not the appropriate type, an exception is thrown.
 */
const MLNode::ObjectType &MLNode::asObject() const{
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    return *m_value.asObject;
}

/**
 * \brief Returns the size of MLNode, if it's an Object or Array.
 *
 * When an Array, the appropriate vector size is returned.
 * When an Object, the number of mapped pairs is returned.
 * If not one of those types, zero is returned.
 */
int MLNode::size() const{
    if ( m_type == Type::Array ){
        return static_cast<int>(m_value.asArray->size());
    } else if ( m_type == Type::Object ){
        return static_cast<int>(m_value.asObject->size());
    } else {
        return 0;
    }
}

/**
 * \brief Returns an indicator that the given key is found in our Object MLNode.
 *
 * If the node is not an Object, an exception is thrown.
 */
bool MLNode::hasKey(const MLNode::StringType &key) const{
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    auto it = m_value.asObject->find(key);
    return it != m_value.asObject->end();
}


/**
 * \brief Removes a key-value pair from an Object MLNode with the given key.
 *
 * If not a Object, an exception is thrown.
 */
void MLNode::remove(const MLNode::StringType &key){
    if ( m_type != Type::Object )
        THROW_EXCEPTION(InvalidMLTypeException, "Node is not of object type.", 0);

    m_value.asObject->erase(key);
}


VisualLog &operator <<(VisualLog &vl, const MLNode &value){
    return vl << value.toString().c_str();
}

}

