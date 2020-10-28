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

#ifndef LVCODEPALLETE_H
#define LVCODEPALLETE_H

#include <QObject>
#include <QQuickItem>

#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT CodePalette : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQuickItem* item         READ item           WRITE setItem       NOTIFY itemChanged)
    Q_PROPERTY(QString type             READ type           WRITE setType       NOTIFY typeChanged)
    Q_PROPERTY(QVariant value           READ value          WRITE setValue      NOTIFY valueChanged)
    Q_PROPERTY(QString name             READ name           CONSTANT)
    Q_PROPERTY(QObject* editFragment    READ editFragment   NOTIFY editFragmentChanged)

public:
    explicit CodePalette(QObject *parent = nullptr);
    virtual ~CodePalette();

    /** Visual item for this palette */
    QQuickItem* item();
    void setItem(QQuickItem* item);
    bool hasItem() const;

    /** Property value corresponding to this palette */
    const QVariant &value() const;
    void setValue(const QVariant& value);
    void setValueFromBinding(const QVariant &value);

    /** Palette name */
    QString name() const;

    void setPath(const QString& path);
    const QString &path() const;

    /** Returns extension for palette */
    QObject* editFragment() const;
    void setEditFragment(QObject* extension);

    /** Palette type */
    QString type() const;
    void setType(QString type);

public slots:
    bool isBindingChange() const;

signals:
    /** Item changed */
    void itemChanged();
    /** Value changed */
    void valueChanged();
    /** Edit fragment changed */
    void editFragmentChanged();
    /** Type changed */
    void typeChanged();

    /** Value was initialized */
    void init(const QVariant& value);
    /** Code changed */
    void codeChanged(const QVariant& value);


private:
    Q_DISABLE_COPY(CodePalette)

    bool        m_bindingChange;
    QQuickItem* m_item;
    QVariant    m_value;
    QString     m_path;
    QObject*    m_editFragment;
    QString     m_type;
};

inline QQuickItem *CodePalette::item(){
    return m_item;
}

/**
 * \brief Item setter
 */
inline void CodePalette::setItem(QQuickItem *item){
    if (m_item == item)
        return;

    m_item = item;
    emit itemChanged();
}

/**
 * \brief Checks wether item is available
 */
inline bool CodePalette::hasItem() const{
    return m_item ? true : false;
}

inline const QVariant& CodePalette::value() const{
    return m_value;
}

/**
 * \brief Value setter for palette
 */
inline void CodePalette::setValue(const QVariant &value){
    if ( (value.canConvert<QObject*>() || m_value != value) && !m_bindingChange ){
        m_value = value;
        emit valueChanged();
    }
}

/**
 * \brief Path setter for palette
 */
inline void CodePalette::setPath(const QString &path){
    m_path = path;
}

/**
 * \brief Path getter for palette
 */
inline const QString& CodePalette::path() const{
    return m_path;
}


inline QObject *CodePalette::editFragment() const{
    return m_editFragment;
}

/**
 * \brief Extension setter for palette
 */
inline void CodePalette::setEditFragment(QObject *editFragment){
    if (m_editFragment == editFragment)
        return;

    m_editFragment = editFragment;
    emit editFragmentChanged();
}

inline QString CodePalette::type() const{
    return m_type;
}

/**
 * \brief Type setter for palette
 */
inline void CodePalette::setType(QString type){
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

/**
 * \brief Shows if palette is currently in the middle of a binding change
 */
inline bool CodePalette::isBindingChange() const{
    return m_bindingChange;
}

}// namespace

#endif // LVCODEPALLETE_H
