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
class LocalValue;
class Callable;

class ObjectPrivate;

/**
 * @brief Stores a js object reference
 *
 * The js object will be owned by this object, which will be recovered
 * by the js garbage collector once there are no more instances of this object.
 */
class LV_ELEMENTS_EXPORT Object{

    friend class LocalValue;
    friend class LocalObject;
    friend class Engine;

public:
    Object(Engine* engine);
    Object(const Object& other);
    Object(Engine *engine, const v8::Local<v8::Object>& value); // Construct from engine value
    virtual ~Object();

    Object& operator = (const Object& other);
    bool operator == (const Object& other);

    //TODO: Add get set methods

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
    std::string toString() const;
    Buffer toBuffer() const;

protected:
    v8::Local<v8::Object> data() const;
    static Object create(ObjectPrivate* d, int* ref);
    Object(ObjectPrivate* d, int* ref);

    ObjectPrivate* m_d;
    int*           m_ref;
};


class LocalObjectPrivate;

/**
 * @brief A scoped object used for accessing object properties
 */
class LV_ELEMENTS_EXPORT LocalObject{

public:
    LocalObject(Object o);
    LocalObject(Context* context);
    ~LocalObject();

    LocalValue get(int index);
    LocalValue get(const LocalValue& key);
    LocalValue get(Engine* engine, const std::string& str);
    void set(int index, const LocalValue& value);
    void set(const LocalValue& key, const LocalValue& value);
    void set(Engine* engine, const std::string& key, const LocalValue& value);

private:
    DISABLE_COPY(LocalObject);
    LocalObject(const v8::Local<v8::Object>& vo);

    LocalObjectPrivate* m_d;
};


}}// namespace lv, el


#endif // LVOBJECT_H
