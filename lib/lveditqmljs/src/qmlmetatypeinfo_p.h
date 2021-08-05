#ifndef LVQMLMETATYPEINFO_H
#define LVQMLMETATYPEINFO_H

#include <QObject>
#include <QJSValue>
#include "live/qmllanguageinfo.h"
#include "live/viewengine.h"

namespace lv{

class QmlMetaTypeInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlMetaTypeInfo(const QmlInheritanceInfo& typeInfo, ViewEngine *engine, QObject *parent = nullptr);
    ~QmlMetaTypeInfo();

public slots:
    QJSValue typeName(const QString& typeReference);
    QJSValue propertyInfo(const QString& propertyName);

private:
    QmlInheritanceInfo m_inheritanceInfo;
    ViewEngine*        m_engine;
};

}// namespace

#endif // LVQMLMETATYPEINFO_H
