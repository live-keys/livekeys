#ifndef QMLEVENTMODEL_H
#define QMLEVENTMODEL_H

#include <QAbstractListModel>

namespace lv {

class QmlEventModel : public QAbstractListModel
{
    Q_OBJECT
public:

    /// \private
    class EventData{
    public:
        EventData(
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

    QmlEventModel(int addPosition, QObject* parent = 0);
    ~QmlEventModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

    void addItem(const EventData& item);
    int addPosition() const;

    void updateFilters();

public slots:
    void setFilter(const QString& filter);
    void setTypeFilter(const QString& typeFilter);

    QStringList types() const;

private:
    QList<EventData>       m_data;
    QString                m_filter;
    QString                m_typeFilter;
    QList<int>             m_filteredData;
    QHash<int, QByteArray> m_roles;
    int                    m_addPosition;
};

inline QHash<int, QByteArray> QmlEventModel::roleNames() const{
    return m_roles;
}

inline int QmlEventModel::addPosition() const{
    return m_addPosition;
}

inline const QString &QmlEventModel::filter() const{
    return m_filter;
}

inline int QmlEventModel::rowCount(const QModelIndex &) const{
    return m_filteredData.size();
}


}

#endif // QMLEVENTMODEL_H
