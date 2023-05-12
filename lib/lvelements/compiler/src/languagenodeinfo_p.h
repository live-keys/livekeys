#ifndef LVLANGUAGENODEINFO_H
#define LVLANGUAGENODEINFO_H

#include <memory>
#include <string>

namespace lv{ namespace el{

class LanguageNodeInfo{

public:
    typedef std::shared_ptr<const LanguageNodeInfo> ConstPtr;

public:

    const std::string& name() const;
    int type() const;

    static ConstPtr create(std::string name);

private:
    LanguageNodeInfo(const std::string& tn, int t);

    std::string m_typename;
    int         m_type;
};

inline const std::string &LanguageNodeInfo::name() const {
    return m_typename;
}

inline int LanguageNodeInfo::type() const{
    return m_type;
}


}} // namespace lv, el

#define LANGUAGE_NODE_INFO(_class) \
    public: \
        static const lv::el::LanguageNodeInfo::ConstPtr& nodeInfo(){ static LanguageNodeInfo::ConstPtr lni = LanguageNodeInfo::create(#_class); return lni; } \
        static std::string nodeInfoName(){ return nodeInfo()->name(); } \
        static int nodeInfoType(){ return nodeInfo()->type(); }



#endif // LVLANGUAGENODEINFO_H
