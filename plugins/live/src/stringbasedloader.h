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

#ifndef STRINGBASEDLOADER_H
#define STRINGBASEDLOADER_H

#include <QQuickItem>

namespace lv{

/// \private
class StringBasedLoader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE  setSource NOTIFY sourceChanged)
    Q_PROPERTY(QObject* item  READ item   NOTIFY itemChanged)

public:
    explicit StringBasedLoader(QQuickItem *parent = nullptr);
    ~StringBasedLoader();

    const QString &source() const;
    void setSource(const QString& source);
    QObject* item() const;

signals:
    void sourceChanged();
    void itemChanged();

private:
    void createObject();

    QString        m_source;
    QQmlComponent* m_sourceComponent;
    QObject*       m_item;
};

inline const QString& StringBasedLoader::source() const{
    return m_source;
}

inline QObject *StringBasedLoader::item() const{
    return m_item;
}

}// namespace

#endif // STRINGBASEDLOADER_H
