/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef LVVISUALLOGFILTER_H
#define LVVISUALLOGFILTER_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>
#include <QFutureWatcher>
#include <QString>

#include "live/lvviewglobal.h"
#include "live/visuallogbasemodel.h"
#include "live/applicationcontext.h"

class QJSEngine;

namespace lv{

class LV_VIEW_EXPORT VisualLogFilter : public VisualLogBaseModel, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::VisualLogBaseModel* source     READ source     WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString                 tag        READ tag        WRITE setTag    NOTIFY tagChanged)
    Q_PROPERTY(QJSValue                prefix     READ prefix     WRITE setPrefix NOTIFY prefixChanged)
    Q_PROPERTY(QJSValue                search     READ search     WRITE setSearch NOTIFY searchChanged)
    Q_PROPERTY(bool                    isIndexing READ isIndexing NOTIFY isIndexingChanged)

private:
    /// \private
    class SearchQuery{
    public:
        enum Type{
            None = 0,
            String,
            Regexp
        };
        /// \private
        union SearchContainer{

            QString* searchString;
            QRegExp* searchRegexp;

            SearchContainer() = default;
            SearchContainer(const QString& pSearchString) : searchString(new QString(pSearchString)){}
            SearchContainer(const QRegExp& pSearchRegexp) : searchRegexp(new QRegExp(pSearchRegexp)){}

            ~SearchContainer(){}
        };

    public:
        SearchQuery() : m_type(SearchQuery::None), m_container(0){}
        SearchQuery(const QJSValue& value, QJSEngine* engine);
        SearchQuery(const SearchQuery& other);
        ~SearchQuery();

        SearchQuery& operator = (const SearchQuery& other);
        bool operator == (const SearchQuery& other);

        QJSValue toJs(QJSEngine* engine) const;
        int locateIn(const QString& str);
        Type searchType() const{ return m_type; }

    private:
        Type             m_type;
        SearchContainer* m_container;
    };

public:
    explicit VisualLogFilter(QObject *parent = nullptr);
    ~VisualLogFilter();

    /**
     * \brief Base model which is the model we're applying the filter too.
     *
     * Interestingly, it can be both the main model, or even another filter model!
     * We could, in theory, have an array or filter models where each one is filtering on the previous one.
     */
    lv::VisualLogBaseModel* source() const;
    /** The tag we're filtering by */
    QString tag() const;
    /** Returns the query result of the search */
    QJSValue search() const;
    /** Indicator if the object is currently performing indexing in the background, asynchronously */
    bool isIndexing() const;

    void setSource(lv::VisualLogBaseModel* source);
    void setTag(QString tag);
    void setSearch(QJSValue search);

    /** Implementation of the respective QQmlParserStatus function */
    void classBegin(){}
    void componentComplete();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

    int totalEntries() const;
    QVariant entryDataAt(int index) const;
    QString entryPrefixAt(int index) const;
    const VisualLogEntry &entryAt(int index) const;

    void setPrefix(QJSValue prefix);
    void setIsIndexing(bool isIndexing);


    /** The prefix we're filtering by */
    QJSValue prefix() const;

public slots:
    void refilterReady();

    void sourceDestroyed();
    void sourceModelReset();
    void sourceModelAboutToReset();
    void sourceRowsAboutToBeRemoved(const QModelIndex&, int from, int to);
    void sourceRowsInserted(const QModelIndex&, int from, int to);

signals:
    /** Source has changed */
    void sourceChanged();
    /** Tag has changed */
    void tagChanged();
    /** Prefix has changed */
    void prefixChanged();
    /** Search string has changed */
    void searchChanged();
    /** Indexing indicator has changed */
    void isIndexingChanged();

private:
    void refilter();
    bool filterEntry(const VisualLogEntry& entry);

    lv::VisualLogBaseModel* m_source;
    QString                 m_tag;
    SearchQuery             m_prefix;
    SearchQuery             m_search;
    bool                    m_componentReady;
    bool                    m_isIndexing;

    QList<int>              m_entries;

    QFutureWatcher<QList<int> > m_workerWatcher;
    bool                        m_workerIgnoreResult;
};


inline VisualLogBaseModel *VisualLogFilter::source() const{
    return m_source;
}

inline QString VisualLogFilter::tag() const{
    return m_tag;
}

inline bool VisualLogFilter::isIndexing() const{
    return m_isIndexing;
}

/** Tag setter */
inline void VisualLogFilter::setTag(QString tag){
    if (m_tag == tag)
        return;

    m_tag = tag;
    emit tagChanged();

    refilter();
}

/** Total number of entries */
inline int VisualLogFilter::totalEntries() const{
    return m_entries.size();
}


}// namespace

#endif // LVVISUALLOGFILTER_H
