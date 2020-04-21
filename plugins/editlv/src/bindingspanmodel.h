#ifndef LVBINDINGSPANMODEL_H
#define LVBINDINGSPANMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "live/elements/bindingpath.h"
#include "bindingspan.h"
//#include "qmlscopesnap_p.h" // TODO: ELEMENTS ProjectScope
#include "live/runnable.h"
#include "languagelvhandler.h"

namespace lv{

class QmlUsageGraphScanner;
class BindingSpanModel : public QAbstractListModel{

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
    explicit BindingSpanModel(LvEditFragment* edit, QObject *parent = nullptr);
    ~BindingSpanModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    int inputPathIndex() const;

    bool isScanning() const;

    void initializeScanner(LanguageLvHandler* lvHandler);

signals:
    void inputPathIndexChanged(int index);
    void pathConnectionChanged(int index, bool connection);
    void isScanningChanged();

public slots:
    void makePathInput(int index);
    void setPathConnection(int index, bool connection);

    void __scannerBindingPathAdded();

private:
    QStringList createPath(const el::BindingPath::Ptr& bp) const;

    QHash<int, QByteArray> m_roles;
    LvEditFragment*        m_edit;
    bool                   m_isScanning;
};

inline QHash<int, QByteArray> BindingSpanModel::roleNames() const{
    return m_roles;
}

}// namespace

#endif // LVQMLBINDINGSPANMODEL_H
