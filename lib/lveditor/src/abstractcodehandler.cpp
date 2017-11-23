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

#include "live/abstractcodehandler.h"

/*!
  \class lv::AbstractCodeHandler
  \inmodule lveditor_cpp

  Code handler interface for file types.
 */
namespace lv{

/*!

 */
AbstractCodeHandler::AbstractCodeHandler(QObject *parent)
    : QObject(parent)
{
}

AbstractCodeHandler::~AbstractCodeHandler(){

}

}// namespace
