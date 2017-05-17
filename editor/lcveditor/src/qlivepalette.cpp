#include "qlivepalette.h"

namespace lcv{

QLivePalette::QLivePalette(QObject *parent)
    : QCodeConverter(parent)
    , m_item(0)
    , m_codeChange(false)
{
}

QLivePalette::~QLivePalette(){
}

void QLivePalette::initPallete(const QVariant &value){
    m_codeChange = true;
    m_value = value;
    emit init(value);
    m_codeChange = false;
}

void QLivePalette::setValueFromCode(const QVariant &value){
    m_codeChange = true;
    m_value = value;
    emit init(value);
    m_codeChange = false;
}

}// namespace
