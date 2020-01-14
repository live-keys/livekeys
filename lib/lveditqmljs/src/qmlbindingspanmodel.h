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
    Q_PROPERTY(bool isScanning    READ isScanning     NOTIFY isScanningChanged)

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

    bool isScanning() const;

    void initializeScanner(CodeQmlHandler* qmlHandler);

signals:
    void inputPathIndexChanged(int index);
    void pathConnectionChanged(int index, bool connection);
    void isScanningChanged();

public slots:
    void makePathInput(int index);
    void setPathConnection(int index, bool connection);

    void __scannerBindingPathAdded();

private:
    QStringList createPath(const QmlBindingPath::Ptr& bp) const;

    QHash<int, QByteArray> m_roles;
    QmlEditFragment*       m_edit;
    QmlUsageGraphScanner*  m_scanner;
    bool                   m_isScanning;
};

inline QHash<int, QByteArray> QmlBindingSpanModel::roleNames() const{
    return m_roles;
}

}// namespace

#endif // LVQMLBINDINGSPANMODEL_H
