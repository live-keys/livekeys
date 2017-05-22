#ifndef QCODEDECLARATION_H
#define QCODEDECLARATION_H

#include "qlcveditorglobal.h"
#include "qprojectdocument.h"

namespace lcv{

class QProjectDocument;

class Q_LCVEDITOR_EXPORT QCodeDeclaration{

public:
    QCodeDeclaration(const QStringList& identifierChain, QProjectDocument* document = 0);
    QCodeDeclaration(
            const QStringList &identifierChain,
            const QString& type,
            const QStringList& parentType,
            QProjectDocument* document = 0
    );
    QCodeDeclaration(
            const QStringList &identifierChain,
            const QString& type,
            const QStringList& parentType,
            int identifierPosition,
            int identifierLength,
            QProjectDocument* document = 0
    );
    ~QCodeDeclaration();

    void setIdentifierPosition(int position);
    int identifierPosition() const;

    void setIdentifierLength(int length);
    int identifierLength() const;

    int position() const;
    int valuePosition() const;

    const QStringList& identifierChain() const;
    const QString& type() const;
    const QStringList& parentType() const;

    QProjectDocument* document();

    int length() const;

    int valueOffset() const;
    void setValuePositionOffset(int positionOffset);

    int valueLength() const;
    void setValueLength(int valueLength);

private:
    Q_DISABLE_COPY(QCodeDeclaration)

    int         m_identifierPosition;
    int         m_identifierLength;
    QStringList m_identifierChain;
    QString     m_type;
    QStringList m_parentType;


    int m_valueOffset;
    int m_valueLength;

    QProjectDocument* m_document;
};

inline void QCodeDeclaration::setIdentifierPosition(int position){
    m_identifierPosition = position;
}

inline int QCodeDeclaration::identifierPosition() const{
    return m_identifierPosition;
}

inline void QCodeDeclaration::setIdentifierLength(int length){
    m_identifierLength = length;
}

inline int QCodeDeclaration::identifierLength() const{
    return m_identifierLength;
}

inline int QCodeDeclaration::position() const{
    return identifierPosition();
}

inline int QCodeDeclaration::valuePosition() const{
    if ( valueLength() == 0 || valueOffset() == -1 )
        return -1;
    return identifierPosition() + identifierLength() + valueOffset();
}

inline const QStringList &QCodeDeclaration::identifierChain() const{
    return m_identifierChain;
}

inline const QString &QCodeDeclaration::type() const{
    return m_type;
}

inline const QStringList &QCodeDeclaration::parentType() const{
    return m_parentType;
}

inline QProjectDocument *QCodeDeclaration::document(){
    return m_document;
}

inline int QCodeDeclaration::length() const{
    return m_identifierLength + m_valueOffset + m_valueLength;
}

inline int QCodeDeclaration::valueOffset() const{
    return m_valueOffset;
}

inline void QCodeDeclaration::setValuePositionOffset(int positionOffset){
    m_valueOffset = positionOffset;
}

inline int QCodeDeclaration::valueLength() const{
    return m_valueLength;
}

inline void QCodeDeclaration::setValueLength(int valueLength){
    m_valueLength = valueLength;
}

}// namespace

#endif // QCODEDECLARATION_H
