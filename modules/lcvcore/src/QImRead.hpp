/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#ifndef QMATSOURCE_HPP
#define QMATSOURCE_HPP

#include <QQuickItem>
#include "QMat.hpp"
#include "QMatDisplay.hpp"

class QImRead : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)

public:
    explicit QImRead(QQuickItem *parent = 0);
    ~QImRead();

    const QString& file() const;
    void setFile(const QString& file);
    
signals:
    void fileChanged();

private:
    QString m_file;
    
};

inline const QString &QImRead::file() const{
    return m_file;
}

#endif // QMATSOURCE_HPP
