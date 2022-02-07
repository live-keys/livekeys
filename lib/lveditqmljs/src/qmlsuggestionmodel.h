/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef QMLSUGGESTIONMODEL_H
#define QMLSUGGESTIONMODEL_H

#include <QAbstractListModel>
#include "qmllanguageinfo.h"
#include "qmlscopesnap_p.h"

namespace lv{

/// \private
class QmlSuggestionModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int addPosition READ addPosition CONSTANT)
public:

    /// \private
    class ItemData{
    public:
        enum Category {
            Property = 1,
            Object,
            Event,
            Function
        };

        ItemData(
            const QString& plabel,
            const QString& pObjType,
            const QString& ptype,
            const QString& pimport,
            const QString& pdoc,
            const QString& pcode,
            const int cat,
            bool group = false,
            bool writable = false
        );

        QString label;
        QString objectType;
        QString type;
        QString importSpace;
        QString documentation;
        QString code;
        int category;
        bool isGroup;
        bool isWritable;
    };

    enum Roles{
        Label = Qt::UserRole + 1,
        ObjectType,
        Type,
        ImportSpace,
        Documentation,
        Code,
        Category,
        IsGroup,
        IsWritable
    };

    QmlSuggestionModel(int addPosition, QObject* parent = 0);
    ~QmlSuggestionModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

    void addItem(const ItemData& item);
    int addPosition() const;

    void updateFilters();
    void addPropertiesAndFunctionsToModel(const QmlInheritanceInfo& typePath);
    void addObjectsToModel(const QmlScopeSnap& scope, bool addCreatable = true, bool addSingletons = true);

public slots:
    void setFilter(const QString& filter);
    void setImportFilter(const QString& importFilter);
    void setTypeFilter(const QString& typeFilter);
    void setCategoryFilter(int cat);

    QStringList importSpaces() const;
    QStringList types() const;

    bool supportsObjectNesting() const { return m_supportsObjectNesting; }
private:
    QList<ItemData>         m_data;
    QString                 m_filter;
    QString                 m_importFilter;
    QString                 m_typeFilter;
    int                     m_categoryFilter;
    bool                    m_supportsObjectNesting;
    QList<int>              m_filteredData;
    QHash<int, QByteArray>  m_roles;
    int                     m_addPosition;
};

inline QHash<int, QByteArray> QmlSuggestionModel::roleNames() const{
    return m_roles;
}

inline int QmlSuggestionModel::addPosition() const{
    return m_addPosition;
}

inline const QString &QmlSuggestionModel::filter() const{
    return m_filter;
}

inline int QmlSuggestionModel::rowCount(const QModelIndex &) const{
    return m_filteredData.size();
}

}
#endif // QMLSUGGESTIONMODEL_H
