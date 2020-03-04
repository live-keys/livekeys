/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "live/qmleditfragment.h"
#include "live/qmldeclaration.h"
#include "live/codepalette.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "qmlcodeconverter.h"
#include "qmlbindingchannel.h"
#include "qmlbindingspan.h"
#include "qmlbindingspanmodel.h"

namespace lv{

/**
 * \class lv::QmlEditFragment
 * \ingroup lveditqmljs
 * \brief An editing fragment for a lv::ProjectDocument.
 *
 * An editing fragment represents a section within a lv::ProjectDocument that is connected to
 * the running application. Fragments have palettes associated with them, and can write code
 * based on the given value of a palette. They provide the set of binding channels connected
 * to the application.
 */

/**
 * \brief QmlEditFragment contructor
 *
 * The Fragment is constructed from a \p declaration object and a \p palette object.
 */
QmlEditFragment::QmlEditFragment(QmlDeclaration::Ptr declaration, QObject *parent)
    : QObject(parent)
    , m_declaration(declaration)
    , m_bindingPalette(nullptr)
    , m_bindingSpan(new QmlBindingSpan(this))
    , m_visualParent(nullptr)
    , m_bindingSpanModel(nullptr)
{
}

/**
 * \brief QmlEditFragment destructor
 */
QmlEditFragment::~QmlEditFragment(){
    ProjectDocumentSection::Ptr section = declaration()->section();
    ProjectDocument* doc = section->document();
    doc->removeSection(section);

    for ( auto it = childFragments().begin(); it != childFragments().end(); ++it ){
        QmlEditFragment* edit = *it;
        edit->deleteLater();
    }

    delete m_bindingSpanModel;
    delete m_bindingSpan;
}

void QmlEditFragment::setRelativeBinding(const QSharedPointer<QmlBindingPath> &bp){

    QmlBindingPath::Ptr bindingToParent = bp;

    QmlEditFragment* parent = parentFragment();
    if( parent && parent->parentFragment() ){
        bindingToParent = QmlBindingPath::join(parent->m_relativeBinding, bindingToParent);
    }

    m_relativeBinding = bindingToParent;

    QmlEditFragment* root = rootFragment();
    if ( !root )
        return;

    QList<QmlBindingChannel::Ptr> bc = root->bindingSpan()->outputChannels();
    for ( auto it = bc.begin(); it != bc.end(); ++it ){
        QmlBindingChannel::Ptr& bc = *it;
        if ( bc->isEnabled() ){
            QmlBindingPath::Ptr newbp = QmlBindingPath::join(bc->bindingPath(), m_relativeBinding, false);
            Runnable* r = bc->runnable();
            QmlBindingChannel::Ptr newbc = DocumentQmlInfo::traverseBindingPath(newbp, r);
            newbc->setEnabled(true);
            bindingSpan()->addOutputChannel(newbc);
        }
    }
}

/**
 * \brief Returns the lv::QmlDeclaration's value postion
 */
int QmlEditFragment::valuePosition() const{
    return m_declaration->valuePosition();
}


/**
 * \brief Returns the lv::QmlDeclaration's value length
 */
int QmlEditFragment::valueLength() const{
    return m_declaration->valueLength();
}

/**
 * \brief Returns true if this fragment edits an object
 */
bool QmlEditFragment::isForObject() const{
    return m_declaration->isForObject();
}

/**
 * \brief Returns true if this fragment edits a property
 */
bool QmlEditFragment::isForProperty() const{
    return m_declaration->isForProperty();
}

bool QmlEditFragment::isForSlot() const{
    return m_declaration->isForSlot();
}

CodePalette *QmlEditFragment::palette(const QString &type){
    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* current = *it;
        if ( current->type() == type )
            return current;
    }
    return nullptr;
}

void QmlEditFragment::addPalette(CodePalette *palette){
    m_palettes.append(palette);
}

void QmlEditFragment::removePalette(CodePalette *palette){
    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* cp = *it;
        if ( cp == palette ){
            emit aboutToRemovePalette(palette);
            m_palettes.erase(it);
            if ( bindingPalette() != palette )
                palette->deleteLater();
            break;
        }
    }

    if (m_palettes.size() == 0)
    {
        emit paletteListEmpty();
    }
}

int QmlEditFragment::totalPalettes() const{
    return m_palettes.size();
}

void QmlEditFragment::removeBindingPalette(){
    if ( !m_bindingPalette )
        return;

    if ( hasPalette(m_bindingPalette ) )
        m_bindingPalette = nullptr;
    else {
        m_bindingPalette->deleteLater();
        m_bindingPalette = nullptr;
    }
}

void QmlEditFragment::setBindingPalette(CodePalette *palette){
    removeBindingPalette();
    m_bindingPalette = palette;
}

void QmlEditFragment::addChildFragment(QmlEditFragment *edit){
    m_childFragments.append(edit);
}

void QmlEditFragment::removeChildFragment(QmlEditFragment *edit){
    for ( auto it = m_childFragments.begin(); it != m_childFragments.end(); ++it ){
        if ( *it == edit ){
            edit->emitRemoval();
            m_childFragments.erase(it);
            edit->deleteLater();
            return;
        }
    }
}

/**
 * \brief Writes the \p code to the value part of this fragment
 */
void QmlEditFragment::write(const QString &code){
    ProjectDocument* document = m_declaration->document();

    if ( document->editingStateIs(ProjectDocument::Palette))
        return;

    document->addEditingState(ProjectDocument::Palette);

    {
        QTextCursor tc(document->textDocument());
        tc.setPosition(valuePosition());
        tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
        tc.beginEditBlock();
        tc.removeSelectedText();
        tc.insertText(code);
        tc.endEditBlock();
    }

    document->removeEditingState(ProjectDocument::Palette);
}

/**
 * \brief Reads the code value of this fragment and returns it.
 */
QString QmlEditFragment::readValueText() const{
    QTextCursor tc(m_declaration->document()->textDocument());
    tc.setPosition(valuePosition());
    tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
    return tc.selectedText();
}

void QmlEditFragment::updatePaletteValue(CodePalette *palette){
    QmlBindingChannel::Ptr inputChannel = bindingSpan()->inputChannel();
    if ( !inputChannel )
        return;

    if ( inputChannel->listIndex() == -1 ){
        palette->setValueFromBinding(inputChannel->property().read());
    } else {
        QQmlListReference ppref = qvariant_cast<QQmlListReference>(inputChannel->property().read());
        palette->setValueFromBinding(QVariant::fromValue(ppref.at(inputChannel->listIndex())));
    }
}

QmlEditFragment *QmlEditFragment::parentFragment(){
    return qobject_cast<QmlEditFragment*>(parent());
}

QmlEditFragment *QmlEditFragment::rootFragment(){
    QmlEditFragment* root = parentFragment();
    if ( !root )
        return nullptr;

    while ( root->parentFragment() ){
        root = root->parentFragment();
    }
    return root;
}

void QmlEditFragment::emitRemoval(){
    emit aboutToBeRemoved();

    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* cp = *it;
        cp->deleteLater();
    }

    if ( m_bindingPalette )
        m_bindingPalette->deleteLater();


    m_palettes.clear();
    m_bindingPalette = nullptr;
}

QmlBindingSpanModel* QmlEditFragment::bindingModel(lv::CodeQmlHandler *codeHandler){
    if ( !m_bindingSpanModel ){
        m_bindingSpanModel = new QmlBindingSpanModel(this);
        QString fileName = declaration()->document()->file()->name();
        if ( fileName.length() && fileName.front().isUpper() ){
            m_bindingSpanModel->initializeScanner(codeHandler);
        }
    }
    return m_bindingSpanModel;
}

QString QmlEditFragment::type() const{
    return m_declaration->type().join();
}

QList<QObject *> QmlEditFragment::getChildFragments() const{
    QList<QObject*> result;
    for (QmlEditFragment* edit : m_childFragments)
        result.append(edit);
    return result;
}

void QmlEditFragment::updateValue(){
    QmlBindingChannel::Ptr inputPath = bindingSpan()->inputChannel();

    if ( inputPath && inputPath->listIndex() == -1 ){
        for ( auto it = m_palettes.begin(); it != m_palettes.end(); ++it ){
            CodePalette* cp = *it;
            cp->setValueFromBinding(inputPath->property().read());
        }
        if ( m_bindingPalette ){
            m_bindingPalette->setValueFromBinding(inputPath->property().read());
            QmlCodeConverter* cvt = static_cast<QmlCodeConverter*>(m_bindingPalette->extension());
            cvt->whenBinding().call();
        }
    }
}

void QmlEditFragment::__inputRunnableObjectReady(){
    QmlBindingChannel::Ptr inputChannel = bindingSpan()->inputChannel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(this, SLOT(updateValue()));
    }
}

}// namespace
