#ifndef QMLIMPORTSMODEL_H
#define QMLIMPORTSMODEL_H

#include <QObject>
#include <QJSValue>
#include <QAbstractListModel>
#include "lveditqmljsglobal.h"

namespace lv{

class ViewEngine;
class LV_EDITQMLJS_EXPORT QmlImportsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    /// \private
    class ItemData{
    public:
        ItemData(const QString& m, const QString& v, const QString& q, int l);

        QString module;
        QString version;
        QString qualifier;
        int line;
    };

    enum Roles{
        Module = Qt::UserRole + 1,
        Version,
        Qualifier,
        Line
    };

public:
    QmlImportsModel(ViewEngine* engine, QObject* parent = nullptr);
    ~QmlImportsModel(){}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void addItem(const QString& m, const QString& v, const QString& q, int l);

public slots:
    void commit(QString m, QString v, QString q);
    void erase(int pos);

    int count();
    QJSValue getImportAtUri(const QString& uri);

    int firstBlock();
    int lastBlock();

signals:
    void countChanged();

private:
    ViewEngine*            m_engine;
    QList<ItemData>        m_data;
    QHash<int, QByteArray> m_roles;

};

inline int QmlImportsModel::rowCount(const QModelIndex &) const{
    return m_data.size();
}

inline QHash<int, QByteArray> QmlImportsModel::roleNames() const{
    return m_roles;
}

inline int QmlImportsModel::count(){
    return m_data.size();
}

}

#endif // QMLIMPORTSMODEL_H
