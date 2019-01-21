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

#ifndef LVCODEPALLETE_H
#define LVCODEPALLETE_H

#include <QObject>
#include <QQuickItem>

#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT CodePalette : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQuickItem* item    READ item       WRITE setItem       NOTIFY itemChanged)
    Q_PROPERTY(QString name        READ name       CONSTANT)
    Q_PROPERTY(QString type        READ type       WRITE setType       NOTIFY typeChanged)
    Q_PROPERTY(QVariant value      READ value      WRITE setValue      NOTIFY valueChanged)
    Q_PROPERTY(QObject* extension  READ extension  NOTIFY extensionChanged)

public:
    explicit CodePalette(QObject *parent = 0);
    virtual ~CodePalette();

    QQuickItem* item();
    void setItem(QQuickItem* item);

    const QVariant &value() const;
    void setValue(const QVariant& value);
    void setValueFromBinding(const QVariant &value);

    QString name() const;

    void setPath(const QString& path);
    const QString &path() const;

    QObject* extension() const;
    void setExtension(QObject* extension, bool own = false);

    QString type() const;
    void setType(QString type);

public slots:
    bool isBindingChange() const;

signals:
    void itemChanged();
    void valueChanged();
    void extensionChanged();
    void typeChanged();

    void init(const QVariant& value);
    void codeChanged(const QVariant& value);

private:
    Q_DISABLE_COPY(CodePalette)

    bool        m_bindingChange;
    QQuickItem* m_item;
    QVariant    m_value;
    QString     m_path;
    QObject*    m_extension;
    bool        m_ownExtension;
    QString     m_type;
};

/**
 * \brief Visual item for this palette
 */
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
 * \brief Property value corresponding to this palette
 */
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

/**
 * \brief Returns extension for palette
 */
inline QObject *CodePalette::extension() const{
    return m_extension;
}

/**
 * \brief Extension setter for palette
 */
inline void CodePalette::setExtension(QObject *extension, bool own){
    if (m_extension == extension)
        return;

    if ( m_ownExtension && m_extension )
        delete m_extension;

    m_extension = extension;
    m_ownExtension = own;
    emit extensionChanged();
}

/**
 * \brief Palette type
 */
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
