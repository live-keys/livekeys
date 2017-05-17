#include "qdocumentcursorinfo.h"

namespace lcv{

QDocumentCursorInfo::QDocumentCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, QObject *parent)
    : QObject(parent)
    , m_canBind(canBind)
    , m_canUnbind(canUnbind)
    , m_canEdit(canEdit)
    , m_canAdjust(canAdjust)
{
}

QDocumentCursorInfo::QDocumentCursorInfo(QObject *parent)
    : QObject(parent)
    , m_canBind(0)
    , m_canUnbind(0)
    , m_canEdit(0)
    , m_canAdjust(0)
{
}

QDocumentCursorInfo::~QDocumentCursorInfo(){
}


}// namespace
