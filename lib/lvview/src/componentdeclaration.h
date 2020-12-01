#ifndef LVCOMPONENTDECLARATION_H
#define LVCOMPONENTDECLARATION_H

#include "live/lvviewglobal.h"
#include <QString>
#include <QUrl>

namespace lv{

class LV_VIEW_EXPORT ComponentDeclaration{

public:
    ComponentDeclaration(const QString& id, const QUrl& url);

    const QString& id() const{ return m_id; }
    const QUrl& url() const{ return m_url; }

private:
    QString m_id;
    QUrl    m_url;
};

}// namespace

#endif // LVCOMPONENTDECLARATION_H
