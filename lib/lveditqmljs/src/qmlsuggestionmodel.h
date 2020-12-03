#ifndef QMLSUGGESTIONMODEL_H
#define QMLSUGGESTIONMODEL_H

#include <QAbstractListModel>

namespace lv{

/// \private
class QmlSuggestionModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int addPosition READ addPosition CONSTANT)
public:

    /// \private
    class ItemData{
    public:
        enum Category {
            Property = 1,
            Object,
            Event,
            Function
        };

        ItemData(
            const QString& plabel,
            const QString& pObjType,
            const QString& ptype,
            const QString& pimport,
            const QString& pdoc,
            const QString& pcode,
            const int cat,
            bool group = false
        );

        QString label;
        QString objectType;
        QString type;
        QString importSpace;
        QString documentation;
        QString code;
        int category;
        bool isGroup;
    };

    enum Roles{
        Label = Qt::UserRole + 1,
        ObjectType,
        Type,
        ImportSpace,
        Documentation,
        Code,
        Category,
        IsGroup
    };

    QmlSuggestionModel(int addPosition, QObject* parent = 0);
    ~QmlSuggestionModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

    void addItem(const ItemData& item);
    int addPosition() const;

    void updateFilters();

public slots:
    void setFilter(const QString& filter);
    void setImportFilter(const QString& importFilter);
    void setTypeFilter(const QString& typeFilter);
    void setCategoryFilter(const int cat);

    QStringList importSpaces() const;
    QStringList types() const;

private:
    QList<ItemData>         m_data;
    QString                 m_filter;
    QString                 m_importFilter;
    QString                 m_typeFilter;
    int                     m_categoryFilter;
    QList<int>              m_filteredData;
    QHash<int, QByteArray>  m_roles;
    int                     m_addPosition;
};

inline QHash<int, QByteArray> QmlSuggestionModel::roleNames() const{
    return m_roles;
}

inline int QmlSuggestionModel::addPosition() const{
    return m_addPosition;
}

inline const QString &QmlSuggestionModel::filter() const{
    return m_filter;
}

inline int QmlSuggestionModel::rowCount(const QModelIndex &) const{
    return m_filteredData.size();
}

}
#endif // QMLSUGGESTIONMODEL_H
