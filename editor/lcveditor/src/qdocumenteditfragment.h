#ifndef QDOCUMENTEDITFRAGMENT_H
#define QDOCUMENTEDITFRAGMENT_H

#include "qlcveditorglobal.h"
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
    QDocumentEditFragment(int position, int length, QCodeConverter* converter = 0);
    virtual ~QDocumentEditFragment();

    int position() const;
    int length() const;
    QCodeConverter* converter();

    void setActionType(ActionType action);
    ActionType actionType() const;

    void updateLength(int length);

    virtual void commit(const QVariant&){}

    void setRuntimeBinding(QCodeRuntimeBinding* runtimeBinding);
    QCodeRuntimeBinding* runtimeBinding();

private:
    int m_position;
    int m_length;
    QCodeConverter*          m_converter;
    QCodeRuntimeBinding* m_binding;

    ActionType m_actionType;
};

inline int QDocumentEditFragment::position() const{
    return m_position;
}

inline int QDocumentEditFragment::length() const{
    return m_length;
}

inline QCodeConverter *QDocumentEditFragment::converter(){
    return m_converter;
}

inline void QDocumentEditFragment::setActionType(QDocumentEditFragment::ActionType action){
    m_actionType = action;
}

inline void QDocumentEditFragment::updateLength(int length){
    m_length = length;
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

}// namespace

#endif // QDOCUMENTEDITFRAGMENT_H
