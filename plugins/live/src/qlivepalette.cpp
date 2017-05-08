#include "qlivepalette.h"

namespace lcv{

QLivePalette::QLivePalette(QObject *parent)
    : QCodeConverter(parent)
    , m_item(0)
{
}

QLivePalette::~QLivePalette(){
}

void QLivePalette::initPallete(const QString &){
    //TODO
}

}// namespace
