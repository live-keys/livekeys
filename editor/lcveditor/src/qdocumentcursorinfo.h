#ifndef QDOCUMENTCURSORINFO_H
#define QDOCUMENTCURSORINFO_H

#include "qlcveditorglobal.h"
#include <QObject>

namespace lcv{

class Q_LCVEDITOR_EXPORT QDocumentCursorInfo: public QObject{

    Q_OBJECT
    Q_PROPERTY(bool canBind   READ canBind   CONSTANT)
    Q_PROPERTY(bool canUnbind READ canUnbind CONSTANT)
    Q_PROPERTY(bool canEdit   READ canEdit   CONSTANT)
    Q_PROPERTY(bool canAdjust READ canAdjust CONSTANT)

public:
    QDocumentCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, QObject* parent = 0);
    QDocumentCursorInfo(QObject* parent = 0);
    ~QDocumentCursorInfo();

    bool canBind() const;
    bool canUnbind() const;
    bool canEdit() const;
    bool canAdjust() const;

private:
    bool m_canBind;
    bool m_canUnbind;
    bool m_canEdit;
    bool m_canAdjust;
};

inline bool QDocumentCursorInfo::canBind() const{
    return m_canBind;
}

inline bool QDocumentCursorInfo::canUnbind() const{
    return m_canUnbind;
}

inline bool QDocumentCursorInfo::canEdit() const{
    return m_canEdit;
}

inline bool QDocumentCursorInfo::canAdjust() const{
    return m_canAdjust;
}

}// namespace

#endif // QDOCUMENTCURSORINFO_H
