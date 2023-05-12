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

#ifndef LVVALUE_H
#define LVVALUE_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/object.h"

#include <string>
#include <memory>
#include <initializer_list>

namespace v8{

class Isolate;
class Integer;
class Name;
class String;
class Symbol;
class Object;
class Number;
class Function;
class FunctionTemplate;
class Value;
class Boolean;
class Context;
class Script;
class Module;
class TryCatch;
template<class T> class Local;
template<class T> class MaybeLocal;
template<class T> class FunctionCallbackInfo;
template<class T> class PropertyCallbackInfo;

} // namespace


namespace lv{ namespace el{

class Element;
class Engine;
class Context;
class ScopedValue;
class Component;
class Function;
class Callable;

/**
 * @brief Wraps a script value
 */
class LV_ELEMENTS_ENGINE_EXPORT Value{

    friend class Engine;

public:
    class Stored{
    public:
        enum Type{
            Boolean,
            Integer,
            Double,
            Object,
            Callable,
            Element
        };
    };

    typedef int       Int32;
    typedef long long Int64;
    typedef double    Number;

public:
    union LV_ELEMENTS_ENGINE_EXPORT Data{
        Int64     asInteger;
        Number    asNumber;
        Object*   asObject;
        Callable* asCallable;
        Element*  asElement;

        Data() = default;
        Data(Int64 value) : asInteger(value){}
        Data(Number value) : asNumber(value){}
        Data(Object value) : asObject(new Object(value)){}
        Data(Callable value);
        Data(Element* value) : asElement(value){}
    };

    Value();
    Value(bool v);
    Value(Int32 v);
    Value(Int64 v);
    Value(Number v);
    Value(Object v);
    Value(Callable v);
    Value(Element* el);
    Value(const Value& other);
    ~Value();

    Stored::Type type() const;

    bool operator == (const Value& other);
    Value& operator = (const Value& other);

    bool isNull() const;
    bool asBool() const;
    Int32 asInt32() const;
    Int64 asInt64() const;
    Number asNumber() const;
    Object asObject() const;
    Callable asCallable() const;
    Element* asElement() const;

private:
    Data         m_data;
    Stored::Type m_type;

};

inline Value::Stored::Type Value::type() const{
    return m_type;
}


class ScopedValuePrivate;

/**
 * @brief Stores a reference to a script value.
 */
class LV_ELEMENTS_ENGINE_EXPORT ScopedValue{

    friend class Callable;
    friend class Script;
    friend class Object::Accessor;

public:
    ScopedValue(Engine* engine);
    ScopedValue(Engine* engine, bool val);
    ScopedValue(Engine* engine, Value::Int32 val);
    ScopedValue(Engine* engine, Value::Int64 val);
    ScopedValue(Engine* engine, Value::Number val);
    ScopedValue(Engine* engine, const std::string& val);
    ScopedValue(Engine* engine, Callable val);
    ScopedValue(Engine* engine, Object val);
    ScopedValue(Engine* engine, const Buffer::Initializer &val);
    ScopedValue(Engine* engine, Element *val);
    ScopedValue(Engine* engine, const Value& value);
    ScopedValue(Engine* engine, const ScopedValue& other); // TODO: This can be eliminated through specialization
    ScopedValue(const ScopedValue& other);
    ScopedValue(const v8::Local<v8::Value>& data);
    ~ScopedValue();

    ScopedValue& operator = (const ScopedValue& other);

    bool operator == (const ScopedValue& other);

    const v8::Local<v8::Value>& data() const;

    bool toBool(Engine* engine) const;
    Value::Int32 toInt32(Engine* engine) const;
    Value::Int64 toInt64(Engine* engine) const;
    Value::Number toNumber(Engine* engine) const;
    std::string toStdString(Engine* engine) const;
    Callable toCallable(Engine* engine) const;
    Object toObject(Engine* engine) const;
    Element* toElement(Engine* engine) const;

    Value toValue(Engine* engine) const;

    bool isNull() const;
    bool isBool() const;
    bool isInt() const;
    bool isNumber() const;
    bool isString() const;
    bool isCallable() const;
    bool isBuffer() const;
    bool isObject() const;
    bool isArray() const;
    bool isElement() const;



    template <typename T> static ScopedValue createValue(Engine* engine, const T& val){
        return ScopedValue(engine, val);
    }

private:
    ScopedValuePrivate* m_d;
    int*               m_ref;
};

// Type Conversions
// ----------------

template <typename T> struct ScriptToV8{     using type = T; };
template <typename T> struct ScriptToV8<T*>{ using type = v8::Object; };
template<> struct ScriptToV8<bool>{          using type = v8::Boolean; };
template<> struct ScriptToV8<int>{           using type = v8::Integer; };
template<> struct ScriptToV8<long long>{     using type = v8::Integer; };
template<> struct ScriptToV8<double>{        using type = v8::Number; };
template<> struct ScriptToV8<ScopedValue>{    using type = v8::Value; };

// Conversion Functions
// --------------------

template<typename T, typename R = ScriptToV8<T> > v8::Local<R> convertToV8(Engine* engine, const T& value){
    convertToV8<Element*>(engine, value);
}

template<> LV_ELEMENTS_ENGINE_EXPORT v8::Local<v8::Integer> convertToV8<int>(Engine* engine, const int& value);
template<> LV_ELEMENTS_ENGINE_EXPORT v8::Local<v8::Integer> convertToV8<long long>(Engine* engine, const long long& value);
template<> LV_ELEMENTS_ENGINE_EXPORT v8::Local<v8::Number> convertToV8<double>(Engine* engine, const double& value);
template<> LV_ELEMENTS_ENGINE_EXPORT v8::Local<v8::Object> convertToV8<Element*>(Engine* engine, Element* const& value);
template<> LV_ELEMENTS_ENGINE_EXPORT v8::Local<v8::Value> convertToV8(Engine* engine, const ScopedValue& value);

template<typename T> T convertFromV8(Engine* engine, const v8::Local<v8::Value>& value){
    return static_cast<T>(convertFromV8<Element*>(engine, value));
}

template<> LV_ELEMENTS_ENGINE_EXPORT bool convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT Value::Int32 convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT Value::Int64 convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT Value::Number convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT std::string convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT Callable convertFromV8(Engine *engine, const v8::Local<v8::Value> &value);
template<> LV_ELEMENTS_ENGINE_EXPORT Object convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT Value convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT ScopedValue convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);
template<> LV_ELEMENTS_ENGINE_EXPORT Element* convertFromV8(Engine* engine, const v8::Local<v8::Value>& value);

}} // namespace lv, el

#endif // LVVALUE_H
