#ifndef QDOCUMENTQMLFRAGMENT_H
#define QDOCUMENTQMLFRAGMENT_H

#include "qqmljsparserglobal.h"
#include "qdocumenteditfragment.h"

#include <QQmlProperty>
#include <QQuickItem>

namespace lcv{

class Q_QMLJSPARSER_EXPORT QDocumentQmlFragment : public QDocumentEditFragment{

public:
    QDocumentQmlFragment(
        int position,
        int length,
        QCodeConverter* converter,
        const QQmlProperty& property
    );
    ~QDocumentQmlFragment();

    void commit(const QVariant &value) Q_DECL_OVERRIDE;

private:
    QQmlProperty m_property;
};

inline void QDocumentQmlFragment::commit(const QVariant &value){
    m_property.write(value);
}

}// namespace

#endif // QDOCUMENTQMLFRAGMENT_H
