#ifndef TABLEHEADER_H
#define TABLEHEADER_H

#include <QAbstractListModel>
#include <QObject>

namespace lv {

class TableHeader : public QAbstractListModel
{
    Q_OBJECT
    enum Roles{
        Value = Qt::UserRole + 1
    };
public:

    explicit TableHeader(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value = QVariant(), int role = Qt::EditRole) override;

    void addColumn();
    void removeColumn();
    void initalizeData(int size);
private:
    static QString letterIndex(int idx);

    QHash<int, QByteArray>  m_roles;
    std::vector<QString>    m_data;
};

inline QHash<int, QByteArray> TableHeader::roleNames() const
{
    return m_roles;
}

#endif // TABLEHEADER_H

} //namespace
