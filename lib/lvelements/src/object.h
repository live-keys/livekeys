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
        ~Accessor();

        ScopedValue get(int index);
        ScopedValue get(const ScopedValue& key);
        ScopedValue get(Engine* engine, const std::string& str);
        void set(int index, const ScopedValue& value);
        void set(const ScopedValue& key, const ScopedValue& value);
        void set(Engine* engine, const std::string& key, const ScopedValue& value);

        bool has(Engine* engine, const ScopedValue& key) const;

        ScopedValue ownProperties() const;

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

        ScopedValue get(int index);
        void set(int index, const ScopedValue& value);

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
    std::string toString() const;
    Buffer toBuffer() const;

    v8::Local<v8::Object> data() const;

protected:
    static Object create(ObjectPrivate* d, int* ref);
    Object(ObjectPrivate* d, int* ref);

    ObjectPrivate* m_d;
    int*           m_ref;
};


}}// namespace lv, el


#endif // LVOBJECT_H
