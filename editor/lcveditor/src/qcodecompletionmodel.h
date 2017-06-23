/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QCODECOMPLETIONMODEL_H
#define QCODECOMPLETIONMODEL_H

#include <QAbstractListModel>

#include "qlcveditorglobal.h"
#include "qcodecompletionsuggestion.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QCodeCompletionContext{
public:
    virtual ~QCodeCompletionContext(){}
};

class Q_LCVEDITOR_EXPORT QCodeCompletionModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(bool isEnabled          READ isEnabled          NOTIFY isEnabledChanged)
    Q_PROPERTY(int  completionPosition READ completionPosition NOTIFY completionPositionChanged)

public:
    enum Roles{
        Label = Qt::UserRole + 1,
        Info,
        Category,
        Completion,
        Documentation
    };

public:
    QCodeCompletionModel(QObject* parent = 0);
    ~QCodeCompletionModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    int completionPosition() const;
    bool isEnabled() const;

    void resetModel();

    void setSuggestions(const QList<QCodeCompletionSuggestion>& suggestions, const QString& suggestionFilter);
    void setCompletionPosition(int index);

    void setFilter(const QString& filter);
    const QString& filter() const;

    QCodeCompletionContext* completionContext();
    void setCompletionContext(QCodeCompletionContext* context);
    void removeCompletionContext();

public slots:
    void enable();
    void disable();

signals:
    void completionPositionChanged(int arg);
    void isEnabledChanged(bool arg);

private:
    void updateFilters();

    QList<QCodeCompletionSuggestion> m_suggestions;
    QString                         m_filter;
    QList<int>                      m_filteredSuggestions;
    QCodeCompletionContext*         m_completionContext;
    QHash<int, QByteArray>          m_roles;
    int                             m_completionPosition;
    bool                            m_isEnabled;
};

inline QHash<int, QByteArray> QCodeCompletionModel::roleNames() const{
    return m_roles;
}

inline int QCodeCompletionModel::completionPosition() const{
    return m_completionPosition;
}

inline bool QCodeCompletionModel::isEnabled() const{
    return m_isEnabled;
}

inline void QCodeCompletionModel::setCompletionPosition(int index){
    if ( index != m_completionPosition ){
        m_completionPosition = index;
        emit completionPositionChanged(index);
    }
}

inline const QString &QCodeCompletionModel::filter() const{
    return m_filter;
}

inline void QCodeCompletionModel::enable(){
    if ( !m_isEnabled ){
        m_isEnabled = true;
        emit isEnabledChanged(m_isEnabled);
    }
}

inline void QCodeCompletionModel::disable(){
    if ( m_isEnabled ){
        m_isEnabled = false;
        emit isEnabledChanged(m_isEnabled);
    }
}

inline int QCodeCompletionModel::rowCount(const QModelIndex &) const{
    return m_filteredSuggestions.size();
}

inline QCodeCompletionContext *QCodeCompletionModel::completionContext(){
    return m_completionContext;
}

}// namespace

#endif // QCODECOMPLETIONMODEL_H
