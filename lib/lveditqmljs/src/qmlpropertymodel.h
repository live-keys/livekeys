#ifndef LVQMLPROPERTYMODEL_H
#define LVQMLPROPERTYMODEL_H

#include <QObject>
#include <QAbstractListModel>

namespace lv{

/// \private
class QmlPropertyModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int addPosition READ addPosition CONSTANT)

public:

    /// \private
    class PropertyData{
    public:
        PropertyData(
            const QString& label,
            const QString& objectType,
            const QString& type,
            const QString& doc,
            const QString& code
        );

        QString label;
        QString objectType;
        QString type;
        QString documentation;
        QString code;
    };

public:
    enum Roles{
        Label = Qt::UserRole + 1,
        ObjectType,
        Type,
        Documentation,
        Code
    };

public:
    QmlPropertyModel(int addPosition, QObject* parent = 0);
    ~QmlPropertyModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

    void addItem(const PropertyData& item);
    int addPosition() const;

    void updateFilters();

public slots:
    void setFilter(const QString& filter);
    void setTypeFilter(const QString& typeFilter);

    QStringList types() const;

private:
    QList<PropertyData>    m_data;
    QString                m_filter;
    QString                m_typeFilter;
    QList<int>             m_filteredData;
    QHash<int, QByteArray> m_roles;
    int                    m_addPosition;
};

inline QHash<int, QByteArray> QmlPropertyModel::roleNames() const{
    return m_roles;
}

inline int QmlPropertyModel::addPosition() const{
    return m_addPosition;
}

inline const QString &QmlPropertyModel::filter() const{
    return m_filter;
}

inline int QmlPropertyModel::rowCount(const QModelIndex &) const{
    return m_filteredData.size();
}

}// namespace


#endif // LVQMLPROPERTYMODEL_H
