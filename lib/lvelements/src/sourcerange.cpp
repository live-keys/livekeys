#include "sourcerange.h"

namespace lv{ namespace el{

Utf8 SourceRange::slice(const Utf8 &str) const{
    if ( length() > 0 )
        return str.substr(m_from, m_length);
    return Utf8();
}

std::string SourceRange::slice(const std::string &str) const{
    if ( length() > 0 )
        return str.substr(m_from, m_length);
    return std::string();
}

}} // namesapce lv, el
