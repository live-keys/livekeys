#ifndef LVQMLITEMMODEL_H
#define LVQMLITEMMODEL_H

#include <QObject>
#include <QAbstractListModel>

namespace lv{

/// \private
class QmlItemModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int addPosition READ addPosition CONSTANT)

public:

    /// \private
    class ItemData{
    public:
        ItemData(const QString plabel, const QString& pimport, const QString& pdoc, const QString& pcode);

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
    QmlItemModel(int addPosition, QObject* parent = 0);
    ~QmlItemModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

    void addItem(const ItemData& item);
    int addPosition() const;

    void updateFilters();

public slots:
    void setFilter(const QString& filter);
    void setImportFilter(const QString& categoryFilter);

    QStringList importSpaces();

private:
    QList<ItemData>         m_data;
    QString                 m_filter;
    QString                 m_importFilter;
    QList<int>              m_filteredData;
    QHash<int, QByteArray>  m_roles;
    int                     m_addPosition;
};

inline QHash<int, QByteArray> QmlItemModel::roleNames() const{
    return m_roles;
}

inline int QmlItemModel::addPosition() const{
    return m_addPosition;
}

inline const QString &QmlItemModel::filter() const{
    return m_filter;
}

inline int QmlItemModel::rowCount(const QModelIndex &) const{
    return m_filteredData.size();
}

}// namespace

#endif // LVQMLITEMMODEL_H
