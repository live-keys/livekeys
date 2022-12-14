#include "bytebuffer.h"
#include "live/visuallog.h"

#include "string.h"

extern "C"{
#include "b64/cencode.h"
#include "b64/cdecode.h"
}

namespace lv{

// ByteBufferData
// ----------------------------------------------------------------------------

class ByteBufferData{
public:
    ByteBufferData(){}
    ~ByteBufferData(){ delete[] buffer; }

    ByteBuffer::Byte* buffer;
    size_t            size;
};


// ByteBuffer
// ----------------------------------------------------------------------------

/**
 * \class ByteBuffer
 * \brief Byte buffer type.
 *
 * \ingroup lvbase
 */

/**
 * \brief Constructor from data
 *
 * Params are an array of unsigned chars and its size, both copied.
 */
ByteBuffer::ByteBuffer(ByteBuffer::Byte *data, size_t size)
    : m_data(new std::shared_ptr<ByteBufferData>(new ByteBufferData))
{
    m_data->get()->buffer = new ByteBuffer::Byte[size];
    m_data->get()->size = size;
    memcpy((void*)((*m_data)->buffer), (void*)data, size);
}

ByteBuffer::ByteBuffer(const ByteBuffer::Byte *data, size_t size)
    : m_data(new std::shared_ptr<ByteBufferData>(new ByteBufferData))
{
    m_data->get()->buffer = new ByteBuffer::Byte[size];
    m_data->get()->size = size;
    memcpy((void*)((*m_data)->buffer), (void*)data, size);
}

/**
 * \brief Copy constructor of ByteBuffer
 *
 * Creates only a shallow copy.
 */
ByteBuffer::ByteBuffer(const ByteBuffer& other)
    : m_data(new std::shared_ptr<ByteBufferData>)
{
    *m_data = *other.m_data;
}

/**
 * \brief Default constructor of ByteBuffer
 *
 * Initializes the object with zeroes.
 */
ByteBuffer::ByteBuffer()
    : m_data(new std::shared_ptr<ByteBufferData>(new ByteBufferData))
{
    m_data->get()->buffer = nullptr;
    m_data->get()->size = 0;
}

/**
 * \brief Destructor of ByteBuffer
 */
ByteBuffer::~ByteBuffer(){
    delete m_data;
}

/**
 * \brief Assignment operator for BytesType
 *
 * Reference counters are updated, and the pointers are copied from one to the other object.
 */
ByteBuffer &ByteBuffer::operator=(const ByteBuffer &other){
    if ( this != &other ){
        *m_data = *other.m_data;
    }
    return *this;
}

/**
 * @brief Equals relational operator for ByteBuffer
 */
bool ByteBuffer::operator ==(const ByteBuffer &other) const{
    return data() == other.data() && size() == other.size();
}

ByteBuffer ByteBuffer::encodeBase64(const ByteBuffer &bf, bool nullTerminate){
    return encodeBase64(bf.data(), bf.size(), nullTerminate);
}

ByteBuffer ByteBuffer::encodeBase64(const ByteBuffer::Byte *bytes, size_t size, bool nullTerminate){
    ByteBuffer bf;
    size_t resultMaxSize = ((size/3) + (size % 3 > 0)) * 4 + (nullTerminate ? 1 : 0);
    (*bf.m_data)->buffer = new ByteBuffer::Byte[resultMaxSize];
    ByteBuffer::Byte* c = (*bf.m_data)->buffer; // track of encoded position
    int cnt = 0; // store the number of bytes encoded by a single call
    base64_encodestate s;
    base64_init_encodestate(&s); // initialize state

    /* gather data from the input and send it to the output */
    cnt = base64_encode_block(bytes, size, c, &s);
    c += cnt;
    cnt = base64_encode_blockend(c, &s); // finalize encoding
    c += cnt;
    if ( nullTerminate ){
        *c = 0;
    }

    (*bf.m_data)->size = c - (*bf.m_data)->buffer;

    return bf;
}

ByteBuffer ByteBuffer::decodeBase64(const ByteBuffer &bf, bool nullTerminate){
    return decodeBase64(bf.data(), bf.size(), nullTerminate);
}

ByteBuffer ByteBuffer::decodeBase64(const ByteBuffer::Byte *bytes, size_t size, bool nullTerminate){
    ByteBuffer bf;
    (*bf.m_data)->buffer = new ByteBuffer::Byte[size + (nullTerminate ? 1 : 0)];
    ByteBuffer::Byte* c = (*bf.m_data)->buffer; // track of decoded position
    int cnt = 0; // store the number of bytes encoded by a single call
    base64_decodestate s;
    base64_init_decodestate(&s); // initialize state

    /* gather data from the input and send it to the output */
    cnt = base64_decode_block(bytes, size, c, &s);
    c += cnt;
    if ( nullTerminate )
        *c = 0;

    (*bf.m_data)->size = c - (*bf.m_data)->buffer;

    return bf;
}

/**
 * \brief Returns the internal data
 */
ByteBuffer::Byte *ByteBuffer::data() const{
    return (*m_data)->buffer;
}

/**
 * \brief Returns the size of the ByteBuffer
 */
size_t ByteBuffer::size() const{
    return (*m_data)->size;
}

}// namespace
