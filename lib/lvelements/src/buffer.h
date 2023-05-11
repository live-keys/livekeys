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

#ifndef LVBUFFER_H
#define LVBUFFER_H

#include "live/elements/lvelementsglobal.h"
#include <memory>
#include <functional>

namespace v8{
class ArrayBuffer;
template<class T> class Local;
}

namespace lv{ namespace el{

class Engine;
class Object;
class ScopedValue;

/**
 * @brief The Buffer class
 *
 * The Buffer class handles transfer of raw data between c++ and js. The equivalent
 * in js is the ```ArrayBuffer```. It's important to note that the data is not owned
 * by the buffer, nor by js. It's meant for purely transfer purposes, i.e. function
 * calls and properties. If you want to keep raw data persistance, use an Object to
 * store the Buffer.
 *
 * @code
 * auto obj = Object::createBuffer(Buffer::Initializer(data, size, deleter);
 * @endcode
 *
 * When accessing an ArrayBuffer, use the Buffer::Accessor:
 *
 * @code
 * Object bufferOb;
 * Buffer::Accessor bufferAccess(bufferObject);
 * void* data = bufferAccess.data();
 * size_t size = bufferAccess.size();
 * @endcode
 */
class LV_ELEMENTS_EXPORT Buffer{

public:
    class AccessorPrivate;

    class LV_ELEMENTS_EXPORT Accessor{
    public:
        Accessor(const Object& o);
        Accessor(const ScopedValue& sv);
        ~Accessor();

        size_t size() const;
        void* data() const;

    private:
        DISABLE_COPY(Accessor);
        Accessor(const v8::Local<v8::ArrayBuffer>& value);

        AccessorPrivate* m_d;
    };

    class DeleterData{
    public:
        std::function<void(void*, size_t)> callback;
    };

    class LV_ELEMENTS_EXPORT Initializer{
    public:
        Initializer(void* data, size_t size, std::function<void (void *, size_t)> deleter);

        size_t size() const{ return m_size; }
        void* data() const{ return m_data; }
        std::function<void(void*, size_t)> deleter() const{ return m_deleter; }
        void use() const;

    private:
        mutable bool m_used;
        void*  m_data;
        size_t m_size;
        std::function<void(void*,size_t)> m_deleter;
    };
};

}} // namespace lv, el

#endif // BUFFER_H
