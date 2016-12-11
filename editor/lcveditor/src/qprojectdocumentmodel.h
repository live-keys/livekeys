#ifndef QPROJECTDOCUMENTMODEL_H
#define QPROJECTDOCUMENTMODEL_H

#include <QAbstractListModel>
#include "qlcveditorglobal.h"

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

    QProjectDocument* isOpened(const QString& path);
    void openDocument(const QString& path, QProjectDocument* document);
    void closeDocuments();

    const QHash<QString, QProjectDocument*> openedFiles() const;

public slots:
    void rescanDocuments();

private:
    QHash<int, QByteArray> m_roles;

    QHash<QString, QProjectDocument*> m_openedFiles;

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
