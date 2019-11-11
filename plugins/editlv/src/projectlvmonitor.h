#ifndef LVPROJECTLVMONITOR_H
#define LVPROJECTLVMONITOR_H

#include <QObject>

namespace lv{

class ViewEngine;
class Project;
class LanguageLvExtension;
class ProjectLvScanner;

/// \private
class ProjectLvMonitor : public QObject{

    Q_OBJECT

    friend class LanguageLvExtension;

public:
    explicit ProjectLvMonitor(
        LanguageLvExtension* extension,
        Project* project,
        ViewEngine* engine,
        QObject *parent = nullptr);
    ~ProjectLvMonitor();

public slots:
    void __newProject(const QString& path);
    void __directoryChanged(const QString& path);
    void __fileChanged(const QString& path);

private:
    LanguageLvExtension* m_projectExtension;
    ProjectLvScanner*    m_scanner;
    Project*             m_project;
    ViewEngine*          m_engine;
};

}// namespace

#endif // LVPROJECTLVMONITOR_H
