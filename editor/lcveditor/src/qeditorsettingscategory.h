#ifndef QEDITORSETTINGSCATEGORY_H
#define QEDITORSETTINGSCATEGORY_H

#include "qlcveditorglobal.h"
#include <QJsonObject>

namespace lcv{

class Q_LCVEDITOR_EXPORT QEditorSettingsCategory{

public:
    QEditorSettingsCategory();
    ~QEditorSettingsCategory();

    virtual void fromJson(const QJsonValue&){}
    virtual QJsonValue toJson() const{ return QJsonValue(); }
};

}// namespace

#endif // QEDITORSETTINGSCATEGORY_H
