#ifndef LVVIEWENGINEINTERCEPTOR_H
#define LVVIEWENGINEINTERCEPTOR_H

#include "lvviewglobal.h"

#include <QUrl>
#include <QQmlAbstractUrlInterceptor>

namespace lv{

class LV_VIEW_EXPORT ViewEngineInterceptor{

public:
    class LV_VIEW_EXPORT ContentResult{
    public:
        ContentResult(bool isValid, const QByteArray& content = "")
            : m_isValid(isValid), m_content(content)
        {}
        bool isValid() const{ return m_isValid; }
        const QByteArray& content() const{ return m_content; }
    private:
         bool       m_isValid;
         QByteArray m_content;
    };

public:
    ViewEngineInterceptor();
    virtual ~ViewEngineInterceptor();

public:
    virtual QUrl interceptUrl(const QUrl& path, QQmlAbstractUrlInterceptor::DataType dataType) = 0;
    virtual ContentResult interceptContent(const QUrl& path) = 0;

};

}// namespace

#endif // LVVIEWENGINEINTERCEPTOR_H
