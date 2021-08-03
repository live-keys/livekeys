#ifndef LVLANGUAGELVEXTENSION_H
#define LVLANGUAGELVEXTENSION_H

#include <QObject>
#include <QQmlParserStatus>

#include "live/documenthandler.h"
#include "live/project.h"
#include "live/viewengine.h"
#include "projectlvmonitor.h"
#include "languagelvhandler.h"
#include "editlvsettings.h"

namespace lv{

class Settings;
class ProjectQmlScope;
class LanguageQmlHandler;
class ProjectQmlScanner;

class LanguageLvExtension : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    LanguageLvExtension(QObject* parent = nullptr);
    ~LanguageLvExtension() override;

    void classBegin() override{}
    void componentComplete() override;

    void addLanguageHandler(LanguageLvHandler* handler);
    void removeLanguageHandler(LanguageLvHandler* handler);

public slots:
    QObject* createHandler(ProjectDocument* document, CodeHandler* handler);

private:
    Q_DISABLE_COPY(LanguageLvExtension)

    Project*               m_project;
    ViewEngine*            m_engine;
    EditLvSettings*        m_settings;
    ProjectLvMonitor*      m_scanMonitor;
    QList<LanguageLvHandler*> m_codeHandlers;
};

} // namespace

#endif // LVLANGUAGELVEXTENSION_H
