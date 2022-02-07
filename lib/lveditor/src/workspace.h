/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVWORKSPACE_H
#define LVWORKSPACE_H

#include <QObject>
#include <QQuickWindow>
#include "lveditorglobal.h"
#include "live/mlnode.h"
#include "live/project.h"
#include "live/qmlpropertywatcher.h"
#include "startupmodel.h"

namespace lv{

class ProjectWorkspace;
class WorkspaceLayer;

/// \private
class LV_EDITOR_EXPORT Workspace : public QObject{

    Q_OBJECT

public:
    class LV_EDITOR_EXPORT Message{

    public:
        enum Type{
            ProjectOpen,
            ProjectClose,
            ProjectActiveChange,
            DocumentOpen,
            DocumentClose,
            DocumentMonitor,
            DocumentContentsChange,
            WindowRectChange,
            WindowStateChange
        };

        typedef QSharedPointer<Message> Ptr;
        typedef QSharedPointer<const Message> ConstPtr;

    public:
        Message(Type type, const MLNode& data);

        Type type() const{ return m_type; }
        const MLNode& data() const{ return m_data; }

    private:
        Type   m_type;
        MLNode m_data;
    };

public:
    explicit Workspace(Project* project, WorkspaceLayer *parent = nullptr);
    ~Workspace();

    static QString absolutePath(const QString& appDataPath);
    static QString absoluteDir(const QString& dir);

    ProjectWorkspace* currentProjectWorkspace() const;

    static Workspace* getFromContext(QQmlContext* context);

    bool wasRecentsFileFound() const;
    StartupModel* recents();

    void saveRecents();

    static void registerTypes(const char* uri);

signals:
    void projectOpen(const QString& path);
    void projectInitialized(const QString& path, ProjectWorkspace* workspace);
    void projectClose(ProjectWorkspace* workspace);

public slots:
    void whenProjectPathChange(const QString& path);


private:
    void saveRecentsIfChanged();

    Project*             m_project;
    ProjectWorkspace*    m_currentProjectWorkspace;
    StartupModel*        m_recentsModel;
    bool                 m_recentsChanged;
    bool                 m_recentsFileFound;
};

inline ProjectWorkspace *Workspace::currentProjectWorkspace() const{
    return m_currentProjectWorkspace;
}

inline bool Workspace::wasRecentsFileFound() const{
    return m_recentsFileFound;
}

}// namespace

#endif // LVWORKSPACE_H
