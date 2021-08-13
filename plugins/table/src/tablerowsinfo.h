#ifndef LVTABLEROWSINFO_H
#define LVTABLEROWSINFO_H

#include <QAbstractTableModel>

namespace lv {

class TableRowsInfo : public QAbstractTableModel{

    Q_OBJECT
    Q_PROPERTY(int defaultRowHeight READ defaultRowHeight WRITE setDefaultRowHeight NOTIFY defaultRowHeightChanged)
    Q_PROPERTY(int contentHeight    READ contentHeight    NOTIFY contentHeightChanged)
    enum Roles{
        Value = Qt::UserRole + 1
    };

    class CellInfo{
    public:
        bool isSelected;
    };

    class RowData{
    public:
        QList<CellInfo> cells;
        QString         label;
        int             height;
    };

public:
    explicit TableRowsInfo(QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void notifyRowAdded();
    void notifyColumnAdded();
    void initializeData(int num);

    int defaultRowHeight() const;
    void setDefaultRowHeight(int defaultRowHeight);

    QString toString() const;

    int contentHeight() const;

    bool isSelected(int column, int row) const;
    void select(int column, int row);
    void deselectAll();

public slots:
    void updateRowHeight(int index, int height);
    int rowHeight(int index) const;

signals:
    void defaultRowHeightChanged();
    void contentHeightChanged();
private:
    RowData* rowDataAtWithCreate(int index);
    RowData* rowDataAt(int index) const;

    int                     m_num;
    QHash<int, QByteArray>  m_roles;
    int                     m_defaultRowHeight;
    QMap<int, RowData*>     m_data;
    int                     m_contentHeight;
};

inline QHash<int, QByteArray> TableRowsInfo::roleNames() const{
    return m_roles;
}

inline int TableRowsInfo::defaultRowHeight() const{
    return m_defaultRowHeight;
}

inline void TableRowsInfo::setDefaultRowHeight(int defaultRowHeight){
    if (m_defaultRowHeight == defaultRowHeight)
        return;

    m_defaultRowHeight = defaultRowHeight;
    emit defaultRowHeightChanged();
}

}

#endif // LVTABLEROWSINFO_H
