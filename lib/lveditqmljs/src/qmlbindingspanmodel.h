#ifndef LVQMLBINDINGSPANMODEL_H
#define LVQMLBINDINGSPANMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "qmlbindingpath.h"
#include "qmlbindingspan.h"
#include "qmlscopesnap_p.h"
#include "live/runnable.h"

namespace lv{

class QmlUsageGraphScanner;
class QmlBindingSpanModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int inputPathIndex READ inputPathIndex NOTIFY inputPathIndexChanged)

public:
    // Model roles
    enum Roles{
        Path = Qt::UserRole + 1,
        IsConnected,
        RunnableName
    };

public:
    explicit QmlBindingSpanModel(QmlEditFragment* edit, QObject *parent = nullptr);
    ~QmlBindingSpanModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    int inputPathIndex() const;


signals:
    void inputPathIndexChanged(int index);

public slots:
    void commit(const QVariant& value);
    void connectPathAtIndex(int index);

private:
    QStringList createPath(const QmlBindingPath::Ptr& bp) const;

    QHash<int, QByteArray> m_roles;
    QmlEditFragment*       m_edit;
};

inline QHash<int, QByteArray> QmlBindingSpanModel::roleNames() const{
    return m_roles;
}

}// namespace

#endif // LVQMLBINDINGSPANMODEL_H
