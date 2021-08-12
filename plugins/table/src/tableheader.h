#ifndef TABLEHEADER_H
#define TABLEHEADER_H

#include <QAbstractListModel>
#include <QObject>

namespace lv {

class TableHeader : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int defaultColumnWidth READ defaultColumnWidth WRITE setDefaultColumnWidth NOTIFY defaultColumnWidthChanged)

    enum Roles{
        Name = Qt::UserRole + 1
    };

    class ColumnData{
    public:
        QString name;
        int     width;
    };

public:
    explicit TableHeader(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value = QVariant(), int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    void addColumn();
    void removeColumn();
    void initalizeData(int size);

    int defaultColumnWidth() const;
    void setDefaultColumnWidth(int defaultColumnSize);

public slots:
    void updateColumnWidth(int index, int width);
    int columnWidth(int index);

signals:
    void defaultColumnWidthChanged(int defaultColumnWidth);

private:
    ColumnData createColumnData(int index);

    QHash<int, QByteArray> m_roles;
    int                    m_defaultColumnWidth;
    QList<ColumnData> m_data;
};

inline QHash<int, QByteArray> TableHeader::roleNames() const{
    return m_roles;
}

inline int TableHeader::defaultColumnWidth() const{
    return m_defaultColumnWidth;
}

inline void TableHeader::setDefaultColumnWidth(int defaultColumnSize){
    if (m_defaultColumnWidth == defaultColumnSize)
        return;

    m_defaultColumnWidth = defaultColumnSize;
    emit defaultColumnWidthChanged(m_defaultColumnWidth);
}

#endif // TABLEHEADER_H

} //namespace
