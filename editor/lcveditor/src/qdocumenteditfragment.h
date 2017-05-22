#ifndef QDOCUMENTEDITFRAGMENT_H
#define QDOCUMENTEDITFRAGMENT_H

#include "qlcveditorglobal.h"
#include <QVariant>

namespace lcv{

class QCodeConverter;
class QCodeDeclaration;
class QCodeRuntimeBinding;
class Q_LCVEDITOR_EXPORT QDocumentEditFragment{

public:
    enum ActionType{
        Edit,
        Adjust
    };

public:
    QDocumentEditFragment(QCodeDeclaration* declaration, QCodeConverter* converter = 0);
    virtual ~QDocumentEditFragment();

    int valuePosition() const;
    int valueLength() const;
    QCodeConverter* converter();

    void setActionType(ActionType action);
    ActionType actionType() const;

    virtual void commit(const QVariant&){}

    QCodeDeclaration* declaration();

    void setRuntimeBinding(QCodeRuntimeBinding* runtimeBinding);
    QCodeRuntimeBinding* runtimeBinding();

private:
    QCodeDeclaration* m_declaration;
    QCodeConverter* m_converter;
    QCodeRuntimeBinding* m_binding;

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

inline QCodeDeclaration *QDocumentEditFragment::declaration(){
    return m_declaration;
}

}// namespace

#endif // QDOCUMENTEDITFRAGMENT_H
