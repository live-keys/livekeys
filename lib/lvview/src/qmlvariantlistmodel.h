#ifndef QMLVARIANTLISTMODEL_H
#define QMLVARIANTLISTMODEL_H

#include <QAbstractListModel>
#include "qmlvariantlist.h"

namespace lv {

class QmlVariantListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QmlVariantListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    QmlVariantListModel(QmlVariantList* model);
    ~QmlVariantListModel(){}

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void appendData(const QVariant& data);
    void removeAt(int index);
    QVariant at(int index);
    int size() const;

private:
    QmlVariantList* m_qmlVariantList;
};


}
#endif // QMLVARIANTLISTMODEL_H
