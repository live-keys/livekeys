#ifndef QQMLJSSETTINGS_H
#define QQMLJSSETTINGS_H

#include "qeditorsettingscategory.h"

namespace lcv{

class QQmlJsHighlighter;
class QQmlJsSettings : public QEditorSettingsCategory{

public:
    QQmlJsSettings(QQmlJsHighlighter* highlighter);
    ~QQmlJsSettings();

    void fromJson(const QJsonValue &json) Q_DECL_OVERRIDE;
    QJsonValue toJson() const Q_DECL_OVERRIDE;

private:
    QQmlJsHighlighter* m_highlighter;

};

}// namespace

#endif // QQMLJSSETTINGS_H
