#ifndef LVTABLEMODELHEADER_H
#define LVTABLEMODELHEADER_H

#include <QAbstractListModel>
#include <QObject>

namespace lv {

class TableModel;
class TableModelHeader : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int defaultColumnWidth READ defaultColumnWidth WRITE setDefaultColumnWidth NOTIFY defaultColumnWidthChanged)
    Q_PROPERTY(int contentWidth       READ contentWidth       NOTIFY contentWidthChanged)
    enum Roles{
        Name = Qt::UserRole + 1
    };

    class ColumnData{
    public:
        int width;
    };

public:
    explicit TableModelHeader(TableModel *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void notifyColumnAdded(int index);
    void notifyColumnRemoved(int idx);
    void notifyModelReset();

    int defaultColumnWidth() const;
    void setDefaultColumnWidth(int defaultColumnSize);

    int size() const;

    int contentWidth() const;

public slots:
    void updateColumnWidth(int index, int width);
    int columnWidth(int index);

signals:
    void defaultColumnWidthChanged(int defaultColumnWidth);
    void contentWidthChanged();

private:
    ColumnData createColumnData();

    QHash<int, QByteArray> m_roles;
    int                    m_defaultColumnWidth;
    QList<ColumnData>      m_data;
    int                    m_contentWidth;
};

inline QHash<int, QByteArray> TableModelHeader::roleNames() const{
    return m_roles;
}

inline int TableModelHeader::defaultColumnWidth() const{
    return m_defaultColumnWidth;
}

inline void TableModelHeader::setDefaultColumnWidth(int defaultColumnSize){
    if (m_defaultColumnWidth == defaultColumnSize)
        return;

    m_defaultColumnWidth = defaultColumnSize;
    emit defaultColumnWidthChanged(m_defaultColumnWidth);
}

#endif // LVTABLEMODELHEADER_H

} //namespace
