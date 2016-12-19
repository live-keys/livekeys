#ifndef QPROJECT_H
#define QPROJECT_H

#include <QObject>
#include <QHash>

//Filewatcher watches opened files
//Directorywatcher watches opened directories
//TODO: See if parser requires watched directories as well
//TODO: Add acitve and scope qproperties
//TODO: Solve: empty project

namespace lcv{

class QProjectFile;
class QProjectFileModel;
class QProjectDocument;

class QProject : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectDocument*  active    READ active    NOTIFY activeChanged)
    Q_PROPERTY(lcv::QProjectDocument*  inFocus   READ inFocus   NOTIFY inFocusChanged)
    Q_PROPERTY(lcv::QProjectFileModel* fileModel READ fileModel NOTIFY fileModelChanged)

public:
    QProject(QObject* parent = 0);
    ~QProject();

    void newProject();
    void openProject(const QString& path);
    void closeProject();
    void setActive(const QString& path);

    QProjectDocument* isOpened(const QString& path);
    void closeFocusedFile();

    lcv::QProjectFileModel* fileModel();
    lcv::QProjectDocument*  active() const;
    lcv::QProjectDocument*  inFocus() const;

public slots:
    void openFile(const QUrl& path);
    void openFile(const QString& path);
    void openFile(lcv::QProjectFile* file);
    void setActive(lcv::QProjectFile *file);

signals:
    void activeChanged(QProjectDocument* active);
    void inFocusChanged(QProjectDocument* inFocus);
    void fileModelChanged(QProjectFileModel* fileModel);

private:
    void setInFocus(QProjectDocument* document);
    void setActive(QProjectDocument* document);

private:
    QProjectFileModel* m_fileModel;

    QHash<QString, QProjectDocument*> m_openedFiles;
    QProjectDocument* m_active;
    QProjectDocument* m_focus;

};

inline QProjectFileModel* QProject::fileModel(){
    return m_fileModel;
}

inline QProjectDocument *QProject::active() const{
    return m_active;
}

inline QProjectDocument *QProject::inFocus() const{
    return m_focus;
}

}// namespace

#endif // QPROJECT_H
