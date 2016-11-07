#ifndef QPROJECTFILEMODEL_H
#define QPROJECTFILEMODEL_H

#include <QAbstractItemModel>
#include <functional>

// actions from livecvproject

// projectdir->findDirEntries(dirname)
// projectdir->activeFileChaned(file)
// projectdir->dirtyFileChanged(file)
// projectdir->scopeFileChanged(file)

namespace lcv{

class QProjectEntry;
class QProjectFile;
class QProjectFileModel : public QAbstractItemModel{

    Q_OBJECT

public:
    enum Roles{
        UrlStringRole = Qt::UserRole + 1
    };
    Q_ENUM(Roles)

public:
    QProjectFileModel(QObject* parent = 0);
    ~QProjectFileModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex rootIndex();
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    bool hasChildren(const QModelIndex &parent) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const;

    void createProject();
    void openProject(const QString& path);
    void closeProject();

//    QProjectEntry* visitFiles(std::function<bool(QProjectEntry*))> visitor);

    QProjectFile* openFile(const QString& file);
    QProjectEntry* findPathInEntry(QProjectEntry* entry, const QString& path);
    QProjectEntry* root();

public slots:
    void fileClosed(const QString& path);

    void fileAdded(const QString& path);
    void fileRemoved(const QString& path);

    void entryRemoved(const QModelIndex& item);
    void entryRemoved(QProjectEntry* entry);
    void entryRemoved(const QModelIndex &item, lcv::QProjectEntry* entry);
    void entryAdded(lcv::QProjectEntry* item, lcv::QProjectEntry* parent);

    void moveEntry(lcv::QProjectEntry* item, lcv::QProjectEntry* parent);

    void expandEntry(lcv::QProjectEntry* entry) const;

    lcv::QProjectEntry *itemAt(const QModelIndex& index) const;

signals:
    void projectNodeChanged(QModelIndex index);

private:
    QProjectFile* openExternalFile(const QString& file);
    QProjectEntry *itemOrRoot(const QModelIndex &index) const;

    QProjectEntry *m_root;
};

inline QProjectEntry *QProjectFileModel::root(){
    return m_root;
}

}// namespace

#endif // QPROJECTFILEMODEL_H
