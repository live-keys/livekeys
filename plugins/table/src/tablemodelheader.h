#ifndef LVTABLEMODELHEADER_H
#define LVTABLEMODELHEADER_H

#include <QAbstractListModel>
#include <QObject>

namespace lv {

class TableModelHeader : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int defaultColumnWidth READ defaultColumnWidth WRITE setDefaultColumnWidth NOTIFY defaultColumnWidthChanged)
    Q_PROPERTY(int contentWidth       READ contentWidth       NOTIFY contentWidthChanged)
    enum Roles{
        Name = Qt::UserRole + 1
    };

    class ColumnData{
    public:
        QString name;
        int     width;
    };

public:
    explicit TableModelHeader(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value = QVariant(), int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    void addColumn(const QString& name = "");
    void removeColumn(int idx);
    void initalizeData(int size);
    void notifyModelReset();

    int defaultColumnWidth() const;
    void setDefaultColumnWidth(int defaultColumnSize);

    int size() const;

    int contentWidth() const;

    void assignColumnName(int index, const QString& name);

public slots:
    void updateColumnWidth(int index, int width);
    int columnWidth(int index);

signals:
    void defaultColumnWidthChanged(int defaultColumnWidth);
    void contentWidthChanged();
private:
    ColumnData createColumnData(int index);

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
