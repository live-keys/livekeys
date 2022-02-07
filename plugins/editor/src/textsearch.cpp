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

#include "textsearch.h"
#include "live/visuallogqt.h"

#include <QDebug>

namespace lv{

TextSearch::TextSearch(QObject *parent)
    : QObject(parent)
    , m_asLines(false)
{
}

TextSearch::~TextSearch(){
}

void TextSearch::process(){
    if ( (m_text.isEmpty() || m_value.isEmpty()) && m_output.isEmpty() )
        return;

    m_output.clear();

    QList<int> lineIndexes;
    if ( m_asLines ){
        lineIndexes.append(0);

        int result = m_text.indexOf('\n');
        while (result != -1 ){
            lineIndexes.append(result);
            result = m_text.indexOf('\n', result + 1);
        }
    }

    if ( !m_value.isEmpty() ){
        int result = m_text.indexOf(m_value);

        while ( result != -1 ){
            if ( m_asLines ){
                bool lineFound = false;
                for ( int i = 0; i < lineIndexes.size(); ++i ){
                    if ( lineIndexes[i] > result ){
                        m_output.append(i - 1);
                        lineFound = true;
                        break;
                    }
                }
                if ( !lineFound ){
                    m_output.append(lineIndexes.size() - 1);
                }
            } else {
                m_output.append(result);
            }

            result = m_text.indexOf(m_value, result + 1);
        }

        emit outputChanged();
    }
}

}// namespace
