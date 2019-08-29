#ifndef LVLINECAPTURE_H
#define LVLINECAPTURE_H

#include "live/lvviewglobal.h"
#include "live/linemessage.h"

#include <functional>

namespace lv{

class LV_VIEW_EXPORT LineCapture{

public:
    enum ErrorType{
        ParseTypeError = 0,
        ParseLossData,
        ParseLengthError,
        TypeNumberError,
        LengthNumberError
    };

public:
    LineCapture();
    ~LineCapture();

    void append(const QByteArray& ba);
    void onMessage(std::function<void(const LineMessage&, void* data)> handler, void* handlerData = 0);
    void onError(std::function<void(int, const std::string&)> handler);

    unsigned long long expectedSize() const;

private:
    std::function<void(const LineMessage&, void*)> m_handler;
    std::function<void(int, const std::string&)> m_errorHandler;
    void* m_handlerData;

    unsigned long long m_expectSize;
    LineMessage        m_current;
};

inline void LineCapture::onMessage(std::function<void (const LineMessage &, void *)> handler, void *handlerData){
    m_handler = handler;
    m_handlerData = handlerData;
}

inline void LineCapture::onError(std::function<void (int, const std::string &)> handler){
    m_errorHandler = handler;
}

inline unsigned long long LineCapture::expectedSize() const{
    return m_expectSize;
}

}// namespace

#endif // LVLINECAPTURE_H
