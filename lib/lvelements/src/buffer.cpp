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
    : m_data(value->GetContents().Data())
    , m_size(value->ByteLength())
    , m_externalized(value->IsExternal())
{
}

}} // namespace lv, el
