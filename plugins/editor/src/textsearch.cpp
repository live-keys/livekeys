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

    qDebug() << lineIndexes;

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
