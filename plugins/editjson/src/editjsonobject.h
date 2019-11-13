#ifndef LVEDITJSONOBJECT_H
#define LVEDITJSONOBJECT_H

#include <QObject>
#include <QQmlParserStatus>

#include "live/projectdocument.h"
#include "live/documenthandler.h"
#include "live/editorsettings.h"

#include "qmllanguageobject.h"

#include "tree_sitter/parser.h"


#ifdef __cplusplus
extern "C" {
#endif

const TSLanguage *tree_sitter_json(void);

#ifdef __cplusplus
}
#endif


namespace lv{

class EditJsonObject : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlLanguageObject* language READ language CONSTANT)

public:
    EditJsonObject(QQmlEngine *parent = nullptr);

    lv::QmlLanguageObject* language() const;

public slots:
    QObject *createHandler(
        ProjectDocument* document,
        DocumentHandler* handler,
        const QString& settingsCategory,
        const QJSValue& initialSettings
    );

private:
    Q_DISABLE_COPY(EditJsonObject)

    ViewEngine*        m_engine;
    EditorSettings*    m_editorSettings;
    QmlLanguageObject* m_language;

};

inline QmlLanguageObject *EditJsonObject::language() const{
    return m_language;
}

}// namespace

#endif // LVEDITJSONOBJECT_H
