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

#include "live/abstractcodehandler.h"

/**
 * \class lv::AbstractCodeHandler
 * \brief Base class for a code handler, managing assisted completion and custom highlighting
 *
 * \ingroup lveditor
 */
namespace lv{

/** \brief Blank constructor of AbstractCodeHandler */
AbstractCodeHandler::AbstractCodeHandler(QObject *parent)
    : QObject(parent)
{
}

/** \brief Blank desctructor of AbstractCodeHandler */
AbstractCodeHandler::~AbstractCodeHandler(){
}


/**
 * \fn lv::AbstractCodeHandler::assistCompletion
 * \brief Assists code completion when typing
 */

/**
 * \fn lv::AbstractCodeHandler::contextBlock
 * \brief Returns borders of the block containing the current position
 */

}// namespace
