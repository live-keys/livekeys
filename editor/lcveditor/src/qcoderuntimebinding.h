#ifndef QCODERUNTIMEBINDING_H
#define QCODERUNTIMEBINDING_H

#include "qlcveditorglobal.h"
#include "qcodedeclaration.h"
#include <QObject>

namespace lcv{

class Q_LCVEDITOR_EXPORT QCodeRuntimeBinding : public QObject{

    Q_OBJECT

    friend class QProjectDocumentBlockData;

public:
    QCodeRuntimeBinding(const QStringList &identifierChain, QProjectDocument* parent = 0);
    QCodeRuntimeBinding(QCodeDeclaration* declaration);
    ~QCodeRuntimeBinding();

    void setConverter(QCodeConverter* converter);

    QCodeDeclaration* declaration();
    const QCodeDeclaration* declaration() const;

    int position() const;
    int length() const;

    bool isModifiedByEngine() const;

    QProjectDocumentBlockData* parentBlock();

public slots:
    void updateValue();

public:
    QCodeDeclaration*          m_declaration;
    QProjectDocumentBlockData* m_parentBlock;
    QCodeConverter*            m_converter;
    bool                       m_modifiedByEngine;
};

inline void QCodeRuntimeBinding::setConverter(QCodeConverter *converter){
    m_converter = converter;
}

inline QCodeDeclaration *QCodeRuntimeBinding::declaration(){
    return m_declaration;
}

inline const QCodeDeclaration *QCodeRuntimeBinding::declaration() const{
    return m_declaration;
}

inline bool QCodeRuntimeBinding::isModifiedByEngine() const{
    return m_modifiedByEngine;
}

inline QProjectDocumentBlockData *QCodeRuntimeBinding::parentBlock(){
    return m_parentBlock;
}

}// namespace

#endif // QCODERUNTIMEBINDING_H
