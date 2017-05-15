/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qlockedfileiosession.h"
#include "qdocumenthandler.h"
#include "qproject.h"
#include "qcodeconverter.h"

#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QFileInfo>

namespace lcv{

QProjectDocument::QProjectDocument(QProjectFile *file, bool isMonitored, QProject *parent)
    : QObject(parent)
    , m_file(file)
    , m_editingDocument(0)
    , m_isDirty(false)
    , m_isMonitored(isMonitored)
{
    readContent();
    m_file->setDocument(this);
}

void QProjectDocument::dumpContent(const QString &content){
    m_content = content;
    emit contentChanged();
}

void QProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        m_content = parentAsProject()->lockedFileIO()->readFromFile(m_file->path());
        m_lastModified = QFileInfo(m_file->path()).lastModified();
        m_changes.clear();
        emit contentChanged();
    }
}

QProject *QProjectDocument::parentAsProject(){
    return qobject_cast<QProject*>(parent());
}

void QProjectDocument::updateBindings(int position, int charsRemoved, const QString &addedText){
    if ( m_bindings.isEmpty() )
        return;

    int charsAdded = addedText.length();

    QLinkedList<QProjectDocumentBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        QProjectDocumentBinding* binding = *it;

        if ( binding->propertyPosition + binding->length() <= position )
            break;

        // delete bindings that have removed characters inside them
        if ( charsRemoved > 0 &&
             position + charsRemoved > binding->propertyPosition &&
             position < binding->propertyPosition + binding->length())
        {
            int bindingPosition = binding->propertyPosition;
            delete binding;
            it = m_bindings.erase(it);

            if ( m_editingDocument && m_editingDocumentHandler )
                m_editingDocumentHandler->rehighlightBlock(m_editingDocument->findBlock(bindingPosition));

        // delete bindings that have inserted characters inside them
        } else if ( charsAdded > 0 &&
                    position > binding->propertyPosition &&
                    position <= binding->propertyPosition + binding->length() )
        {
            int bindingPosition = binding->propertyPosition;
            delete binding;
            it = m_bindings.erase(it);

            if ( m_editingDocument && m_editingDocumentHandler )
                m_editingDocumentHandler->rehighlightBlock(m_editingDocument->findBlock(bindingPosition));

        //TODO: Remove also when changing chars around the binding.

        } else {
            // update other bindings positions
            binding->propertyPosition = binding->propertyPosition - charsRemoved + charsAdded;


            if ( charsRemoved > 0 && m_editingDocument && !binding->parentBlock ){
                QTextBlock block = m_editingDocument->findBlock(binding->propertyPosition);
                QProjectDocumentBlockData* bd = static_cast<QProjectDocumentBlockData*>(block.userData());
                if ( !bd ){
                    bd = new QProjectDocumentBlockData;
                    block.setUserData(bd);
                }
                bd->addBinding(binding);
            }

            ++it;
        }
    }

    updateBindingBlocks(position, addedText);

}

// solve bindings that are moved from one block to the next
// (ex. when new line is inserted before a bind)
void QProjectDocument::updateBindingBlocks(int position, const QString& addedText){
    if ( m_editingDocument &&
       ( addedText.contains(QChar(QChar::ParagraphSeparator)) ||
         addedText.contains(QChar(QChar::LineFeed))))
    {
        QTextBlock block = m_editingDocument->findBlock(position);
        int blockEnd = block.position() + block.length();

        // iterate current block, see if it has any binds, check if binds are before the end of the block
        QProjectDocumentBlockData* bd = static_cast<QProjectDocumentBlockData*>(block.userData());
        if ( bd && bd->m_bindings.size() > 0 ){
            QProjectDocumentBlockData* bddestination = 0;
            QTextBlock destinationBlock;

            QLinkedList<QProjectDocumentBinding*>::iterator bdit = bd->m_bindings.begin();
            while ( bdit != bd->m_bindings.end() ){
                QProjectDocumentBinding* bdbind = *bdit;

                if ( bdbind->propertyPosition >= blockEnd ){
                    if ( !bddestination ){
                        destinationBlock = m_editingDocument->findBlock(position + addedText.length());
                        bddestination = static_cast<QProjectDocumentBlockData*>(destinationBlock.userData());
                        if ( !bddestination ){
                            bddestination = new QProjectDocumentBlockData;
                            destinationBlock.setUserData(bddestination);
                        }
                    }
                    bdit = bd->m_bindings.erase(bdit);
                    bddestination->addBinding(bdbind);
                    m_editingDocumentHandler->rehighlightBlock(destinationBlock);
                    continue;
                }
                ++bdit;
            }
        }

    }
}

void QProjectDocument::assignEditingDocument(QTextDocument *doc, QDocumentHandler* handler){
    m_editingDocument        = doc;
    m_editingDocumentHandler = handler;
}

QProjectDocumentBinding *QProjectDocument::addNewBinding(int position, int length, const QStringList &propertyChain){

    // bindings are added in descending order according to their position
    QLinkedList<QProjectDocumentBinding*>::iterator it = m_bindings.begin();
    QProjectDocumentBinding* binding = 0;
    while( it != m_bindings.end() ){
        QProjectDocumentBinding* itBind = *it;

        if ( itBind->propertyPosition < position ){
            binding = new QProjectDocumentBinding(this);
            binding->propertyPosition = position;
            binding->propertyLength   = length;
            binding->propertyChain    = propertyChain;
            break;

        // do not add the same binding
        } else if ( itBind->propertyPosition == position ){
            return 0;
        }
        ++it;
    }
    if ( it == m_bindings.end() ){
        binding = new QProjectDocumentBinding(this);
        binding->propertyPosition = position;
        binding->propertyLength   = length;
        binding->propertyChain    = propertyChain;
    }
    if ( binding ){
        m_bindings.insert(it, binding);
    }

    if ( m_editingDocument && binding ){
        QTextBlock bl = m_editingDocument->findBlock(position);
        QProjectDocumentBlockData* blockdata = static_cast<QProjectDocumentBlockData*>(bl.userData());
        if ( !blockdata ){
            blockdata = new QProjectDocumentBlockData;
            bl.setUserData(blockdata);
        }
        blockdata->addBinding(binding);
    }

    return binding;
}

void QProjectDocument::documentContentsChanged(int position, int charsRemoved, const QString &addedText){
    updateBindings(position, charsRemoved, addedText);
    bool hasPendingChange = m_changes.size() > 0 && m_changes.last().commited == false;
    if ( charsRemoved == 0 && addedText.length() == 1 && !addedText[0].isSpace() ){
        if ( hasPendingChange )
            m_changes.last().charsAdded.append(addedText);
        else {
            m_changes.append(QProjectDocumentAction(position, addedText, "", false));
        }
    } else {
        if (hasPendingChange){
            m_changes.last().commited = true;
        }
        m_changes.append(
            QProjectDocumentAction(position, addedText, m_content.mid(position, charsRemoved), false)
        );
    }
    if ( m_changes.size() > 100 )
        m_changes.removeFirst();
}

void QProjectDocument::documentContentsSilentChanged(int position, int charsRemoved, const QString &addedText){
    QLinkedList<QProjectDocumentBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        QProjectDocumentBinding* binding = *it;

        if ( binding->propertyPosition <= position )
            break;

        binding->propertyPosition = binding->propertyPosition - charsRemoved + addedText.length();

        if ( charsRemoved > 0 && !binding->parentBlock ){
            QTextBlock block = m_editingDocument->findBlock(binding->propertyPosition);
            QProjectDocumentBlockData* bd = static_cast<QProjectDocumentBlockData*>(block.userData());
            if ( !bd ){
                bd = new QProjectDocumentBlockData;
                block.setUserData(bd);
            }
            bd->addBinding(binding);
        }

        ++it;
    }
    updateBindingBlocks(position, addedText);
}

QProjectDocumentBinding *QProjectDocument::bindingAt(int position){
    QLinkedList<QProjectDocumentBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        QProjectDocumentBinding* binding = *it;

        if ( binding->propertyPosition == position )
            return binding;

        if ( binding->propertyPosition < position )
            return 0;

        ++it;
    }
    return 0;
}

bool QProjectDocument::removeBindingAt(int position){
    QLinkedList<QProjectDocumentBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        QProjectDocumentBinding* binding = *it;

        if ( binding->propertyPosition == position ){
            m_bindings.erase(it);
            delete binding;
            return true;
        }

        if ( binding->propertyPosition < position )
            return false;

        ++it;
    }
    return false;
}

void QProjectDocument::updateBindingValue(QProjectDocumentBinding *binding, const QString& value){
    if ( binding->valuePositionOffset != -1 ){

        int from = binding->propertyPosition + binding->propertyLength + binding->valuePositionOffset;
        int to   = from + binding->valueLength;

        if ( m_editingDocument ){
            QTextCursor editCursor(m_editingDocument);
            editCursor.setPosition(from);
            editCursor.setPosition(to, QTextCursor::KeepAnchor);

            binding->valueLength      = value.length();
            binding->modifiedByEngine = true;

            m_editingDocumentHandler->addEditingState(QDocumentHandler::Runtime);
            editCursor.beginEditBlock();
            editCursor.removeSelectedText();
            editCursor.insertText(value);
            editCursor.endEditBlock();
            m_editingDocumentHandler->removeEditingState(QDocumentHandler::Runtime);
        } else {
            m_content.replace(from, binding->valueLength, value);
            setIsDirty(true);
        }

    }
}

bool QProjectDocument::isActive() const{
    QProject* prj = qobject_cast<QProject*>(parent());
    if ( prj && prj->active() == this )
        return true;
    return false;
}

bool QProjectDocument::save(){
    if ( m_file->path() != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(m_file->path(), m_content ) ){
            setIsDirty(false);
            m_lastModified = QDateTime::currentDateTime();
            if ( parentAsProject() )
                emit parentAsProject()->fileChanged(m_file->path());
            return true;
        }
    }
    return false;
}

bool QProjectDocument::saveAs(const QString &path){
    if ( m_file->path() == path ){
        save();
    } else if ( path != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(path, m_content ) ){
            QProjectFile* file = parentAsProject()->relocateDocument(m_file->path(), path, this);
            if ( file ){
                m_file->setDocument(0);
                m_file = file;
                emit fileChanged();
            }
            setIsDirty(false);
            m_lastModified = QDateTime::currentDateTime();
            return true;
        }
    }
    return false;
}

bool QProjectDocument::saveAs(const QUrl &url){
    return saveAs(url.toLocalFile());
}

QProjectDocument::~QProjectDocument(){
    if ( m_file->parent() == 0 )
        m_file->deleteLater();
    else {
        m_file->setDocument(0);
    }
}

void QProjectDocumentAction::undo(){

}

void QProjectDocumentAction::redo(){

}

QProjectDocumentBlockData::~QProjectDocumentBlockData(){
    foreach ( QProjectDocumentBinding* binding, m_bindings ){
        binding->parentBlock = 0;
    }
}

void QProjectDocumentBlockData::addBinding(QProjectDocumentBinding *binding){
    binding->parentBlock = this;
    m_bindings.append(binding);
}

void QProjectDocumentBlockData::removeBinding(QProjectDocumentBinding *binding){
    m_bindings.removeOne(binding);
    if ( binding->parentBlock == this )
        binding->parentBlock = 0;
}

QProjectDocumentBinding::QProjectDocumentBinding(QProjectDocument *parent)
    : QObject(parent)
    , propertyPosition(-1)
    , propertyLength(0)
    , valuePositionOffset(-1)
    , valueLength(0)
    , modifiedByEngine(false)
    , parentBlock(0)
    , m_document(parent)
    , m_converter(0)
{}

QProjectDocumentBinding::~QProjectDocumentBinding(){
    if ( parentBlock ){
        parentBlock->removeBinding(this);
    }
}

void QProjectDocumentBinding::updateValue(){
    if ( m_converter )
        m_document->updateBindingValue(
            this, m_converter->serialize()->toCode(sender()->property(propertyChain.last().toUtf8()))
        );
}

}// namespace
