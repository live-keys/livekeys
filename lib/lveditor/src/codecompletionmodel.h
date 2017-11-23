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

#ifndef LVCODECOMPLETIONMODEL_H
#define LVCODECOMPLETIONMODEL_H

#include <QAbstractListModel>

#include "live/lveditorglobal.h"
#include "live/codecompletionsuggestion.h"

namespace lv{

class LV_EDITOR_EXPORT CodeCompletionContext{
public:
    virtual ~CodeCompletionContext(){}
};

class LV_EDITOR_EXPORT CodeCompletionModel : public QAbstractListModel{

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
    CodeCompletionModel(QObject* parent = 0);
    ~CodeCompletionModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    int completionPosition() const;
    bool isEnabled() const;

    void resetModel();

    void setSuggestions(const QList<CodeCompletionSuggestion>& suggestions, const QString& suggestionFilter);
    void setCompletionPosition(int index);

    void setFilter(const QString& filter);
    const QString& filter() const;

    CodeCompletionContext* completionContext();
    void setCompletionContext(CodeCompletionContext* context);
    void removeCompletionContext();

public slots:
    void enable();
    void disable();

signals:
    void completionPositionChanged(int arg);
    void isEnabledChanged(bool arg);

private:
    void updateFilters();

    QList<CodeCompletionSuggestion> m_suggestions;
    QString                         m_filter;
    QList<int>                      m_filteredSuggestions;
    CodeCompletionContext*         m_completionContext;
    QHash<int, QByteArray>          m_roles;
    int                             m_completionPosition;
    bool                            m_isEnabled;
};

inline QHash<int, QByteArray> CodeCompletionModel::roleNames() const{
    return m_roles;
}

inline int CodeCompletionModel::completionPosition() const{
    return m_completionPosition;
}

inline bool CodeCompletionModel::isEnabled() const{
    return m_isEnabled;
}

inline void CodeCompletionModel::setCompletionPosition(int index){
    if ( index != m_completionPosition ){
        m_completionPosition = index;
        emit completionPositionChanged(index);
    }
}

inline const QString &CodeCompletionModel::filter() const{
    return m_filter;
}

inline void CodeCompletionModel::enable(){
    if ( !m_isEnabled ){
        m_isEnabled = true;
        emit isEnabledChanged(m_isEnabled);
    }
}

inline void CodeCompletionModel::disable(){
    if ( m_isEnabled ){
        m_isEnabled = false;
        emit isEnabledChanged(m_isEnabled);
    }
}

inline int CodeCompletionModel::rowCount(const QModelIndex &) const{
    return m_filteredSuggestions.size();
}

inline CodeCompletionContext *CodeCompletionModel::completionContext(){
    return m_completionContext;
}

}// namespace

#endif // LVCODECOMPLETIONMODEL_H
