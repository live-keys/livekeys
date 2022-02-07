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
#include "v8nowarnings.h"

namespace lv{ namespace el{

Buffer::Buffer(void *data, size_t size)
    : m_data(data)
    , m_size(size)
    , m_externalized(true)
{
}

Buffer::Buffer(const v8::Local<v8::ArrayBuffer> &value)
    : m_data(/*value->GetContents().Data()*/) //HERE: Will need to use backing stores
    , m_size(value->ByteLength())
    , m_externalized(value->IsExternal())
{
}

}} // namespace lv, el
