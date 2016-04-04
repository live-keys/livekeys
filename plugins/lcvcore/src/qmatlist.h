#ifndef QMATLIST_H
#define QMATLIST_H

#include <QAbstractListModel>
#include "qmat.h"

class QMatList : public QAbstractListModel{

    Q_OBJECT

public:
    explicit QMatList(QObject* parent = 0);
    ~QMatList();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void appendMat(QMat* mat);
    void removeMat(QMat* mat);
    void removeAt(int index);
    QMat* at(int index);

private:
    QList<QMat*> m_list;

};


inline int QMatList::rowCount(const QModelIndex &) const{
    return m_list.size();
}

#endif // QMATLIST_H
