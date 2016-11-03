#ifndef QPROJECT_H
#define QPROJECT_H

#include <QObject>
#include <QHash>

//Filewatcher watches opened files
//Directorywatcher watches opened directories
//TODO: See if parser requires watched directories as well
//TODO: Add acitve and scope qproperties

namespace lcv{

class QProjectFile;
class QProjectFileModel;

class QProject : public QObject{

    Q_OBJECT

public:
    QProject(QObject* parent = 0);
    ~QProject();

    void openProject(const QString& path);
    void closeProject();
    void openFile(const QString& path);
    void setActive(const QString& path);
    QProjectFile* isOpened(const QString& path);
    void closeFocusedFile();

    QProjectFileModel* fileModel();

signals:
    void activeChanged(QProjectFile* active);

private:
    QProjectFileModel* m_fileModel;

    QHash<QString, QProjectFile*> m_openedFiles;
    QProjectFile* m_active;
    QProjectFile* m_focus;

};

inline QProjectFileModel* QProject::fileModel(){
    return m_fileModel;
}

}// namespace

#endif // QPROJECT_H
