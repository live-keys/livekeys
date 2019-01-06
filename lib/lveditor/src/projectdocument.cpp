/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
#include "live/visuallog.h"
#include "textdocumentlayout.h"

#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QFileInfo>

#include <QDebug>

namespace lv{

ProjectDocument::ProjectDocument(ProjectFile *file, bool isMonitored, Project *parent)
    : QObject(parent)
    , m_file(file)
    , m_textDocument(new QTextDocument(this))
    , m_iteratingSections(false)
    , m_lastChange(m_changes.end())
    , m_editingState(0)
    , m_isDirty(false)
    , m_isSynced(true)
    , m_isMonitored(isMonitored)
{
    connect(m_textDocument, &QTextDocument::contentsChange, this, &ProjectDocument::documentContentsChanged);
    readContent();
    m_textDocument->setDocumentLayout(new TextDocumentLayout(m_textDocument));
    m_file->setDocument(this);
}

void ProjectDocument::resetContent(const QString &content){
    m_textDocument->setPlainText(content);
    emit contentChanged();
}

void ProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        addEditingState(ProjectDocument::Read);
        m_textDocument->setPlainText(
            QString::fromStdString(parentAsProject()->lockedFileIO()->readFromFile(m_file->path().toStdString()))
        );
        removeEditingState(ProjectDocument::Read);
        m_lastModified = QFileInfo(m_file->path()).lastModified();
        m_changes.clear();
        m_lastChange = m_changes.end();
        m_isSynced = true;
        emit contentChanged();
    }
}

Project *ProjectDocument::parentAsProject(){
    return qobject_cast<Project*>(parent());
}

/**
 * @brief ProjectDocument::updateSections
 *
 * Sections are iterated in descending order from their position. If the text between a section changes,
 * the textChanged handler is notified. If the handler requests an invalidation, invalidations are parsed
 * at the end of the iteration, by looking up the m_setctionsToRemove list.
 * @param position
 * @param charsRemoved
 * @param addedText
 */
void ProjectDocument::updateSections(int position, int charsRemoved, const QString &addedText){
    if ( m_sections.isEmpty() )
        return;

    // Invalidation scenario

    m_iteratingSections = true;

    int charsAdded = addedText.length();

    ProjectDocument::SectionIterator it = m_sections.begin();
    while( it != m_sections.end() ){
        ProjectDocumentSection::Ptr& section = *it;
        if ( section->position() + section->length() <= position )
            break;

        int sectionPosition = section->position();

        // notify sections that have removed characters inside them
        if ( section->isValid() ){
            if ( charsRemoved > 0 &&
                 position + charsRemoved > section->position() &&
                 position < section->position() + section->length())
            {
                if ( section->m_textChangedHandler )
                    section->m_textChangedHandler(section, position, charsRemoved, addedText);
                else
                    section->invalidate();

            } else if ( charsAdded > 0 &&
                        position > section->position() &&
                        position <= section->position() + section->length() )
            {
                if ( section->m_textChangedHandler )
                    section->m_textChangedHandler(section, position, charsRemoved, addedText);
                else
                    section->invalidate();

            } else {
                // update other bindings positions
                section->m_position = section->position() - charsRemoved + charsAdded;

                if ( charsRemoved > 0 && m_textDocument && !section->parentBlock() ){
                    QTextBlock block = m_textDocument->findBlock(section->position());
                    ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());
                    if ( !bd ){
                        bd = new ProjectDocumentBlockData;
                        block.setUserData(bd);
                    }
                    bd->addSection(section);
                }
            }
        }

        if ( !section->isValid() ){

            it = m_sections.erase(it);

            QTextBlock tb = m_textDocument->findBlock(sectionPosition);
            emit formatChanged(tb.position(), tb.length());
        } else {
            ++it;
        }
    }

    m_iteratingSections = false;

    while ( !m_sectionsToRemove.isEmpty() ){
        ProjectDocumentSection::Ptr se = m_sectionsToRemove.takeLast();
        removeSection(se);
    }

    updateSectionBlocks(position, addedText);

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

void ProjectDocument::updateSectionBlocks(int position, const QString &addedText){
    QTextBlock block = m_textDocument->findBlock(position);
    QTextBlock otherBlock = m_textDocument->findBlock(position + addedText.length());
    if ( m_sections.size() > 0 &&
         m_textDocument &&
       // ( addedText.contains(QChar(QChar::ParagraphSeparator)) ||
       //  addedText.contains(QChar(QChar::LineFeed))))
        block.blockNumber() != otherBlock.blockNumber())
    {
        int blockEnd = block.position() + block.length();

        // iterate current block, see if it has any binds, check if binds are before the end of the block
        ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());

        if ( bd && bd->m_sections.size() > 0 ){
            ProjectDocumentBlockData* bddestination = nullptr;
            QTextBlock destinationBlock;

            QLinkedList<ProjectDocumentSection::Ptr>::iterator seit = bd->m_sections.begin();
            while ( seit != bd->m_sections.end() ){
                ProjectDocumentSection::Ptr& itSection = *seit;

                if ( itSection->position() >= blockEnd ){
                    if ( !bddestination ){
                        destinationBlock = m_textDocument->findBlock(position + addedText.length());
                        bddestination = static_cast<ProjectDocumentBlockData*>(destinationBlock.userData());
                        if ( !bddestination ){
                            bddestination = new ProjectDocumentBlockData;
                            destinationBlock.setUserData(bddestination);
                        }
                    }
                    seit = bd->m_sections.erase(seit);
                    bddestination->addSection(itSection);
                    emit formatChanged(destinationBlock.position(), destinationBlock.length());
                    continue;
                }
                ++seit;
            }
        }
    }
}

QString ProjectDocument::getCharsRemoved(int position, int count){
    if ( count > 0 ){
        syncContent();
        QTextCursor c(m_textDocument);
        c.setPosition(position);
        c.setPosition(position + count, QTextCursor::MoveMode::KeepAnchor);
        return c.selectedText();
    }
    return "";
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


ProjectDocumentSection::Ptr ProjectDocument::createSection(int type, int position, int length){
    ProjectDocumentSection::Ptr section = ProjectDocumentSection::create(this, type, position, length);

    // sections are added in descending order according to their position
    SectionIterator it = m_sections.begin();

    while( it != m_sections.end() ){
        ProjectDocumentSection::Ptr& itSection = *it;

        if ( itSection->position() < section->position() )
            break;

        ++it;
    }
    m_sections.insert(it, section);

    if ( m_textDocument ){
        QTextBlock bl = m_textDocument->findBlock(section->position());
        ProjectDocumentBlockData* blockdata = static_cast<ProjectDocumentBlockData*>(bl.userData());
        if ( !blockdata ){
            blockdata = new ProjectDocumentBlockData;
            bl.setUserData(blockdata);
        }
        blockdata->addSection(section);
    }

    return section;
}

ProjectDocumentSection::Ptr ProjectDocument::sectionAt(int position){
    SectionIterator it = m_sections.begin();
    while( it != m_sections.end() ){
        ProjectDocumentSection::Ptr& section = *it;
        if ( section->position() == position )
            return section;

        if ( section->position() < position )
            return ProjectDocumentSection::Ptr(nullptr);

        ++it;
    }
    return ProjectDocumentSection::Ptr(nullptr);
}

bool ProjectDocument::removeSectionAt(int position){
    SectionIterator it = m_sections.begin();
    while( it != m_sections.end() ){
        ProjectDocumentSection::Ptr& section = *it;

        if ( section->position() == position ){
            section->invalidate();
            m_sections.erase(it);
            return true;
        }

        if ( section->position() < position )
            return false;

        ++it;
    }
    return false;
}

void ProjectDocument::removeSection(ProjectDocumentSection::Ptr section){
    int sectionPosition = section->position();
    section->invalidate();
    if ( m_iteratingSections ){
        m_sectionsToRemove.append(section);
    } else {
        SectionIterator it = m_sections.begin();
        while( it != m_sections.end() ){
            ProjectDocumentSection::Ptr& currentSection = *it;
            if ( currentSection.data() == section.data() ){
                m_sections.erase(it);

                if ( m_textDocument ){
                    QTextBlock tb = m_textDocument->findBlock(sectionPosition);
                    emit formatChanged(tb.position(), tb.length());
                }

                return;
            }
            ++it;
        }
    }
}

bool ProjectDocument::isActive() const{
    Project* prj = qobject_cast<Project*>(parent());
    if ( prj && prj->active() == this )
        return true;
    return false;
}

void ProjectDocument::documentContentsChanged(int position, int charsRemoved, int charsAdded){
    QString addedText = "";
    if ( charsAdded == 1 ){
        QChar c = m_textDocument->characterAt(position);
        if ( c == DocumentHandler::ParagraphSeparator )
            c = DocumentHandler::NewLine;
        addedText = c;
    } else if ( charsAdded > 0 ){
        QTextCursor cursor(m_textDocument);
        cursor.setPosition(position);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        addedText = cursor.selection().toPlainText();
    }

    updateMarkers(position, charsRemoved, addedText.size());
    updateSections(position, charsRemoved, addedText);
}

bool ProjectDocument::save(){
    syncContent();
    if ( m_file->path() != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(m_file->path().toStdString(), m_textDocument->toPlainText().toStdString() ) ){
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
        if ( parentAsProject()->lockedFileIO()->writeToFile(path.toStdString(), m_textDocument->toPlainText().toStdString() ) ){
            ProjectFile* file = parentAsProject()->relocateDocument(m_file->path(), path, this);
            if ( file ){
                m_file->setDocument(nullptr);
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
    if ( m_file->parent() == nullptr )
        m_file->deleteLater();
    else {
        m_file->setDocument(nullptr);
    }
}

void ProjectDocumentAction::undo(){
//    parent->m_content.replace(position, charsAdded.size(), charsRemoved);
}

void ProjectDocumentAction::redo(){
//    parent->m_content.replace(position, charsRemoved.size(), charsAdded);
}

ProjectDocumentBlockData::ProjectDocumentBlockData() : m_collapseState(NoCollapse), m_numOfCollapsedLines(0)
    , m_stateChangeFlag(false) {}

ProjectDocumentBlockData::~ProjectDocumentBlockData(){
    foreach ( const ProjectDocumentSection::Ptr& section, m_sections ){
        section->m_parentBlock = nullptr;
    }
}

ProjectDocumentSection::~ProjectDocumentSection(){
}

void ProjectDocumentSection::onTextChanged(
        std::function<void(ProjectDocumentSection::Ptr, int, int, const QString &)> handler)
{
    m_textChangedHandler = handler;
}

void ProjectDocumentSection::invalidate(){
    m_position = -1;
    if ( m_parentBlock )
        m_parentBlock->removeSection(this);
}

void ProjectDocumentBlockData::addSection(ProjectDocumentSection::Ptr section){
    section->m_parentBlock = this;
    m_sections.append(section);
}

void ProjectDocumentBlockData::removeSection(ProjectDocumentSection::Ptr section){
    removeSection(section.data());
}

void ProjectDocumentBlockData::removeSection(ProjectDocumentSection *section){
    for ( auto it = m_sections.begin(); it != m_sections.end(); ++it ){
        if ( it->data() == section ){
            m_sections.erase(it);
            return;
        }
    }
    if ( section->m_parentBlock == this )
        section->m_parentBlock = nullptr;
}

void ProjectDocumentBlockData::setCollapse(CollapseState state, CollapseFunctionType func)
{
    m_collapseState = state;
    m_onCollapse = func;

}

void ProjectDocumentBlockData::setNumOfCollapsedLines(int num)
{
    m_numOfCollapsedLines = num;
}

void ProjectDocumentBlockData::setReplacementString(QString &repl)
{
    m_replacementString = repl;
}

ProjectDocumentBlockData::CollapseState ProjectDocumentBlockData::collapseState() { return m_collapseState; }
int ProjectDocumentBlockData::numOfCollapsedLines() { return m_numOfCollapsedLines; }
CollapseFunctionType ProjectDocumentBlockData::onCollapse() { return m_onCollapse; }
QString& ProjectDocumentBlockData::replacementString() { return m_replacementString; }

void ProjectDocumentBlockData::resetCollapseParams()
{
	m_collapseState = NoCollapse;
	m_replacementString = QString();

}

} // namespace

