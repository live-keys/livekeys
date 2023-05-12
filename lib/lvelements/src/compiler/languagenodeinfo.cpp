#include "languagenodeinfo_p.h"

namespace lv{ namespace el{

LanguageNodeInfo::LanguageNodeInfo(const std::string &tn, int t)
    : m_typename(tn), m_type(t)
{
}


LanguageNodeInfo::ConstPtr LanguageNodeInfo::create(std::string name){
    static int counter = 0;
    if ( name.rfind("Node") == name.size() - 5 )
        name = name.substr(0, name.size() - 4);
    return ConstPtr(new LanguageNodeInfo(name, counter++));
}


}} // namespace lv, el
