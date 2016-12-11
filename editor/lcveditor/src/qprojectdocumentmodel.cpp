#include "qprojectdocumentmodel.h"
#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qproject.h"
#include <QDebug>

namespace lcv{

QProjectDocumentModel::QProjectDocumentModel(QProject *project)
    : QAbstractListModel(project)
{
    m_roles[QProjectDocumentModel::Name]   = "name";
    m_roles[QProjectDocumentModel::Path]   = "path";
    m_roles[QProjectDocumentModel::IsOpen] = "isOpen";
}

QProjectDocumentModel::~QProjectDocumentModel(){
    closeDocuments();
}

int QProjectDocumentModel::rowCount(const QModelIndex &) const{
    return m_openedFiles.size();
}

QVariant QProjectDocumentModel::data(const QModelIndex &index, int role) const{
    if ( index.row() < 0 || index.row() >= m_openedFiles.size() )
        return QVariant();
    QHash<QString, QProjectDocument*>::const_iterator it = m_openedFiles.constBegin() + index.row();

    if ( role == QProjectDocumentModel::Name ){
        return it.value()->file()->name();
    } else if ( role == QProjectDocumentModel::Path ){
        return it.value()->file()->path();
    } else if ( role == QProjectDocumentModel::IsOpen ){
        return true;
    }

    return QVariant();
}

QHash<int, QByteArray> QProjectDocumentModel::roleNames() const{
    return m_roles;
}

void QProjectDocumentModel::openDocument(const QString &path, QProjectDocument *document){
    beginResetModel();
    m_openedFiles[path] = document;
    endResetModel();
}

void QProjectDocumentModel::closeDocuments(){
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        delete it.value();
    }
    m_openedFiles.clear();
}

void QProjectDocumentModel::rescanDocuments(){
    //HERE
}


}// namespace

