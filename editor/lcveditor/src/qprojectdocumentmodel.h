#ifndef QPROJECTDOCUMENTMODEL_H
#define QPROJECTDOCUMENTMODEL_H

#include <QAbstractListModel>
#include "qlcveditorglobal.h"

class QFileSystemWatcher;

namespace lcv{

class QProject;
class QProjectDocument;
class Q_LCVEDITOR_EXPORT QProjectDocumentModel : public QAbstractListModel{

    Q_OBJECT

public:
    enum Roles{
        Name = Qt::UserRole + 1,
        Path,
        IsOpen
    };

public:
    QProjectDocumentModel(QProject* project);
    ~QProjectDocumentModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void openDocument(const QString& path, QProjectDocument* document);
    void closeDocuments();

    const QHash<QString, QProjectDocument*> openedFiles() const;

    void updateDocumeMonitoring(QProjectDocument* document, bool monitor);

    void closeDocumentsInPath(const QString& path, bool closeIfActive = false);
    void closeDocument(const QString& path, bool closeIfActive = false);

public slots:
    void rescanDocuments();
    void monitoredFileChanged(const QString& path);

    bool saveDocuments();
    lcv::QProjectDocument* isOpened(const QString& path);

    QStringList listUnsavedDocuments();
    QStringList listUnsavedDocumentsInPath(const QString& path);

signals:
    void monitoredDocumentChanged(lcv::QProjectDocument* document);
    void documentChangedOutside(lcv::QProjectDocument* document);

private:
    QHash<int, QByteArray> m_roles;

    QHash<QString, QProjectDocument*> m_openedFiles;

    QFileSystemWatcher* fileWatcher();
    QFileSystemWatcher* m_fileWatcher;

};

inline QProjectDocument *QProjectDocumentModel::isOpened(const QString &path){
    if ( m_openedFiles.contains(path) )
        return m_openedFiles[path];
    return 0;
}

inline const QHash<QString, QProjectDocument *> QProjectDocumentModel::openedFiles() const{
    return m_openedFiles;
}

}// namespace

#endif // QPROJECTDOCUMENTMODEL_H
