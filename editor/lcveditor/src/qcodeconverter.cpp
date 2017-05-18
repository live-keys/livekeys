#include "qcodeconverter.h"

namespace lcv{

QCodeConverter::QCodeConverter(QObject *parent)
    : QObject(parent)
    , m_serialize(0)
{

}

QCodeConverter::~QCodeConverter(){
}

}// namespace
