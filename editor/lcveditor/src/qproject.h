/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QPROJECT_H
#define QPROJECT_H

#include "qlcveditorglobal.h"

#include <QObject>
#include <QHash>
#include "qlockedfileiosession.h"
#include "qprojectdocument.h"

namespace lcv{

class QProjectEntry;
class QProjectFile;
class QProjectFileModel;
class QProjectNavigationModel;
class QProjectDocumentModel;

class Q_LCVEDITOR_EXPORT QProject : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectDocument*  active                READ active          NOTIFY activeChanged)
    Q_PROPERTY(lcv::QProjectDocument*  inFocus               READ inFocus         NOTIFY inFocusChanged)
    Q_PROPERTY(lcv::QProjectFileModel* fileModel             READ fileModel       NOTIFY fileModelChanged)
    Q_PROPERTY(lcv::QProjectNavigationModel* navigationModel READ navigationModel NOTIFY navigationModelChanged)
    Q_PROPERTY(lcv::QProjectDocumentModel* documentModel     READ documentModel   NOTIFY documentModelChanged)
    Q_PROPERTY(QString path                                  READ path            NOTIFY pathChanged)

    friend class QProjectFileModel;
    friend class QProjectDocument;
    friend class QProjectDocumentModel;

public:
    QProject(QObject* parent = 0);
    ~QProject();

    void setActive(const QString& path);

    QProjectFile* lookupBestFocus(QProjectEntry* entry);

    QProjectDocument* isOpened(const QString& path);

    lcv::QProjectFileModel* fileModel();
    lcv::QProjectNavigationModel* navigationModel();
    lcv::QProjectDocumentModel* documentModel();
    lcv::QProjectDocument*  active() const;
    lcv::QProjectDocument*  inFocus() const;

    const QString& path() const;

    QLockedFileIOSession::Ptr lockedFileIO();

public slots:
    void newProject();
    void closeProject();
    void openFile(const QUrl& path, int mode);
    void openFile(const QString& path, int mode);
    void openFile(lcv::QProjectFile* file, int mode);
    void setActive(lcv::QProjectFile *file);

    bool isDirProject() const;
    bool isFileInProject(const QUrl& path) const;
    bool isFileInProject(const QString& path) const;

    void openProject(const QString& path);
    void openProject(const QUrl& url);

    void closeFile(const QString& path);
    void closeFocusedFile();

    QString dir() const;

signals:
    void pathChanged(QString path);
    void activeChanged(QProjectDocument* active);
    void inFocusChanged(QProjectDocument* inFocus);

    void fileModelChanged(QProjectFileModel* fileModel);
    void navigationModelChanged(QProjectNavigationModel* navigationModel);
    void documentModelChanged(QProjectDocumentModel* documentModel);

    void directoryChanged(const QString& path);
    void fileChanged(const QString& path);

private:
    void setInFocus(QProjectDocument* document);
    void setActive(QProjectDocument* document);

private:
    QProjectFileModel*       m_fileModel;
    QProjectNavigationModel* m_navigationModel;
    QProjectDocumentModel*   m_documentModel;

    QLockedFileIOSession::Ptr m_lockedFileIO;

    QProjectDocument* m_active;
    QProjectDocument* m_focus;
    QString           m_path;
};

inline QProjectFileModel* QProject::fileModel(){
    return m_fileModel;
}

inline QProjectNavigationModel *QProject::navigationModel(){
    return m_navigationModel;
}

inline QProjectDocumentModel *QProject::documentModel(){
    return m_documentModel;
}

inline QProjectDocument *QProject::active() const{
    return m_active;
}

inline QProjectDocument *QProject::inFocus() const{
    return m_focus;
}

inline const QString &QProject::path() const{
    return m_path;
}

inline QLockedFileIOSession::Ptr QProject::lockedFileIO(){
    return m_lockedFileIO;
}


}// namespace

#endif // QPROJECT_H
