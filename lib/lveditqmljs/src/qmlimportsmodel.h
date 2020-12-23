#ifndef QMLIMPORTSMODEL_H
#define QMLIMPORTSMODEL_H

#include <QObject>
#include <QJSValue>
#include <QAbstractListModel>
#include "live/documentqmlinfo.h"
#include "lveditqmljsglobal.h"

namespace lv{

class ViewEngine;
class LV_EDITQMLJS_EXPORT QmlImportsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles{
        Module = Qt::UserRole + 1,
        Version,
        Qualifier,
        Line
    };

    // Declaration -> position -> guaranteed to update live

public:
    QmlImportsModel(ViewEngine* engine, QObject* parent = nullptr);
    ~QmlImportsModel(){}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void addItem(const QString& m, const QString& v, const QString& q, int l);

    DocumentQmlInfo::Import createItem(const QString& m, const QString& v, const QString& q, int l);

    void setImports(const QList<DocumentQmlInfo::Import>& imports);

public slots:
    void commit(QString m, QString v, QString q);
    void erase(int line);

    int count();
    QJSValue getImportAtUri(const QString& uri);

    int firstBlock();
    int lastBlock();

signals:
    void countChanged();

private:
    ViewEngine*                    m_engine;
    QList<DocumentQmlInfo::Import> m_data;
    QHash<int, QByteArray>         m_roles;

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
