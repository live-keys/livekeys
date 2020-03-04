#ifndef LVCOMPONENTMODEL_H
#define LVCOMPONENTMODEL_H

#include <QObject>
#include <QAbstractListModel>

namespace lv{

/// \private
class LvComponentModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int addPosition READ addPosition CONSTANT)

public:

    /// \private
    class ComponentData{
    public:
        ComponentData(const QString plabel, const QString& pimport, const QString& pdoc, const QString& pcode);

        QString label;
        QString importSpace;
        QString documentation;
        QString code;
    };

public:
    enum Roles{
        Label = Qt::UserRole + 1,
        ImportSpace,
        Documentation,
        Code
    };

public:
    LvComponentModel(int addPosition, QObject* parent = 0);
    ~LvComponentModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

    void addItem(const ComponentData& item);
    int addPosition() const;

    void updateFilters();

public slots:
    void setFilter(const QString& filter);
    void setImportFilter(const QString& categoryFilter);

    QStringList importSpaces();

private:
    QList<ComponentData>    m_data;
    QString                 m_filter;
    QString                 m_importFilter;
    QList<int>              m_filteredData;
    QHash<int, QByteArray>  m_roles;
    int                     m_addPosition;
};

inline QHash<int, QByteArray> LvComponentModel::roleNames() const{
    return m_roles;
}

inline int LvComponentModel::addPosition() const{
    return m_addPosition;
}

inline const QString &LvComponentModel::filter() const{
    return m_filter;
}

inline int LvComponentModel::rowCount(const QModelIndex &) const{
    return m_filteredData.size();
}

}// namespace

#endif // LVQMLITEMMODEL_H
