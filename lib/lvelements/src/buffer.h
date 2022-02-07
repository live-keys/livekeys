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

namespace v8{
class ArrayBuffer;
template<class T> class Local;
}

namespace lv{ namespace el{

class Engine;

/**
 * @brief The Buffer class
 *
 * The Buffer class handles transfer of raw data between c++ and js. The equivalent
 * in js is the ```ArrayBuffer```. It's important to note that the data is not owned
 * by the buffer, nor by js. It's meant for purely transfer purposes, i.e. function
 * calls and properties. If you want to keep raw data persistance, use an element subclass,
 * and a Buffer property.
 *
 * @code
 * class BufferHolder : public Element{
 *      ...
 *      .scriptProperty<Buffer>("data", ...)
 *      ...
 * };
 * @endcode
 *
 * Then in js, you can hold a reference to BufferHolder while accesing it's raw data.
 */
class LV_ELEMENTS_EXPORT Buffer{

public:
    Buffer(void* data, size_t size);
    Buffer(const v8::Local<v8::ArrayBuffer>& value);
    ~Buffer(){}

    void* data() const{ return m_data; }
    template<typename T> T dataAs(){ return reinterpret_cast<T>(m_data); }
    size_t size() const{ return m_size; }
    bool isExternal() const{ return m_externalized; }

private:
    void*  m_data;
    size_t m_size;
    bool   m_externalized;
};

}} // namespace lv, el

#endif // BUFFER_H
