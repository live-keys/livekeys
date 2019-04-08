#ifndef QMLOBJECTLISTMODEL_H
#define QMLOBJECTLISTMODEL_H

#include <QAbstractListModel>
#include "live/lvviewglobal.h"

namespace lv {

class QmlObjectList;

/// \private
class QmlObjectListModel: public QAbstractListModel{

    Q_OBJECT

public:
    explicit QmlObjectListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    QmlObjectListModel(QmlObjectList* model);
    ~QmlObjectListModel(){}

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void appendData(QObject* data);
    void removeAt(int index);
    QObject* at(int index);
    int size() const;

private:
    QmlObjectList* m_qmlObjectList;
};

}

#endif // QMLOBJECTLISTMODEL_H

