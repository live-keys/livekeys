#ifndef LVTABLEMODELROWSINFO_H
#define LVTABLEMODELROWSINFO_H

#include <QAbstractTableModel>

namespace lv {

class TableModelRowsInfo : public QAbstractTableModel{

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
    explicit TableModelRowsInfo(QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void notifyRowAdded(int index);
    void notifyColumnAdded(int index);
    void notifyModelReset(int newRowCount = 0);
    void initializeData(int num);

    void removeRow(int idx);
    void removeColumn(int idx);

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

    int                     m_rowCount;
    QHash<int, QByteArray>  m_roles;
    int                     m_defaultRowHeight;
    QMap<int, RowData*>     m_data;
    int                     m_contentHeight;
};

inline QHash<int, QByteArray> TableModelRowsInfo::roleNames() const{
    return m_roles;
}

inline int TableModelRowsInfo::defaultRowHeight() const{
    return m_defaultRowHeight;
}

inline void TableModelRowsInfo::setDefaultRowHeight(int defaultRowHeight){
    if (m_defaultRowHeight == defaultRowHeight)
        return;

    m_defaultRowHeight = defaultRowHeight;
    emit defaultRowHeightChanged();
}

}

#endif // LVTABLEMODELROWSINFO_H
