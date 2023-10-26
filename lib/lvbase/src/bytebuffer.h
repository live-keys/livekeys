#ifndef LVBYTEBUFFER_H
#define LVBYTEBUFFER_H

#include "live/lvbaseglobal.h"
#include <string>
#include <memory>

namespace lv{

class ByteBufferData;
class LV_BASE_EXPORT ByteBuffer{

public:
    typedef char Byte;

public:
    ByteBuffer();
    ByteBuffer(Byte* data, size_t size);
    ByteBuffer(const Byte* data, size_t size);
    ByteBuffer(const ByteBuffer& other);
    ~ByteBuffer();

    ByteBuffer& operator=(const ByteBuffer& other);
    bool operator == (const ByteBuffer& other) const;

    static ByteBuffer encodeBase64(const ByteBuffer& bf, bool nullTerminate = false);
    static ByteBuffer encodeBase64(const ByteBuffer::Byte* bytes, size_t size, bool nullTerminate = false);

    static ByteBuffer decodeBase64(const ByteBuffer& bf, bool nullTerminate = false);
    static ByteBuffer decodeBase64(const Byte *bytes, size_t size, bool nullTerminate = false);

    ByteBuffer::Byte* data() const;
    size_t size() const;

private:
    std::shared_ptr<ByteBufferData>* m_data;
};

} // namespace

#endif // LVBYTEBUFFER_H
