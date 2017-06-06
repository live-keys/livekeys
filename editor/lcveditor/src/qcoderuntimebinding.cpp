#include "qcoderuntimebinding.h"
#include "qprojectdocument.h"
#include "qcodeconverter.h"

#include <QDebug>

namespace lcv{

QCodeRuntimeBinding::QCodeRuntimeBinding(const QStringList& identifierChain, QProjectDocument *parent)
    : QObject(parent)
    , m_declaration(QCodeDeclaration::create(identifierChain, parent))
    , m_parentBlock(0)
    , m_converter(0)
    , m_modifiedByEngine(false)
{
}

QCodeRuntimeBinding::QCodeRuntimeBinding(QCodeDeclaration::Ptr declaration)
    : QObject(declaration->document())
    , m_declaration(declaration)
    , m_parentBlock(0)
    , m_converter(0)
    , m_modifiedByEngine(0)
{
}

QCodeRuntimeBinding::~QCodeRuntimeBinding(){
    if( m_parentBlock )
        m_parentBlock->removeBinding(this);
}

void QCodeRuntimeBinding::updateValue(){
    if ( m_converter ){
        declaration()->document()->updateBindingValue(
            this,
            m_converter->serialize()->toCode(sender()->property(declaration()->identifierChain().last().toUtf8()))
        );
        m_modifiedByEngine = true;
    }
}

int QCodeRuntimeBinding::position() const{
    return m_declaration->identifierPosition();
}

int QCodeRuntimeBinding::length() const{
    return m_declaration->length();
}

}// namespace
