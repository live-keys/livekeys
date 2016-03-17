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

#ifndef QMATSTATE_HPP
#define QMATSTATE_HPP

#include "qmat.h"
#include "qlcvglobal.h"

class Q_LCV_EXPORT QMatState{

public:
    QMat* mat;
    bool  linearFilter;
    mutable int   textureIndex;
    mutable bool  textureSync;

    QMatState();

    int compare(const QMatState *other) const;
};

#endif // QMATSTATE_HPP
