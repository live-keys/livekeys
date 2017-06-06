#ifndef QDOCUMENTEDITFRAGMENT_H
#define QDOCUMENTEDITFRAGMENT_H

#include "qlcveditorglobal.h"
#include "qcodedeclaration.h"
#include <QVariant>

namespace lcv{

class QCodeConverter;
class QCodeRuntimeBinding;
class Q_LCVEDITOR_EXPORT QDocumentEditFragment{

public:
    enum ActionType{
        Edit,
        Adjust
    };

public:
    QDocumentEditFragment(QCodeDeclaration::Ptr declaration, QCodeConverter* converter = 0);
    virtual ~QDocumentEditFragment();

    int valuePosition() const;
    int valueLength() const;
    QCodeConverter* converter();

    void setActionType(ActionType action);
    ActionType actionType() const;

    virtual void commit(const QVariant&){}

    QCodeDeclaration::Ptr declaration();

    void setRuntimeBinding(QCodeRuntimeBinding* runtimeBinding);
    QCodeRuntimeBinding* runtimeBinding();

private:
    QCodeDeclaration::Ptr m_declaration;
    QCodeConverter*       m_converter;
    QCodeRuntimeBinding*  m_binding;

    ActionType m_actionType;
};

inline QCodeConverter *QDocumentEditFragment::converter(){
    return m_converter;
}

inline void QDocumentEditFragment::setActionType(QDocumentEditFragment::ActionType action){
    m_actionType = action;
}

inline void QDocumentEditFragment::setRuntimeBinding(QCodeRuntimeBinding *binding){
    m_binding = binding;
}

inline QCodeRuntimeBinding *QDocumentEditFragment::runtimeBinding(){
    return m_binding;
}

inline QDocumentEditFragment::ActionType QDocumentEditFragment::actionType() const{
    return m_actionType;
}

inline QCodeDeclaration::Ptr QDocumentEditFragment::declaration(){
    return m_declaration;
}

}// namespace

#endif // QDOCUMENTEDITFRAGMENT_H
