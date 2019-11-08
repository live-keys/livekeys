#ifndef LVSOURCERANGE_H
#define LVSOURCERANGE_H

#include "live/elements/lvelementsglobal.h"
#include "live/utf8.h"

#include "inttypes.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT SourceRange{

public:
    SourceRange() : m_from(0), m_length(0){}
    SourceRange(uint32_t from, uint32_t len) : m_from(from), m_length(len){}

    uint32_t from() const{ return m_from; }
    uint32_t length() const{ return m_length; }
    uint32_t to() const{ return m_from + m_length; }
    bool isValid() const{ return m_length > 0; }

    Utf8 slice(const Utf8& str) const;
    std::string slice(const std::string& str) const;

private:
    uint32_t m_from;
    uint32_t m_length;
};

}} // namespace lv, el

#endif // LVSOURCERANGE_H
