/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef LVMLNODE_H
#define LVMLNODE_H

#include "live/exception.h"

#include <QMap>
#include <sstream>
#include <initializer_list>

namespace lv{

class VisualLog;

class LV_BASE_EXPORT MLOutOfRanceException: public lv::Exception{
public:
    MLOutOfRanceException(const QString& message = "", int code = 0) : lv::Exception(message, code){}
};

class LV_BASE_EXPORT InvalidMLTypeException: public lv::Exception{
public:
    InvalidMLTypeException(const QString& message = "", int code = 0) : lv::Exception(message, code){}
};

class LV_BASE_EXPORT TypeNotSerializableException: public lv::Exception{
public:
    TypeNotSerializableException(const QString& message = "", int code = 0) : lv::Exception(message, code){}
};

// MLNode
// ------

class LV_BASE_EXPORT MLNode{

public:
    typedef MLNode ValueType;
    typedef MLNode& Reference;
    typedef const MLNode& ConstReference;
    typedef std::allocator<MLNode> AllocatorType;
    typedef std::ptrdiff_t DifferenceType;
    typedef std::size_t SizeType;
    typedef std::allocator_traits<AllocatorType>::pointer Pointer;
    typedef std::allocator_traits<AllocatorType>::const_pointer ConstPointer;

    union MLValue;

    typedef long long                IntType;
    typedef double                   FloatType;
    typedef bool                     BoolType;
    typedef std::string              StringType;
    typedef QList<MLNode>            ArrayType;
    typedef QMap<StringType, MLNode> ObjectType;
    typedef unsigned char            ByteType;

    // MLNode::BytesType
    // -----------------

    class LV_BASE_EXPORT BytesType{

    public:
        BytesType(unsigned char* data, size_t size);
        BytesType(const BytesType& other);
        BytesType();
        ~BytesType();

        BytesType& operator=(const BytesType& other);
        bool operator == (const BytesType& other) const;

        QByteArray toBase64();
        static BytesType fromBase64(const StringType& str);

        ByteType* data();
        size_t size() const;

    private:
        ByteType* m_data;
        size_t    m_size;
        int*      m_ref;
    };

    // MLNode::IteratorValue
    // ---------------------

    class LV_BASE_EXPORT IteratorValue{
    public:
        ObjectType::iterator objectIterator;
        ArrayType::iterator  arrayIterator;
        bool                 primitiveIterator;
    };

    // MLNode::Iterator
    // ----------------

    class LV_BASE_EXPORT Iterator{

    public:
        friend class MLNode;

    public:
        typedef MLNode::DifferenceType DifferenceType;
        typedef MLNode::ValueType      ValueType;
        typedef MLNode::Reference      Reference;
        typedef MLNode::Pointer        Pointer;
        typedef MLNode::ConstPointer   ConstPointer;
        typedef std::random_access_iterator_tag IteratorCategory;

    public:
        Iterator(Pointer object);
        Iterator(const Iterator& other);
        ~Iterator(){}
        Iterator& operator=(const Iterator& other);

        bool canCompareTo(const Iterator& other) const;
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        bool operator < (const Iterator& other) const;
        bool operator<=(const Iterator& other) const;
        bool operator>(const Iterator& other) const;
        bool operator>=(const Iterator& other) const;

        Iterator& operator++();
        Iterator& operator--();
        Iterator& operator+=(DifferenceType i);
        Iterator& operator-=(DifferenceType i);

        Iterator operator+(DifferenceType i);
        Iterator operator-(DifferenceType i);
        DifferenceType operator-(const Iterator& other) const;
        Reference operator*() const;
        Pointer operator->() const;

        Reference operator[](DifferenceType i) const;

        ObjectType::key_type key() const;

        Reference value() const;

    private:
        void positionAtBegin();
        void positionAtEnd();

    private:
        Pointer       m_object;
        IteratorValue m_it;
    };
    friend class Iterator;


    // MLNode::ConstIteratorValue
    // --------------------------

    class LV_BASE_EXPORT ConstIteratorValue{
    public:
        ObjectType::const_iterator objectIterator;
        ArrayType::const_iterator  arrayIterator;
        bool                       primitiveIterator;
    };

    // MLNode::ConstIterator
    // ---------------------

    class LV_BASE_EXPORT ConstIterator{
    public:
        friend class MLNode;

    public:
        typedef MLNode::DifferenceType DifferenceType;
        typedef MLNode::ValueType      ValueType;
        typedef MLNode::Reference      Reference;
        typedef MLNode::ConstReference ConstReference;
        typedef MLNode::Pointer        Pointer;
        typedef MLNode::ConstPointer   ConstPointer;
        typedef std::random_access_iterator_tag IteratorCategory;

    public:
        ConstIterator(ConstPointer object);
        ConstIterator(const Iterator& other);
        ConstIterator(const ConstIterator& other);
        ~ConstIterator(){}
        ConstIterator& operator=(const ConstIterator& other);

        bool canCompareTo(const ConstIterator& other) const;
        bool operator==(const ConstIterator& other) const;
        bool operator!=(const ConstIterator& other) const;
        bool operator < (const ConstIterator& other) const;
        bool operator<=(const ConstIterator& other) const;
        bool operator>(const ConstIterator& other) const;
        bool operator>=(const ConstIterator& other) const;

        ConstIterator& operator++();
        ConstIterator& operator--();
        ConstIterator& operator+=(DifferenceType i);
        ConstIterator& operator-=(DifferenceType i);

        ConstIterator operator+(DifferenceType i);
        ConstIterator operator-(DifferenceType i);
        DifferenceType operator-(const ConstIterator& other) const;
        ConstReference operator*() const;
        ConstPointer operator->() const;

        ConstReference operator[](DifferenceType i) const;

        ObjectType::key_type key() const;

        ConstReference value() const;

    private:
        void positionAtBegin();
        void positionAtEnd();

    private:
        ConstPointer       m_object;
        ConstIteratorValue m_it;
    };

    friend class ConstIterator;

public:
    enum Type{
        Null = 0,
        Object,
        Array,
        Bytes,
        String,
        Boolean,
        Integer,
        Float
    };

public:
    union MLValue{
        ObjectType*  asObject;
        ArrayType*   asArray;
        BytesType*   asBytes;
        StringType*  asString;
        BoolType     asBool;
        IntType      asInt;
        FloatType    asFloat;

        MLValue() = default;
        MLValue(const ObjectType& object) : asObject(new ObjectType(object)){}
        MLValue(const ArrayType& array) : asArray(new ArrayType(array)){}
        MLValue(const BytesType& bytes) : asBytes(new BytesType(bytes)){}
        MLValue(MLNode::ByteType* bytes, size_t size) : asBytes(new BytesType(bytes, size)){}
        MLValue(const StringType& str) : asString(new StringType(str)){}
        MLValue(BoolType boolVal) : asBool(boolVal){}
        MLValue(IntType intVal) : asInt(intVal){}
        MLValue(FloatType floatVal) : asFloat(floatVal){}
        MLValue(Type t);
    };

public:
    MLNode();
    MLNode(const std::initializer_list<MLNode>& init);
    MLNode(std::nullptr_t);
    MLNode(const char* value);
    MLNode(const StringType& value);
    MLNode(MLNode::Type value);
    MLNode(int value);
    MLNode(IntType value);
    MLNode(float value);
    MLNode(FloatType value);
    MLNode(BoolType value);
    MLNode(const BytesType& value);
    MLNode(MLNode::ByteType* value, size_t size);
    MLNode(const ArrayType& value);
    MLNode(const ObjectType& value);
    MLNode(const MLNode& other);
    MLNode(MLNode&& other);
    ~MLNode();

    const MLNode& operator[](const StringType& reference) const;
    MLNode& operator[](const StringType& reference);
    const MLNode& operator[](int index) const;
    MLNode& operator[](int index);

    MLNode& operator=(MLNode other);

    void append(const MLNode& value);

    Type type() const;

    bool isNull() const;
    int asInt() const;
    bool asBool() const;
    FloatType asFloat() const;
    StringType asString() const;
    BytesType asBytes() const;

    const ArrayType& asArray() const;

    int size() const;
    bool hasKey(const StringType& key) const;
    void remove(const StringType& key);

    std::string typeString() const;
    std::string toString(int indent = -1, int indentStep = 4) const;

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    ConstIterator cbegin() const;
    ConstIterator cend() const;

private:
    void toStringImpl(std::ostream& o, int indent = -1, int indentStep = 4) const;

    Type    m_type;
    MLValue m_value;
};


inline MLNode::MLValue::MLValue(MLNode::Type t){
    switch(t){
    case Type::Null: break;
    case Type::Object:  asObject = new ObjectType(); break;
    case Type::Array:   asArray = new ArrayType(); break;
    case Type::Bytes:   asBytes = new BytesType(); break;
    case Type::String:  asString = new StringType(); break;
    case Type::Boolean: asBool = false;
    case Type::Integer: asInt = 0;
    case Type::Float:   asFloat = 0;
    }
}

inline MLNode &MLNode::operator=(MLNode other){
    std::swap(m_type, other.m_type);
    std::swap(m_value, other.m_value);

    return *this;
}

inline MLNode::Type MLNode::type() const{
    return m_type;
}

LV_BASE_EXPORT VisualLog &operator <<(VisualLog &vl, const MLNode &value);

namespace ml{

template<typename T> void serialize(MLNode&, const T&){
    throw TypeNotSerializableException();
}

template<typename T> void deserialize(const MLNode&, T&){
    throw TypeNotSerializableException();
}

}// namespace ml

}// namespace lv

#endif // LVMLNODE_H
