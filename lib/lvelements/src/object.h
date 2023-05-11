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

#ifndef LVOBJECT_H
#define LVOBJECT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/buffer.h"
#include <string>

namespace v8{

class Object;
template<class T> class Local;

}// namespace

namespace lv{ namespace el{

class Element;
class Engine;
class Context;
class ScopedValue;
class Callable;

class ObjectPrivate;

/**
 * @brief Stores a js object reference
 *
 * The js object will be owned by this object, which will be recovered
 * by the js garbage collector once there are no more instances of this object.
 */
class LV_ELEMENTS_EXPORT Object{

    friend class ScopedValue;
    friend class Accessor;
    friend class ArrayAccessor;
    friend class Engine;

public:
    class AccessorPrivate;

    class LV_ELEMENTS_EXPORT Accessor{
    public:
        Accessor(Object o);
        Accessor(Context* context);
        Accessor(const ScopedValue& sv);
        ~Accessor();

        ScopedValue get(Engine* engine, int index);
        ScopedValue get(Engine* engine, const ScopedValue& key);
        ScopedValue get(Engine* engine, const std::string& str);
        void set(Engine* engine, int index, const ScopedValue& value);
        void set(Engine* engine, const ScopedValue& key, const ScopedValue& value);
        void set(Engine* engine, const std::string& key, const ScopedValue& value);

        bool has(Engine* engine, const ScopedValue& key) const;
        bool has(Engine* engine, const std::string& key) const;

        ScopedValue ownProperties(Engine *engine) const;

    private:
        DISABLE_COPY(Accessor);
        Accessor(const v8::Local<v8::Object>& vo);

        AccessorPrivate* m_d;
    };

    class ArrayAccessorPrivate;

    class LV_ELEMENTS_EXPORT ArrayAccessor{
    public:
        ArrayAccessor(Object o);
        ArrayAccessor(Engine* engine, const ScopedValue& lval);
        ~ArrayAccessor();

        int length() const;

        ScopedValue get(Engine *engine, int index);
        void set(Engine* engine, int index, const ScopedValue& value);

    private:
        DISABLE_COPY(ArrayAccessor);

        ArrayAccessorPrivate* m_d;
    };

public:
    Object(Engine* engine);
    Object(const Object& other);
    Object(Engine *engine, const v8::Local<v8::Object>& value); // Construct from engine value
    virtual ~Object();

    Object& operator = (const Object& other);
    bool operator == (const Object& other);

    bool isNull() const;
    bool isString() const;

    bool isPoint() const;
    void toPoint(double &x, double &y) const;
    bool isSize() const;
    void toSize(double &width, double &height) const;
    bool isRectangle() const;
    void toRectangle(double& x, double& y, double& width, double& height);
    bool isDate() const;
    double toDate() const;

    static Object create(Engine* engine);
    static Object createArray(Engine* engine, int length = 0);
    static Object createBuffer(Engine* engine, const Buffer::Initializer& bufferInitializer);
    std::string toString() const;

    v8::Local<v8::Object> data() const;

protected:
    static Object create(ObjectPrivate* d, int* ref);
    Object(ObjectPrivate* d, int* ref);

    ObjectPrivate* m_d;
    int*           m_ref;
};


}}// namespace lv, el


#endif // LVOBJECT_H
