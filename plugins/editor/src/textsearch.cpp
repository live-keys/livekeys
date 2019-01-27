#include "textsearch.h"
#include "live/visuallogqt.h"

#include <QDebug>

namespace lv{

TextSearch::TextSearch(QObject *parent)
    : QObject(parent)
{
}

TextSearch::~TextSearch(){
}

void TextSearch::process(){
    m_output.clear();

    if ( !m_value.isEmpty() ){
        int result = m_text.indexOf(m_value);
        while ( result != -1 ){
            m_output.append(result);
            result = m_text.indexOf(m_value, result + 1);
        }

        emit outputChanged();
    }
}

}// namespace
