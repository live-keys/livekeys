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

#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/lockedfileiosession.h"
#include "live/documenthandler.h"
#include "live/project.h"
#include "live/codeconverter.h"
#include "live/codedeclaration.h"
#include "live/coderuntimebinding.h"

#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QFileInfo>

#include <QDebug>

namespace lv{

ProjectDocument::ProjectDocument(ProjectFile *file, bool isMonitored, Project *parent)
    : QObject(parent)
    , m_file(file)
    , m_editingDocument(0)
    , m_lastChange(m_changes.end())
    , m_isDirty(false)
    , m_isSynced(true)
    , m_isMonitored(isMonitored)
{
    readContent();
    m_file->setDocument(this);
}

void ProjectDocument::dumpContent(const QString &content){
    m_content = content;
    emit contentChanged(0);
}

void ProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        m_content = parentAsProject()->lockedFileIO()->readFromFile(m_file->path());
        m_lastModified = QFileInfo(m_file->path()).lastModified();
        m_changes.clear();
        m_lastChange = m_changes.end();
        m_isSynced = true;
        emit contentChanged(0);
    }
}

Project *ProjectDocument::parentAsProject(){
    return qobject_cast<Project*>(parent());
}

void ProjectDocument::updateBindings(int position, int charsRemoved, const QString &addedText){
    if ( m_bindings.isEmpty() )
        return;

    int charsAdded = addedText.length();

    QLinkedList<CodeRuntimeBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        CodeRuntimeBinding* binding = *it;

        if ( binding->position() + binding->length() <= position )
            break;

        // delete bindings that have removed characters inside them
        if ( charsRemoved > 0 &&
             position + charsRemoved > binding->position() &&
             position < binding->position() + binding->length())
        {
            int bindingPosition = binding->position();
            delete binding;
            it = m_bindings.erase(it);

            if ( m_editingDocument && m_editingDocumentHandler )
                m_editingDocumentHandler->rehighlightBlock(m_editingDocument->findBlock(bindingPosition));

        // delete bindings that have inserted characters inside them
        } else if ( charsAdded > 0 &&
                    position > binding->position() &&
                    position <= binding->position() + binding->length() )
        {
            int bindingPosition = binding->position();
            delete binding;
            it = m_bindings.erase(it);

            if ( m_editingDocument && m_editingDocumentHandler )
                m_editingDocumentHandler->rehighlightBlock(m_editingDocument->findBlock(bindingPosition));

        } else {
            // update other bindings positions
            binding->declaration()->setIdentifierPosition(binding->position() - charsRemoved + charsAdded);


            if ( charsRemoved > 0 && m_editingDocument && !binding->parentBlock() ){
                QTextBlock block = m_editingDocument->findBlock(binding->position());
                ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());
                if ( !bd ){
                    bd = new ProjectDocumentBlockData;
                    block.setUserData(bd);
                }
                bd->addBinding(binding);
            }

            ++it;
        }
    }

    updateBindingBlocks(position, addedText);

}

void ProjectDocument::updateMarkers(int position, int charsRemoved, int charsAdded){
    if ( m_markers.isEmpty() )
        return;

    auto it = m_markers.begin();
    while ( it != m_markers.end() ){
        ProjectDocumentMarker::Ptr& marker = *it;
        if ( marker->m_position <= position )
            break;

        if ( charsRemoved > 0 && marker->position() <= position + charsRemoved ){
            marker->invalidate();
            it = m_markers.erase(it);
        } else {
            marker->m_position = marker->m_position - charsRemoved + charsAdded;
        }
        ++it;
    }
}

// solve bindings that are moved from one block to the next
// (ex. when new line is inserted before a bind)
void ProjectDocument::updateBindingBlocks(int position, const QString& addedText){
    if ( m_editingDocument &&
       ( addedText.contains(QChar(QChar::ParagraphSeparator)) ||
         addedText.contains(QChar(QChar::LineFeed))))
    {
        QTextBlock block = m_editingDocument->findBlock(position);
        int blockEnd = block.position() + block.length();

        // iterate current block, see if it has any binds, check if binds are before the end of the block
        ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());
        if ( bd && bd->m_bindings.size() > 0 ){
            ProjectDocumentBlockData* bddestination = 0;
            QTextBlock destinationBlock;

            QLinkedList<CodeRuntimeBinding*>::iterator bdit = bd->m_bindings.begin();
            while ( bdit != bd->m_bindings.end() ){
                CodeRuntimeBinding* bdbind = *bdit;

                if ( bdbind->position() >= blockEnd ){
                    if ( !bddestination ){
                        destinationBlock = m_editingDocument->findBlock(position + addedText.length());
                        bddestination = static_cast<ProjectDocumentBlockData*>(destinationBlock.userData());
                        if ( !bddestination ){
                            bddestination = new ProjectDocumentBlockData;
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

QString ProjectDocument::getCharsRemoved(int position, int count){
    if ( count > 0 ){
        syncContent();
        return m_content.mid(position, count);
    }
    return "";
}

void ProjectDocument::assignEditingDocument(QTextDocument *doc, DocumentHandler* handler){
    syncContent();

    m_editingDocument        = doc;
    m_editingDocumentHandler = handler;
}

CodeRuntimeBinding *ProjectDocument::addNewBinding(CodeDeclaration::Ptr declaration){
    if ( declaration->document() != this ){
        qCritical("Runtime binding requested on a different document.");
        return 0;
    }

    // bindings are added in descending order according to their position
    QLinkedList<CodeRuntimeBinding*>::iterator it = m_bindings.begin();
    CodeRuntimeBinding* binding = 0;
    while( it != m_bindings.end() ){
        CodeRuntimeBinding* itBind = *it;

        if ( itBind->position() < declaration->position() ){
            binding = new CodeRuntimeBinding(declaration);
            break;

        // do not add the same binding
        } else if ( itBind->position() == declaration->position() ){
            return 0;
        }
        ++it;
    }
    if ( it == m_bindings.end() ){
        binding = new CodeRuntimeBinding(declaration);
    }
    if ( binding ){
        m_bindings.insert(it, binding);
    }

    if ( m_editingDocument && binding ){
        QTextBlock bl = m_editingDocument->findBlock(declaration->position());
        ProjectDocumentBlockData* blockdata = static_cast<ProjectDocumentBlockData*>(bl.userData());
        if ( !blockdata ){
            blockdata = new ProjectDocumentBlockData;
            bl.setUserData(blockdata);
        }
        blockdata->addBinding(binding);
    }

    return binding;
}

void ProjectDocument::documentContentsChanged(
        DocumentHandler* author,
        int position,
        int charsRemoved,
        const QString &addedText)
{
    updateMarkers(position, charsRemoved, addedText.size());
    updateBindings(position, charsRemoved, addedText);
    bool hasPendingChange = m_changes.size() > 0 && m_changes.last().commited == false;
    bool isPendingChangeCompatible =
            hasPendingChange &&
            (m_changes.last().position + m_changes.last().charsAdded.size() == position) &&
            charsRemoved == 0 && addedText.length() == 1 && !addedText[0].isSpace();

    if ( hasPendingChange && isPendingChangeCompatible ){
        if ( m_lastChange == m_changes.end() ){
            --m_lastChange;
            m_lastChange->undo();
        }
        m_changes.last().charsAdded.append(addedText);
    } else {
        if (hasPendingChange){
            m_changes.last().commited = true;
        }
        m_changes.append(
            ProjectDocumentAction(this, position, addedText, getCharsRemoved(position, charsRemoved), false)
        );
        if ( m_lastChange == m_changes.end() )
            --m_lastChange;
    }
    if ( m_changes.size() > 100 ){
        if ( m_lastChange == m_changes.begin() && m_lastChange != m_changes.end() ){
            m_lastChange->redo();
            ++m_lastChange;
        }
        m_changes.removeFirst();
    }

    setIsDirty(true);

    resetSync();
    emit contentChanged(author);
}

void ProjectDocument::documentContentsSilentChanged(
        DocumentHandler* author,
        int position,
        int charsRemoved,
        const QString &addedText)
{
    updateMarkers(position, charsRemoved, addedText.size());
    bool hasPendingChange = m_changes.size() > 0 && m_changes.last().commited == false;
    bool isPendingChangeCompatible = hasPendingChange && m_changes.last().position == position;

    if ( hasPendingChange && isPendingChangeCompatible ){
        if ( m_lastChange == m_changes.end() ){
            --m_lastChange;
            m_lastChange->undo();
        }
        m_changes.last().charsAdded = addedText;
    } else {
        if (hasPendingChange){
            m_changes.last().commited = true;
        }
        m_changes.append(
            ProjectDocumentAction(this, position, addedText, m_content.mid(position, charsRemoved), false)
        );
        if ( m_lastChange == m_changes.end() )
            --m_lastChange;
    }
    if ( m_changes.size() > 100 ){
        if ( m_lastChange == m_changes.begin() && m_lastChange != m_changes.end() ){
            m_lastChange->redo();
            ++m_lastChange;
        }
        m_changes.removeFirst();
    }

    QLinkedList<CodeRuntimeBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        CodeRuntimeBinding* binding = *it;

        if ( binding->position() <= position )
            break;

        binding->declaration()->setIdentifierPosition(binding->position() - charsRemoved + addedText.length());

        if ( charsRemoved > 0 && !binding->parentBlock() ){
            QTextBlock block = m_editingDocument->findBlock(binding->position());
            ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());
            if ( !bd ){
                bd = new ProjectDocumentBlockData;
                block.setUserData(bd);
            }
            bd->addBinding(binding);
        }

        ++it;
    }
    updateBindingBlocks(position, addedText);

    setIsDirty(true);

    resetSync();
    emit contentChanged(author);
}

ProjectDocumentMarker::Ptr ProjectDocument::addMarker(int position){
    // markers are added in descending order according to their position
    auto it = m_markers.begin();
    while( it != m_markers.end() ){
        if ( (*it)->position() <= position )
            break;
        ++it;
    }
    ProjectDocumentMarker::Ptr result(new ProjectDocumentMarker(position));
    m_markers.insert(it, result);
    return result;
}

void ProjectDocument::removeMarker(ProjectDocumentMarker::Ptr marker){
    auto it = m_markers.begin();
    while ( it != m_markers.end() ){
        if ( (*it).data() == marker.data() ){
            m_markers.erase(it);
            return;
        }
        ++it;
    }
}

CodeRuntimeBinding *ProjectDocument::bindingAt(int position){
    QLinkedList<CodeRuntimeBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        CodeRuntimeBinding* binding = *it;

        if ( binding->position() == position )
            return binding;

        if ( binding->position() < position )
            return 0;

        ++it;
    }
    return 0;
}

bool ProjectDocument::removeBindingAt(int position){
    QLinkedList<CodeRuntimeBinding*>::iterator it = m_bindings.begin();
    while( it != m_bindings.end() ){
        CodeRuntimeBinding* binding = *it;

        if ( binding->position() == position ){
            m_bindings.erase(it);
            delete binding;
            return true;
        }

        if ( binding->position() < position )
            return false;

        ++it;
    }
    return false;
}

void ProjectDocument::updateBindingValue(CodeRuntimeBinding *binding, const QString& value){
    if ( binding->declaration()->valueOffset() != -1 ){

        int from = binding->position() + binding->declaration()->identifierLength() + binding->declaration()->valueOffset();
        int to   = from + binding->declaration()->valueLength();

        if ( m_editingDocument ){
            QTextCursor editCursor(m_editingDocument);
            editCursor.setPosition(from);
            editCursor.setPosition(to, QTextCursor::KeepAnchor);

            binding->declaration()->setValueLength(value.length());

            m_editingDocumentHandler->addEditingState(DocumentHandler::Runtime);
            editCursor.beginEditBlock();
            editCursor.removeSelectedText();
            editCursor.insertText(value);
            editCursor.endEditBlock();
            m_editingDocumentHandler->removeEditingState(DocumentHandler::Runtime);
        } else {
            m_content.replace(from, binding->declaration()->valueLength(), value);
            setIsDirty(true);
        }

    }
}

bool ProjectDocument::isActive() const{
    Project* prj = qobject_cast<Project*>(parent());
    if ( prj && prj->active() == this )
        return true;
    return false;
}

bool ProjectDocument::save(){
    syncContent();
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

bool ProjectDocument::saveAs(const QString &path){
    if ( m_file->path() == path ){
        save();
    } else if ( path != "" ){
        syncContent();
        if ( parentAsProject()->lockedFileIO()->writeToFile(path, m_content ) ){
            ProjectFile* file = parentAsProject()->relocateDocument(m_file->path(), path, this);
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

bool ProjectDocument::saveAs(const QUrl &url){
    return saveAs(url.toLocalFile());
}

void ProjectDocument::syncContent() const{
    while ( m_lastChange != m_changes.end() ){
        m_lastChange->redo();
        ++m_lastChange;
    }
    m_isSynced = true;
}

ProjectDocument::~ProjectDocument(){
    if ( m_file->parent() == 0 )
        m_file->deleteLater();
    else {
        m_file->setDocument(0);
    }
}

void ProjectDocumentAction::undo(){
    parent->m_content.replace(position, charsAdded.size(), charsRemoved);
}

void ProjectDocumentAction::redo(){
    parent->m_content.replace(position, charsRemoved.size(), charsAdded);
}

ProjectDocumentBlockData::~ProjectDocumentBlockData(){
    foreach ( CodeRuntimeBinding* binding, m_bindings ){
        binding->m_parentBlock = 0;
    }
}

void ProjectDocumentBlockData::addBinding(CodeRuntimeBinding *binding){
    binding->m_parentBlock = this;
    m_bindings.append(binding);
}

void ProjectDocumentBlockData::removeBinding(CodeRuntimeBinding *binding){
    m_bindings.removeOne(binding);
    if ( binding->m_parentBlock == this )
        binding->m_parentBlock = 0;
}

}// namespace
