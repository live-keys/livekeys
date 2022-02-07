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

#include "mlnodetojson.h"
#include "live/exception.h"
#include <list>
#include <QByteArray>

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

namespace lv{
namespace ml{

namespace{

class MLNodeTrace{

private:
    std::list<MLNode*> path;
    std::string lastKey;

public:
    MLNodeTrace(MLNode* r){
        path.push_back(r);
    }

    template<typename T> MLNode* insert(const T& value){
        if ( path.back()->type() == MLNode::Object ){
            (*path.back())[lastKey] = value;
            return &(*path.back())[lastKey];
        } else if ( path.back()->type() == MLNode::Array ){
            MLNode* br = path.back();
            br->append(value);

            MLNode* ret = &((*br)[br->size() - 1]);
            return ret;
        } else {
            (*path.back()) = value;
            return &(*path.back());
        }
    }

    bool Null() { insert(MLNode::Null); return true; }
    bool Bool(bool b) { insert(b); return true; }
    bool Int(int i) { insert(i); return true; }
    bool Uint(unsigned u) { insert(static_cast<MLNode::IntType>(u)); return true; }
    bool Int64(int64_t i) { insert(static_cast<MLNode::IntType>(i)); return true; }
    bool Uint64(uint64_t u) { insert(static_cast<MLNode::IntType>(u)); return true; }
    bool Double(double d) { insert(d); return true; }
    bool RawNumber(const char* str, SizeType, bool) {
        insert(str);
        return true;
    }
    bool String(const char* str, SizeType, bool) {
        insert(str);
        return true;
    }

    bool StartObject() {
        path.push_back(insert(MLNode(MLNode::Object)));
        return true;
    }
    bool Key(const char* str, SizeType, bool) {
        lastKey = str;
        return true;
    }
    bool EndObject(SizeType){
        if (path.size() > 1)
            path.pop_back();
        return true;
    }

    bool StartArray() {
        path.push_back(insert(MLNode(MLNode::Array)));
        return true;
    }
    bool EndArray(SizeType){
        if (path.size() > 1)
            path.pop_back();
        return true;
    }
};

void recurseSerialize(const MLNode& n, rapidjson::Writer<rapidjson::StringBuffer>& writer){
    switch( n.type() ){
    case MLNode::Null:
        writer.Null();
        break;
    case MLNode::Object:{
        writer.StartObject();
        const MLNode::ObjectType& o = n.asObject();
        for ( auto it = o.begin(); it != o.end(); ++it ){
            writer.Key(it->first.c_str());
            recurseSerialize(it->second, writer);
        }
        writer.EndObject();
        break;
    }
    case MLNode::Array:{
        writer.StartArray();
        const MLNode::ArrayType& a = n.asArray();
        for ( auto it = a.begin(); it != a.end(); ++it ){
            recurseSerialize(*it, writer);
        }
        writer.EndArray();
        break;
    }
    case MLNode::Bytes:{
        MLNode::BytesType b = n.asBytes();
        QByteArray bs = b.toBase64();
        writer.String(bs.data(), bs.length());
        break;
    }
    case MLNode::String:{
        const std::string& s = n.asString();
        writer.String(s.c_str(), static_cast<rapidjson::SizeType>(s.length()));
        break;
    }
    case MLNode::Boolean:
        writer.Bool(n.asBool());
        break;
    case MLNode::Integer:
        writer.Int64(n.asInt());
        break;
    case MLNode::Float:
        writer.Double(n.asFloat());
        break;
    }
}

} // namespace

void toJson(const MLNode &n, std::string &result){
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    recurseSerialize(n, writer);

    result = s.GetString();
}

void fromJson(const std::string &data, MLNode &n){
    MLNodeTrace handler(&n);

    Reader reader;
    StringStream ss(data.c_str());
    reader.Parse(ss, handler);
}

void fromJson(const char *data, MLNode &n){
    MLNodeTrace handler(&n);

    Reader reader;
    StringStream ss(data);


    ParseResult pr = reader.Parse(ss, handler);
    if ( !pr ){
        THROW_EXCEPTION(lv::Exception,
            "Failed to parse json: " + std::string(GetParseError_En(pr.Code())) + " at " + std::to_string(pr.Offset()), Exception::toCode("Json"));
    }
}

}// namespace ml
}// namespace

