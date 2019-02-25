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

#ifndef QMATSTATE_H
#define QMATSTATE_H

#include "qlcvcoreglobal.h"
#include "qmat.h"

class Q_LCVCORE_EXPORT QMatState{

public:
    /**
    *\brief Matrix for which the state is for.
    */
    QMat* mat;
    /**
    *\brief Linear filtering flag.
    */
    bool  linearFilter;
    /**
    *\brief Stored index of the texture to be displayed.
    */
    mutable int   textureIndex;
    /**
    *\brief Stores wether the texture has been loaded (synced with opengl).
    */
    mutable bool  textureSync;

    QMatState();

    int compare(const QMatState *other) const;
};

#endif // QMATSTATE_H
