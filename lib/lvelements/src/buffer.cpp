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

#include "buffer.h"
#include "live/elements/engine.h"
#include "live/elements/object.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

class Buffer::AccessorPrivate{
public:
    AccessorPrivate(const v8::Local<v8::ArrayBuffer>& ab)
        : data(ab)
    {
        backingStore = data->GetBackingStore();
    }

    v8::Local<v8::ArrayBuffer> data;
    std::shared_ptr<v8::BackingStore> backingStore;
};

Buffer::Accessor::Accessor(const Object &o)
    : m_d(nullptr)
{
    m_d = new Buffer::AccessorPrivate(v8::Local<v8::ArrayBuffer>::Cast(o.data()));
}

Buffer::Accessor::Accessor(const ScopedValue& sv)
    : m_d(nullptr)
{
    m_d = new Buffer::AccessorPrivate(sv.data().As<v8::ArrayBuffer>());
}

Buffer::Accessor::~Accessor(){
    delete m_d;
}

size_t Buffer::Accessor::size() const{
    return m_d->backingStore->ByteLength();
}

void *Buffer::Accessor::data() const{
    return m_d->backingStore->Data();
}

Buffer::Initializer::Initializer(void *data, size_t size, std::function<void (void *, size_t)> deleter)
    : m_used(false)
    , m_data(data)
    , m_size(size)
    , m_deleter(deleter)
{
}

void Buffer::Initializer::use() const{
    if ( m_used ){
        THROW_EXCEPTION(lv::Exception, "Buffer::Initializer has already been used for initialisation.", lv::Exception::toCode("~Initializer"));
    }
    m_used = true;
}

}} // namespace lv, el
