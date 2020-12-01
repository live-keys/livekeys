/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVVISUALLOGMODEL_H
#define LVVISUALLOGMODEL_H

#include <QString>
#include <QColor>
#include <QAbstractListModel>
#include <QJSValue>

#include "live/lvviewglobal.h"
#include "live/visuallog.h"
#include "live/visuallogbasemodel.h"

class QQmlEngine;

//TODO: Manage caching for components

namespace lv{

class LV_VIEW_EXPORT VisualLogModel : public VisualLogBaseModel, public VisualLog::ViewTransport{

    Q_OBJECT
    Q_PROPERTY(int      width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QJSValue style READ style WRITE setStyle NOTIFY styleChanged)

public:
    class TextStyle{
    public:
        QColor     infoColor;
        QColor     warnColor;
        QColor     errorColor;
        QByteArray font;
        int        fontSize;
    };

public:
    VisualLogModel(QQmlEngine* engine);
    ~VisualLogModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    void onMessage(
        const VisualLog::Configuration* configuration,
        const VisualLog::MessageInfo& messageInfo,
        const std::string& message
    ) Q_DECL_OVERRIDE;
    void onView(
        const VisualLog::Configuration* configuration,
        const VisualLog::MessageInfo& messageInfo,
        const std::string& viewName,
        const QVariant& value
    ) Q_DECL_OVERRIDE;

    /** Returns the width */
    int width() const;

    int totalEntries() const Q_DECL_OVERRIDE;
    QVariant entryDataAt(int index) const Q_DECL_OVERRIDE;
    QString entryPrefixAt(int index) const Q_DECL_OVERRIDE;
    const VisualLogEntry &entryAt(int index) const Q_DECL_OVERRIDE;

    QList<VisualLogEntry>::Iterator begin();
    QList<VisualLogEntry>::Iterator end();
    QList<VisualLogEntry>::ConstIterator begin() const;
    QList<VisualLogEntry>::ConstIterator end() const;

    QJSValue style() const;

public slots:
    QString messageAt(int index) const;
    QString locationAt(int index) const;
    void setWidth(int width);
    void clearValues();

    void setStyle(QJSValue style);

signals:
    /** Width has changed */
    void widthChanged(int width);
    /** Entry was added */
    void entryAdded();
    /** Style changed */
    void styleChanged();

private:
    void resetTextComponent();
    QQmlComponent* component(const QString& key);
    QString componentPath(const QString& componentKey);

    TextStyle                      m_textStyle;

    QQmlEngine*                    m_engine;
    QList<VisualLogEntry>          m_entries;
    QQmlComponent*                 m_textComponent;
    QHash<QString, QQmlComponent*> m_components;
    int                            m_width;
};

/** Implementation of the respective QAbstractListModel function */
inline int VisualLogModel::rowCount(const QModelIndex &) const{
    return m_entries.size();
}
inline int VisualLogModel::width() const{
    return m_width;
}

/** Sets the width */
inline void VisualLogModel::setWidth(int width){
    if (m_width == width)
        return;

    m_width = width;
    emit widthChanged(width);
}

/** Implementation of the respective VisualLogBaseModel function */
inline int VisualLogModel::totalEntries() const{
    return m_entries.size();
}

/** Begin-iterator of the log entries */
inline QList<VisualLogEntry>::Iterator VisualLogModel::begin(){
    return m_entries.begin();
}

/** End-iterator of the log entries */
inline QList<VisualLogEntry>::Iterator VisualLogModel::end(){
    return m_entries.end();
}

/** Const begin-iterator of the log entries */
inline QList<VisualLogEntry>::ConstIterator VisualLogModel::begin() const{
    return m_entries.begin();
}

/** Const end-iterator of the log entries */
inline QList<VisualLogEntry>::ConstIterator VisualLogModel::end() const{
    return m_entries.end();
}

}// namespace

#endif // VISUALLOGMODEL_H
