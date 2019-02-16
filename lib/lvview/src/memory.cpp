#include "memory.h"

namespace lv{

Memory::Memory(QObject *parent)
    : QObject(parent)
{
}

Memory::~Memory(){
}

Shared *Memory::reloc(Shared *sd){
    return sd->reloc();
}

}// namespace
