#ifndef LVLINEMESSAGE_H
#define LVLINEMESSAGE_H

#include <QByteArray>
#include "lvviewglobal.h"

namespace lv{

class LV_VIEW_EXPORT LineMessage{

public:
    enum Type{
        Raw  = 1,
        Json = 2,
        Error = 9,
        Build = 10,
        Input = 11
    };

public:
    LineMessage(){}

    static QByteArray create(int type, const QByteArray& ba, int id = 0);
    static QByteArray create(int type, const char* ba, int len, int id = 0);

    QByteArray data;
    int        type;
    int        id;
};

inline QByteArray LineMessage::create(int type, const QByteArray &ba, int id){
    if ( !id ){
        return "Type:" + QByteArray::number(type) + ";Len:" + QByteArray::number(ba.length()) + "\r\n" + ba;
    } else {
        return "Type:" + QByteArray::number(type) +
               ";Len:" + QByteArray::number(ba.length()) +
               ";Id:" + QByteArray::number(id) + "\r\n" + ba;
    }
}

inline QByteArray LineMessage::create(int type, const char *ba, int len, int id){
    return create(type, QByteArray::fromRawData(ba, len), id);
}

}// namespace

#endif // LINEMESSAGE_H
